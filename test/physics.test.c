#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <cmocka.h>

#include "../src/mem.h"
#include "../src/physics.h"

typedef struct {
    physics_obj_t* p;
} ptest_t;

//  ****************************************
//  Misc functions
//  ****************************************

static void clr_mem_data( void *data ) {
    if ( data ) {
        mem_free( data );
    }
}

//  ****************************************
//   Test Fixtures
//  ****************************************

static int physics_setup(void **state) {
    ptest_t *test_struct = test_malloc( sizeof( ptest_t ) );
    *state = test_struct;
    return 0;
}

static int physics_teardown(void **state) {
    test_free( *state );
    return 0;
}

// ************
// bit_mask_01x
// ************

static void physics_ok(void **state) {
    assert_int_equal( 0, 0 );
}

int physics_test() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown( physics_ok, physics_setup, physics_teardown ),
    };

    return cmocka_run_group_tests( tests, NULL, NULL );
}
