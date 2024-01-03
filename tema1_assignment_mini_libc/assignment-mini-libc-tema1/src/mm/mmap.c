// SPDX-License-Identifier: BSD-3-Clause

#include <sys/mman.h>
#include <errno.h>
#include <internal/syscall.h>


void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
	/* TODO: Implement mmap(). - DONE */
	long sol = syscall(__NR_mmap, addr, length, prot, flags, fd, offset);
	if (sol < 0) {
		errno = - sol;
		return MAP_FAILED;
	}

	return (void *)sol;
}

void *mremap(void *old_address, size_t old_size, size_t new_size, int flags)
{
	/* TODO: Implement mremap(). - DONE */
	long sol = syscall(__NR_mremap, old_address, old_size, new_size, flags);
	if (sol < 0) {
		errno = - sol;
		return MAP_FAILED;
	}

	return (void *)sol;
}

int munmap(void *addr, size_t length)
{
	/* TODO: Implement munmap(). - DONE */
	long sol = syscall(__NR_munmap, addr, length);
	if (sol < 0) {
		errno = - sol;
		return -1;
	}

	return 0;
}
