#include <fcntl.h>
#include <internal/syscall.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>

int nanosleep(const struct timespec *req, struct timespec *rem)
{
	int sol = syscall(__NR_nanosleep, req, rem);
	if (sol < 0) {
		errno = - sol;
		return -1;
	}

	return sol;
}
