// SPDX-License-Identifier: BSD-3-Clause

#include <sys/stat.h>
#include <errno.h>
#include <internal/syscall.h>

int fstat(int fd, struct stat *st)
{
	/* TODO: Implement fstat(). - DONE */
	int sol = syscall(__NR_fstat, fd, st);

	if (sol < 0) {
		errno = - sol;
		return -1;
	}

	return sol;
}
