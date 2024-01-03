// SPDX-License-Identifier: BSD-3-Clause

#include "osmem.h"
#include <assert.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include "helpers.h"

#define memory_allignment 8
#define ALIGN(size)                                                            \
	(((size) + (memory_allignment - 1)) & ~(memory_allignment - 1))
#define META_SIZE sizeof(struct block_meta)

#define MMAP_THRESHOLD (128 * 1024)
#define PAGE_SIZE getpagesize()

struct block_meta block_meta;
void *head;

struct block_meta *find_free_block(struct block_meta **last, size_t size)
{
	struct block_meta *current = head;
	size_t min_dif = MMAP_THRESHOLD;
	struct block_meta *ret = NULL;

	while (current) {
		*last = current;
		if (current->status == STATUS_FREE && current->size >= size &&
				current->size - size < min_dif) {
			min_dif = current->size - size;
			ret = current;
		}
		current = current->next;
	}

	return ret;
}

struct block_meta *request_space(struct block_meta *last, size_t size)
{
	if (last != NULL && last->status == STATUS_FREE) {
		void *request = sbrk(ALIGN(size) - last->size);

		if (request == (void *) -1)
			return NULL;

		last->size = ALIGN(size);

		return last;
	}

	struct block_meta *block;

	block = sbrk(0);

	void *request = sbrk(ALIGN(size) + ALIGN(META_SIZE));

	assert((void *)block == request);
	if (request == (void *)-1)
		return NULL;

	if (last)
		last->next = block;

	block->size = ALIGN(size);
	block->next = NULL;
	block->status = STATUS_FREE;

	return block;
}

void split_block(struct block_meta *block, size_t size)
{
	if (block->size < (ALIGN(size) + ALIGN(META_SIZE) + 8))
		return;

	size_t new_size = ALIGN(block->size - ALIGN(size) - ALIGN(META_SIZE));

	struct block_meta *new_block_meta =
			(struct block_meta *)((char *)block + ALIGN(META_SIZE) + ALIGN(size));
	new_block_meta->size = new_size;
	new_block_meta->status = STATUS_FREE;
	new_block_meta->next = block->next;

	block->size = ALIGN(size);
	block->next = new_block_meta;
}

void coalesce_block(struct block_meta *block)
{
	while (block != NULL && block->status == STATUS_FREE && block->next != NULL &&
				 block->next->status == STATUS_FREE) {
		size_t new_size = ALIGN(block->size + block->next->size + ALIGN(META_SIZE));

		block->size = new_size;
		block->next = block->next->next;
	}
}

void coalesce_blocks(void)
{
	struct block_meta *block = head;

	while (block != NULL) {
		coalesce_block(block);
		block = block->next;
	}
}

struct block_meta *get_block_ptr(void *ptr)
{
	return (struct block_meta *)(ptr - ALIGN(META_SIZE));
}

