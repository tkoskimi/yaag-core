// Memory Management Module
//
// @author Tuomas Koskimies

#include <stdlib.h>

#ifdef TEST
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <cmocka.h>
#endif

void *mem_malloc(size_t size) {
#ifdef TEST
    return test_malloc( size );
#else
    return malloc( size );
#endif
}

void mem_free(void *ptr) {
#ifdef TEST
    return test_free( ptr );
#else
    return free( ptr );
#endif
}
