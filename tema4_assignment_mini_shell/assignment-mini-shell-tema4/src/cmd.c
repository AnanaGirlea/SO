// SPDX-License-Identifier: BSD-3-Clause

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <fcntl.h>
#include <unistd.h>

#include "cmd.h"
#include "utils.h"

#define READ 0
#define WRITE 1

/**
 * Internal change-directory command.
 */
static bool shell_cd(word_t *dir)
{
	/* TODO: Execute cd. - DONE */
	if (dir == NULL || dir->string == NULL)
		return -1;

	int cd = chdir(dir->string);

	if (cd == -1)
		return -1;

	return 0;
}

/**
 * Internal exit/quit command.
 */
static int shell_exit(void)
{
	/* TODO: Execute exit/quit. - DONE */
	return SHELL_EXIT;
}

/**
 * Parse a simple command (internal, environment variable assignment,
 * external command).
 */
static int parse_simple(simple_command_t *s, int level, command_t *father)
{
	/* TODO: Sanity checks. - DONE */
	if (s == NULL)
		return -1;

	/* TODO: If builtin command, execute the command. - DONE */
	if (strcmp(s->verb->string, "cd") == 0) {
		if (s->out != NULL) {
			int fcd = open(get_word(s->out), O_CREAT, 0666);

			close(fcd);
		}

		int sol = shell_cd(s->params);

		return sol;
	}

	if (strcmp(s->verb->string, "exit") == 0 || strcmp(s->verb->string, "quit") == 0) {
		shell_exit();

		return SHELL_EXIT;
	}

	/* TODO: If variable assignment, execute the assignment and return
	 * the exit status. - DONE
	 */
	if (s->verb->next_part != NULL && strcmp(s->verb->next_part->string, "=") == 0) {
		int new_env;

		if (s->verb->next_part->next_part != NULL) {
			new_env = setenv(s->verb->string, get_word(s->verb->next_part->next_part), 1);

			return new_env;
		}
		new_env = setenv(s->verb->string, "", 1);

		return new_env;
	}

	/* TODO: If external command: - DONE
	 *   1. Fork new process
	 *     2c. Perform redirections in child
	 *     3c. Load executable in child
	 *   2. Wait for child
	 *   3. Return exit status
	 */
	pid_t pid_fork = fork();

	if (pid_fork == -1)
		return -1;

	if (pid_fork == 0) {
		if (s->in != NULL) {
			int fin = open(get_word(s->in), O_RDONLY);

			dup2(fin, STDIN_FILENO);
			close(fin);
		}
		if (s->out != NULL) {
			int fout;

			if (s->io_flags == IO_OUT_APPEND)
				fout = open(get_word(s->out), O_WRONLY | O_CREAT | O_APPEND, 0644);
			else
				fout = open(get_word(s->out), O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0644);

			dup2(fout, STDOUT_FILENO);
			close(fout);
		}
		if (s->err != NULL) {
			int ferr;

			if (s->io_flags == IO_ERR_APPEND)
				ferr = open(get_word(s->err), O_WRONLY | O_CREAT | O_APPEND, 0644);
			else
				ferr = open(get_word(s->err), O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0644);

			dup2(ferr, STDERR_FILENO);
			close(ferr);
		}
		int size = 0;
		char **arg = get_argv(s, &size);
		int exe = execvp(get_word(s->verb), arg);

		printf("Execution failed for '%s'\n", get_word(s->verb));
		exit(exe);
	}
	int status;

	waitpid(pid_fork, &status, 0);

	return WEXITSTATUS(status);
}

/**
 * Process two commands in parallel, by creating two children.
 */
