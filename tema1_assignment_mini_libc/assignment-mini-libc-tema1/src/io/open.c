// SPDX-License-Identifier: BSD-3-Clause

#include <fcntl.h>
#include <internal/syscall.h>
#include <stdarg.h>
#include <errno.h>

int open(const char *filename, int flags, ...)
{
	/* TODO: Implement open system call. - DONE */
	va_list valist;
	int a;

	va_start(valist, flags);
	a = va_arg(valist, int);
	va_end(valist);

	int sol = syscall(__NR_open, filename, flags, a);

	if (sol < 0) {
		errno = - sol;
		return -1;
	}

	return sol;
}
