#include <internal/io.h>
#include <internal/syscall.h>
#include <internal/types.h>
#include <string.h>

int puts(const char *str)
{
    write(1, str, strlen(str));
    write(1, "\n", 1);

    return 1;
}
