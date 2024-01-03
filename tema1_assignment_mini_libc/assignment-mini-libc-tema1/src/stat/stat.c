// SPDX-License-Identifier: BSD-3-Clause

#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <internal/syscall.h>

int stat(const char *restrict path, struct stat *restrict buf)
{
	/* TODO: Implement stat(). - DONE */
	int sol = syscall(__NR_stat, path, buf);

	if (sol < 0) {
		errno = - sol;
		return -1;
	}

	return sol;
}
