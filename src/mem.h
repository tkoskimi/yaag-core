// Memory Manager
//
// This memory manager provides a simple and fast memory allocator.
//
// (c) Tuomas Koskimies, 2018

#ifndef _mem_
#define _mem_

#include <stdlib.h>

void *mem_malloc(size_t size);

void mem_free(void *ptr);

//void *mem_malloc_lin(struct Block **block, size_t size);

//void mem_free_lin(void *ptr);

#endif // _mem_