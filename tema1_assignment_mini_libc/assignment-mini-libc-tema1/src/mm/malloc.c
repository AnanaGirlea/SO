// SPDX-License-Identifier: BSD-3-Clause

#include <internal/mm/mem_list.h>
#include <internal/types.h>
#include <internal/essentials.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

void *malloc(size_t size)
{
	/* TODO: Implement malloc(). - DONE */
	void *sol = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if (sol == NULL)
		return NULL;

	mem_list_add(sol, size);

	return sol;
}

void *calloc(size_t nmemb, size_t size)
{
	/* TODO: Implement calloc(). - DONE */
	size_t new_size = nmemb * size;

	void *sol = mmap(NULL, new_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if (sol == NULL)
		return NULL;

	memset(sol, 0, new_size);
	mem_list_add(sol, new_size);

	return sol;
}

void free(void *ptr)
{
	/* TODO: Implement free(). - DONE */
	struct mem_list *list_free = mem_list_find(ptr);

	if (list_free == NULL) {
		return;
	}

	munmap(ptr, list_free->len);
	mem_list_del(list_free->start);
}

void *realloc(void *ptr, size_t size)
{
	/* TODO: Implement realloc(). - DONE */
	void *sol = NULL;

	if (size == NULL) {
		sol = NULL;
		free(ptr);
	} else {
		if (ptr == NULL) {
			return malloc(size);
		} else {
			struct mem_list *new_list = mem_list_find(ptr);
			if(new_list == NULL) {
				return NULL;
			} else {
				sol = mremap(ptr, new_list->len, size, 0);
				if (sol == MAP_FAILED)
					return NULL;
				mem_list_del(ptr);
				mem_list_add(sol, size);
			}
		}
	}

	return sol;
}

void *reallocarray(void *ptr, size_t nmemb, size_t size)
{
	/* TODO: Implement reallocarray(). - DONE */
	return realloc(ptr, nmemb*size);
}
