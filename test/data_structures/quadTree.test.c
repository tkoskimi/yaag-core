#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <cmocka.h>

#include "../../src/mem.h"
#include "../../src/data_structures/quad_tree.h"

typedef struct {
    QStruct* q;
} QuadTest;

static int quad_setup(void **state) {
    QuadTest *test_struct = test_malloc( sizeof( QuadTest ) );
    test_struct->q = quad_new();
    *state = test_struct;

    return 0;
}

static int quad_teardown(void **state) {
    quad_free( ( ( QuadTest * ) *state )->q );
    test_free( *state );

    return 0;
}

static void bit_masks(void **state) {
    assert_int_equal( 0x0, _bit_mask_011(0) );
    assert_int_equal( 0x1, _bit_mask_011(1) );
    assert_int_equal( 0x7, _bit_mask_011(3) );
    assert_int_equal( 0xff, _bit_mask_011(8) );
    assert_int_equal( 0x1fff, _bit_mask_011(13) );
    assert_int_equal( 0x7fffffff, _bit_mask_011(31) );

    assert_int_equal( 0x0, _bit_mask_010(1,1) );
    assert_int_equal( 0x2, _bit_mask_010(1,2) );
    assert_int_equal( 0x1c, _bit_mask_010(2,5) );
    assert_int_equal( 0x00f0, _bit_mask_010(4,8) );
}

static void test_index(void **state) {
    QStruct* q = ( ( QuadTest * ) *state )->q;
    assert_int_equal( 0x0, quad_point_index(q, 0x0, 0x0));

    int x0;
    int y0;

    x0 = 0x1 << ( REGION_DIM_IN_BITS - DEPTH_OF_QTREE);
    y0 = 0x1 << ( REGION_DIM_IN_BITS - DEPTH_OF_QTREE);
    assert_int_equal( 0x3, quad_point_index(q, x0, y0));

    x0 = 0x3 << ( REGION_DIM_IN_BITS - DEPTH_OF_QTREE);
    y0 = 0x2 << ( REGION_DIM_IN_BITS - DEPTH_OF_QTREE);
    assert_int_equal( 0xe, quad_point_index(q, x0, y0));

    x0 = 0x2 << ( REGION_DIM_IN_BITS - DEPTH_OF_QTREE);
    y0 = 0x3 << ( REGION_DIM_IN_BITS - DEPTH_OF_QTREE);
    assert_int_equal( 0xd, quad_point_index(q, x0, y0));
}

static void test_index_in_custom_q(void **state) {
    QStruct* q = quad_new_and_init( 0x10, 0x10, 5, 2);
    assert_int_equal( 0x0, quad_point_index(q, 0x10, 0x10));
    assert_int_equal( 0xc, quad_point_index(q, 0x20, 0x20));
    assert_int_equal( 0x0, quad_point_index(q, 0x30, 0x20));
    
    int x0;
    int y0;

    x0 = ( 0x1 << ( q->dim - q->depth ) ) + 0x10;
    y0 = ( 0x1 << ( q->dim - q->depth ) ) + 0x10;
    assert_int_equal( 0x3, quad_point_index(q, x0, y0));

    x0 = ( 0x3 << ( q->dim - q->depth ) ) + 0x10;
    y0 = ( 0x2 << ( q->dim - q->depth ) ) + 0x10;
    assert_int_equal( 0xe, quad_point_index(q, x0, y0));

    x0 = ( 0x2 << ( q->dim - q->depth ) ) + 0x10;
    y0 = ( 0x3 << ( q->dim - q->depth ) ) + 0x10;
    assert_int_equal( 0xd, quad_point_index(q, x0, y0));
}

int quad_test() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown( bit_masks, quad_setup, quad_teardown ),
        cmocka_unit_test_setup_teardown( test_index, quad_setup, quad_teardown ),
        cmocka_unit_test_setup_teardown( test_index_in_custom_q, quad_setup, quad_teardown ),
    };

    return cmocka_run_group_tests( tests, NULL, NULL );
}
