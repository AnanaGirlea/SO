// SPDX-License-Identifier: BSD-3-Clause

#include <unistd.h>
#include <internal/syscall.h>
#include <errno.h>

int truncate(const char *path, off_t length)
{
	/* TODO: Implement truncate(). - DONE */
	int sol = syscall(__NR_truncate, path, length);

	if (sol < 0) {
		errno = - sol;
		return -1;
	}

	return sol;
}