static bool run_in_parallel(command_t *cmd1, command_t *cmd2, int level,
														command_t *father)
{
	/* TODO: Execute cmd1 and cmd2 simultaneously. - DONE */
	pid_t pid_fork_1 = fork();

	if (pid_fork_1 == -1)
		return -1;

	if (pid_fork_1 == 0) {
		int sol = parse_command(cmd1, level, father);

		exit(sol);
	}

	pid_t pid_fork_2 = fork();

	if (pid_fork_2 == -1)
		return -1;

	if (pid_fork_2 == 0) {
		int sol = parse_command(cmd2, level, father);

		exit(sol);
	}

	int status_1, status_2;

	waitpid(pid_fork_1, &status_1, 0);
	waitpid(pid_fork_2, &status_2, 0);

	if (WIFEXITED(status_1) && WIFEXITED(status_2))
		return WEXITSTATUS(status_1) && WEXITSTATUS(status_2);
	else if (WIFEXITED(status_1))
		return WEXITSTATUS(status_1);
	else if (WIFEXITED(status_2))
		return WEXITSTATUS(status_2);
	else
		return -1;
}

/**
 * Run commands by creating an anonymous pipe (cmd1 | cmd2).
 */
static bool run_on_pipe(command_t *cmd1, command_t *cmd2, int level,
												command_t *father)
{
	/* TODO: Redirect the output of cmd1 to the input of cmd2. - DONE */
	int fd[2];

	if (pipe(fd) == -1)
		return -1;

	pid_t pid_fork_1 = fork();

	if (pid_fork_1 == -1)
		return -1;

	if (pid_fork_1 == 0) {
		close(fd[READ]);
		dup2(fd[WRITE], STDOUT_FILENO);

		int sol = parse_command(cmd1, level, father);

		close(fd[WRITE]);
		exit(sol);
	}

	pid_t pid_fork_2 = fork();

	if (pid_fork_2 == -1)
		return -1;

	if (pid_fork_2 == 0) {
		close(fd[WRITE]);
		dup2(fd[READ], STDIN_FILENO);

		int sol = parse_command(cmd2, level, father);

		close(fd[READ]);
		exit(sol);
	}
	close(fd[READ]);
	close(fd[WRITE]);

	int status_1, status_2;

	waitpid(pid_fork_1, &status_1, 0);
	waitpid(pid_fork_2, &status_2, 0);

	return WEXITSTATUS(status_2);
}

/**
 * Parse and execute a command.
 */
int parse_command(command_t *c, int level, command_t *father)
{
	/* TODO: sanity checks - DONE */
	if (c == NULL)
		return -1;

	int sol, sol_1, sol_2;

	if (c->op == OP_NONE) {
		/* TODO: Execute a simple command. - DONE */
		sol = parse_simple(c->scmd, level, father);

		return sol;
	}

	switch (c->op) {
	case OP_SEQUENTIAL:
		/* TODO: Execute the commands one after the other. - DONE */
		sol_1 = parse_command(c->cmd1, level, c);
		sol_2 = parse_command(c->cmd2, level, c);

		return sol_2;

	case OP_PARALLEL:
		/* TODO: Execute the commands simultaneously. - DONE */
		sol = run_in_parallel(c->cmd1, c->cmd2, level, c);

		return sol;

	case OP_CONDITIONAL_NZERO:
		/* TODO: Execute the second command only if the first one
		 * returns non zero. - DONE
		 */
		sol_1 = parse_command(c->cmd1, level, c);

		if (sol_1 != 0) {
			sol_2 = parse_command(c->cmd2, level, c);

			return sol_2;
		}

		return sol_1;

	case OP_CONDITIONAL_ZERO:
		/* TODO: Execute the second command only if the first one
		 * returns zero. - DONE
		 */
		sol_1 = parse_command(c->cmd1, level, c);

		if (sol_1 == 0) {
			sol_2 = parse_command(c->cmd2, level, c);

			return sol_2;
		}

		return sol_1;

	case OP_PIPE:
		/* TODO: Redirect the output of the first command to the
		 * input of the second. - DONE
		 */
		sol = run_on_pipe(c->cmd1, c->cmd2, level, c);

		return sol;

	default:
		return SHELL_EXIT;
	}

	return 0;
}
