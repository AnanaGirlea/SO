// SPDX-License-Identifier: BSD-3-Clause

#include <unistd.h>
#include <internal/syscall.h>
#include <stdarg.h>
#include <errno.h>

int close(int fd)
{
	/* TODO: Implement close(). - DONE */
	int sol = syscall(__NR_close, fd);

	if (sol < 0) {
		errno = - sol;
		return -1;
	}

	return sol;
}
