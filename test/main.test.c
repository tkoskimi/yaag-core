#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "./data_structures/doublyLinkedList.test.h"

/* A test case that does nothing and succeeds. */
void null_test_success(void **state) {
    (void) state; /* unused */
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(null_test_success),
        cmocka_unit_test(null_test_1),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
