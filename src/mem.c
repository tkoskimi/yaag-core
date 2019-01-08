// Memory Management
//
// [Implementation details]
//
// (c) Tuomas Koskimies, 2018

#include <stdlib.h>

void *mem_malloc(size_t size) {
    return malloc(size);
}

void mem_free(void *ptr) {
    return free(ptr);
}
