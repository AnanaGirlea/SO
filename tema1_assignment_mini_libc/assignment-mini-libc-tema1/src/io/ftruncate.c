// SPDX-License-Identifier: BSD-3-Clause

#include <unistd.h>
#include <internal/syscall.h>
#include <errno.h>

int ftruncate(int fd, off_t length)
{
	/* TODO: Implement ftruncate(). - DONE */
	int sol = syscall(__NR_ftruncate, fd, length);

	if (sol < 0) {
		errno = - sol;
		return -1;
	}

	return sol;
}