void *os_malloc(size_t size)
{
	/* TODO: Implement os_malloc - DONE */
	struct block_meta *block;
	size_t new_size = ALIGN(size);
	size_t new_full_size = new_size + ALIGN(META_SIZE);

	// Case 1: size of block is below 0
	if (new_size <= 0)
		return NULL;

	// Case 2: size of block is bigger than MMAP_THRESHOLD
	if (new_full_size >= MMAP_THRESHOLD) {
		void *sol = mmap(0, new_full_size, PROT_READ | PROT_WRITE,
										 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		DIE(sol == MAP_FAILED, "mmap failed");

		struct block_meta *new_block_meta = (struct block_meta *)sol;

		new_block_meta->size = new_size;
		new_block_meta->status = STATUS_MAPPED;
		new_block_meta->next = NULL;

		return (void *)(new_block_meta + 1);
	}
	coalesce_blocks();

	// Case 3: size of block is smaller than MMAP_THRESHOLD
	if (head == NULL) {
		block = request_space(NULL, MMAP_THRESHOLD - ALIGN(META_SIZE));

		if (block == NULL)
			return NULL;

		head = block;
	} else {
		struct block_meta *last = head;

		block = find_free_block(&last, new_size);

		if (block == NULL) {
			block = request_space(last, new_size);
			if (block == NULL)
				return NULL;
		}
	}
	split_block(block, size);

	block->status = STATUS_ALLOC;

	return (void *)(block + 1);
}

void os_free(void *ptr)
{
	/* TODO: Implement os_free - DONE */
	// Case 1: block is empty
	if (ptr == NULL)
		return;

	// Case 2: freeeing mapped memory blocks from Memory Mapping Segment
	if (((struct block_meta *)(ptr - ALIGN(META_SIZE)))->status ==
			STATUS_MAPPED) {
		munmap((ptr - ALIGN(META_SIZE)),
					 ALIGN(((struct block_meta *)(ptr - ALIGN(META_SIZE)))->size +
								 META_SIZE));
	} else {
		// Case 3: freeeing mapped memory blocks from Heap (allocated using sbrk)
		struct block_meta *block_ptr = get_block_ptr(ptr);

		assert(block_ptr->status == STATUS_ALLOC);
		block_ptr->status = STATUS_FREE;
	}
}

void *os_calloc(size_t nmemb, size_t size)
{
	/* TODO: Implement os_calloc - DONE */
	struct block_meta *block;

	// Case 1: size of block or number of blocks are below 0
	if (nmemb <= 0 || size <= 0)
		return NULL;

	size_t new_size = ALIGN(size * nmemb);
	size_t new_full_size = ALIGN(new_size + ALIGN(META_SIZE));

	// Case 2: size of block is bigger than PAGE_SIZE
	if (new_full_size >= (size_t)PAGE_SIZE) {
		void *sol = mmap(0, new_full_size, PROT_READ | PROT_WRITE,
										 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		DIE(sol == MAP_FAILED, "mmap failed");

		struct block_meta *new_block_meta = (struct block_meta *)sol;

		new_block_meta->size = new_size;
		new_block_meta->status = STATUS_MAPPED;
		new_block_meta->next = NULL;

		memset(new_block_meta + 1, 0, new_full_size);
		return (void *)(new_block_meta + 1);
	}
	coalesce_blocks();

	// Case 3: size of block is smaller than PAGE_SIZE
	if (head == NULL) {
		block = request_space(NULL, MMAP_THRESHOLD - ALIGN(META_SIZE));

		if (block == NULL)
			return NULL;

		head = block;
	} else {
		struct block_meta *last = head;

		block = find_free_block(&last, new_size);

		if (block == NULL) {
			block = request_space(last, new_size);

			if (block == NULL)
				return NULL;
		}
	}
	split_block(block, new_size);

	block->status = STATUS_ALLOC;
	memset(block + 1, 0, new_size);

	return (void *)(block + 1);
}

void *os_realloc(void *ptr, size_t size)
{
	/* TODO: Implement os_realloc - DONE */
	struct block_meta *block = get_block_ptr(ptr);
	size_t new_size = ALIGN(size);
	size_t new_full_size = new_size + ALIGN(META_SIZE);

	// Case 1: block is NULL or size of block is below 0
	if (new_size == 0) {
		os_free(ptr);
		return NULL;
	}
	if (ptr == NULL)
		return os_malloc(new_size);

	if (block->status == STATUS_FREE)
		return NULL;

	if (block->size == new_size)
		return ptr;

	// Case 2: block in Memory Mapping Segment
	if (block->status == STATUS_MAPPED) {
		void *new_ptr = os_malloc(new_size);

		if (new_ptr == NULL)
			return NULL;

		if (block->size > new_size)
			memcpy(new_ptr, ptr, new_size);
		else
			memcpy(new_ptr, ptr, block->size);

		os_free(ptr);
		return new_ptr;
	}

	// Case 3: block is in Heap
	if (block->status == STATUS_ALLOC) {
		// Case 3.1: old block is in Heap, new size needs to be in Memory Mapping
		if (new_full_size >= MMAP_THRESHOLD) {
			void *new_ptr = os_malloc(new_size);

			if (new_ptr == NULL)
				return NULL;

			memcpy(new_ptr, ptr, block->size);

			os_free(ptr);
			return new_ptr;
		}
		// Case 3.2: old block is in Heap, new size needs to be in Heap
		while (new_size > block->size && block->next != NULL &&
						block->next->status == STATUS_FREE) {
			block->size = ALIGN(block->size + block->next->size + ALIGN(META_SIZE));
			block->next = block->next->next;
		}
		if (new_size <= block->size) {
			split_block(block, new_size);
			return ptr;
		}
		if (block->next == NULL) {
			void *request = sbrk(new_size - block->size);

			if (request == (void *)-1)
				return NULL;

			block->size = new_size;

			return ptr;
		}
		void *new_ptr = os_malloc(new_size);

		if (new_ptr == NULL)
			return NULL;

		memcpy(new_ptr, ptr, block->size);
		os_free(ptr);

		return new_ptr;
	}

	return NULL;
}
