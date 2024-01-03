#include <fcntl.h>
#include <internal/syscall.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>

int sleep(unsigned int seconds)
{
    struct timespec req, rem;
    req.tv_sec = seconds;
    req.tv_nsec = 0;

    int sol = nanosleep(&req, &rem);

	return rem.tv_sec;
}
