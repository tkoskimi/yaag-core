#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <cmocka.h>

#include "../../src/mem.h"
#include "../../src/data_structures/quad_tree.h"

typedef struct {
    qtree_t* q;
} qtest_t;


//
//   Test Fixtures
//

static int qtree_setup(void **state) {
    qtest_t *test_struct = test_malloc( sizeof( qtest_t ) );
    *state = test_struct;
    return 0;
}

static int qtree_teardown(void **state) {
    test_free( *state );
    return 0;
}

// ************
// bit_mask_01x
// ************

static void bit_mask(void **state) {
    assert_int_equal( 0x0, _bit_mask_011(0) );
    assert_int_equal( 0x1, _bit_mask_011(1) );
    assert_int_equal( 0x7, _bit_mask_011(3) );
    assert_int_equal( 0xff, _bit_mask_011(8) );
    assert_int_equal( 0x1fff, _bit_mask_011(13) );
    assert_int_equal( 0x7fffffff, _bit_mask_011(31) );
    assert_int_equal( 0x0, _bit_mask_010(1,1) );
    assert_int_equal( 0x2, _bit_mask_010(1,2) );
    assert_int_equal( 0x3, _bit_mask_010(0,2) );
    assert_int_equal( 0x1c, _bit_mask_010(2,5) );
    assert_int_equal( 0x00f0, _bit_mask_010(4,8) );
}

// *****************
// qtree_point_index
// *****************

static void point_index(void **state) {
    qtree_t* q = qtree_new();
    int x0;
    int y0;

    //           <--------...----| REGION_DIM_IN_BITS (e.g. 10)
    //                 <--...----| REGION_DIM_IN_BITS - DEPTH_OF_QTREE (e.g. 7)
    // +-+-+-+...+-+-+-+-+...+-+-+
    // |0|0|0|   |0|0|1|0|   |0|0| x0
    // +-+-+-+...+-+-+-+-+...+-+-+
    // |0|0|0|   |0|0|1|0|   |0|0| y0
    // +-+-+-+...+-+-+-+-+...+-+-+
    // ===========================
    //               +-+-+-+-+-+-+
    //               |0|0|0|0|1|1| 0x3
    //               +-+-+-+-+-+-+
    x0 = 0x1 << ( REGION_DIM_IN_BITS - DEPTH_OF_QTREE);
    y0 = 0x1 << ( REGION_DIM_IN_BITS - DEPTH_OF_QTREE);
    assert_int_equal( 0x3, qtree_point_index(q, x0, y0));

    // +-+-+-+...+-+-+-+-+...+-+-+
    // |0|0|0|   |0|1|1|0|   |0|0| x0
    // +-+-+-+...+-+-+-+-+...+-+-+
    // |0|0|0|   |0|1|0|0|   |0|0| y0
    // +-+-+-+...+-+-+-+-+...+-+-+
    // ===========================
    //               +-+-+-+-+-+-+
    //               |0|0|1|1|1|0| 0xe 
    //               +-+-+-+-+-+-+
    x0 = 0x3 << ( REGION_DIM_IN_BITS - DEPTH_OF_QTREE);
    y0 = 0x2 << ( REGION_DIM_IN_BITS - DEPTH_OF_QTREE);
    assert_int_equal( 0xe, qtree_point_index(q, x0, y0));

    // +-+-+-+...+-+-+-+-+...+-+-+
    // |0|0|0|   |0|1|0|0|   |0|0| x0
    // +-+-+-+...+-+-+-+-+...+-+-+
    // |0|0|0|   |0|1|1|0|   |0|0| y0
    // +-+-+-+...+-+-+-+-+...+-+-+
    // ===========================
    //               +-+-+-+-+-+-+
    //               |0|0|1|1|0|1| 0xd 
    //               +-+-+-+-+-+-+
    x0 = 0x2 << ( REGION_DIM_IN_BITS - DEPTH_OF_QTREE);
    y0 = 0x3 << ( REGION_DIM_IN_BITS - DEPTH_OF_QTREE);
    assert_int_equal( 0xd, qtree_point_index(q, x0, y0));

    // Clean-up.
    qtree_free( q );
}

static void point_index_in_custom_q(void **state) {
    qtree_t* q = qtree_new();
    qtree_init( q, 0x10, 0x10, 5, 2);

    assert_int_equal( 0x0, qtree_point_index( q, 0x10, 0x10 ) );
    assert_int_equal( 0xc, qtree_point_index( q, 0x20, 0x20 ) );
    assert_int_equal( -1, qtree_point_index( q, 0x30, 0x20 ) );
    
    int x0;
    int y0;

    x0 = ( 0x1 << ( q->dim - q->depth ) ) + 0x10;
    y0 = ( 0x1 << ( q->dim - q->depth ) ) + 0x10;
    assert_int_equal( 0x3, qtree_point_index(q, x0, y0));

    x0 = ( 0x3 << ( q->dim - q->depth ) ) + 0x10;
    y0 = ( 0x2 << ( q->dim - q->depth ) ) + 0x10;
    assert_int_equal( 0xe, qtree_point_index(q, x0, y0));

    x0 = ( 0x2 << ( q->dim - q->depth ) ) + 0x10;
    y0 = ( 0x3 << ( q->dim - q->depth ) ) + 0x10;
    assert_int_equal( 0xd, qtree_point_index(q, x0, y0));

    qtree_free( q );
}

static void point_index_max_in_custom_q(void **state) {
    qtree_t* q = qtree_new();
    qtree_init( q, 0x0, 0x0, 16, 8);
    int x0;
    int y0;

    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // |0|0|0|0|0|0|0|0|1|0|0|0|0|1|1|1| 0x87
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // |0|0|0|0|0|0|0|0|0|1|1|1|0|1|0|0| 0x74
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // |1|0|0|1|0|1|0|1|0|0|1|1|1|0|1|0| 0x953a
    x0 = ( 0x87 << ( q->dim - q->depth ) );
    y0 = ( 0x74 << ( q->dim - q->depth ) );
    assert_int_equal( 0x953a, qtree_point_index(q, x0, y0));

    qtree_free( q );
}

static void node_path_max_in_custom_q(void **state) {
    qtree_t* q = qtree_new();
    qtree_init( q, 0x0, 0x0, 16, 8);
    char* name = NULL;

    name = qtree_node_path( q, 0x953a, 0x953a );
    assert_string_equal( "10.01.01.01.00.11.10.10", name );
    mem_free( name );

    qtree_free( q );
}

static void point_index_min_in_custom_q(void **state) {
    qtree_t* q = qtree_new();
    qtree_init( q, 0x0, 0x0, 1, 1);
    int x0;
    int y0;

    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // |0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0| 0x0
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // |0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|1| 0x1
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // |0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|1| 0x1
    x0 = ( 0x0 << ( q->dim - q->depth ) );
    y0 = ( 0x1 << ( q->dim - q->depth ) );
    assert_int_equal( 0x1, qtree_point_index(q, x0, y0));

    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // |0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|1| 0x1
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // |0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|1| 0x1
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // |0|0|0|0|0|0|0|0|0|0|0|0|0|0|1|1| 0x3
    x0 = ( 0x1 << ( q->dim - q->depth ) );
    y0 = ( 0x1 << ( q->dim - q->depth ) );
    assert_int_equal( 0x3, qtree_point_index(q, x0, y0));

    qtree_free( q );
}

static void node_path_min_in_custom_q(void **state) {
    qtree_t* q = qtree_new();
    qtree_init( q, 0x0, 0x0, 1, 1);
    char* name = NULL;

    name = qtree_node_path( q, 0x1, 0x1 );
    assert_string_equal( "01", name );
    mem_free( name );

    name = qtree_node_path( q, 0x3, 0x3 );
    assert_string_equal( "11", name );
    mem_free( name );

    qtree_free( q );
}

static void node_path(void **state) {
    qtree_t* q = qtree_new();

    assert_null( qtree_node_path(q, -1, 0));
    assert_null( qtree_node_path(q, 0, -1));
    assert_null( qtree_node_path(q, -1, -1));

    char* name = NULL;

    name = qtree_node_path( q, 0x8, 0x4 );
    assert_string_equal( "00", name );
    mem_free( name );

    name = qtree_node_path( q, 0x4, 0x8 );
    assert_string_equal( "00", name );
    mem_free( name );

    name = qtree_node_path( q, 0x2, 0x1 );
    assert_string_equal( "00.00", name );
    mem_free( name );

    name = qtree_node_path( q, 0x1, 0x2 );
    assert_string_equal( "00.00", name );
    mem_free( name );

    name = qtree_node_path( q, 0x0, 0x0 );
    assert_string_equal( "00.00.00", name );
    mem_free( name );

    name = qtree_node_path( q, 0x3, 0x3 );
    assert_string_equal( "00.00.11", name );
    mem_free( name );

    name = qtree_node_path( q, 0x24, 0x24 );
    assert_string_equal( "10.01.00", name );
    mem_free( name );

    qtree_free( q );
}

static void node_path_in_custom_q(void **state) {
    qtree_t* q = qtree_new();
    qtree_init( q, 0x10, 0x10, 6, 4);
    char* name = NULL;

    //                 <---- 2 * 4 ---->
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // |0|0|0|0|0|0|0|0|0|0|0|1|1|0|1|1| 0x1b
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    name = qtree_node_path( q, 0x1b, 0x1b );
    assert_string_equal( "00.01.10.11", name );
    mem_free( name );

    qtree_free( q );
}

static void parse_null(void **state) {
    assert_null( qtree_split_path( NULL ) );
}

static void parse_a(void **state) {
    char** strs = qtree_split_path( "a" );
    assert_string_equal( "a", strs[0] );
    assert_null( strs[1] );

    // Clean-up
    test_free( strs[0] );
    test_free( strs );
}

static void parse_abc(void **state) {
    char** strs = qtree_split_path( "a.b.c" );
    assert_string_equal( "a", strs[0] );
    assert_string_equal( "b", strs[1] );
    assert_string_equal( "c", strs[2] );
    assert_null( strs[3] );

    // Clean-up
    test_free( strs[0] );
    test_free( strs[1] );
    test_free( strs[2] );
    test_free( strs );
}

static void parse_name_long(void **state) {
    char *path = "itisamax.b";
    char** strs = qtree_split_path( path );
    assert_string_equal( "itisamax", strs[0] );
    assert_string_equal( "b", strs[1] );

    // Clean-up
    test_free( strs[0] );
    test_free( strs[1] );
    test_free( strs );
}

static void parse_name_too_long(void **state) {
    char *path = "thisistoolong.b";
    char** strs = qtree_split_path( path );
    assert_null( strs );
    return;
}

static void parse_dot_ends(void **state) {
    char *path = "a.";
    char** strs = qtree_split_path( path );
    assert_null( strs );
}

static void parse_dot_starts(void **state) {
    char *path = ".a";
    char** strs = qtree_split_path( path );
    assert_null( strs );
    return;
}

static void parse_tree_deep(void **state) {
    char *path = "a.b.c.d.e.f.g.h";
    char** strs = qtree_split_path( path );
    assert_string_equal( "a", strs[0] );
    assert_string_equal( "b", strs[1] );
    assert_string_equal( "c", strs[2] );
    assert_string_equal( "d", strs[3] );
    assert_string_equal( "e", strs[4] );
    assert_string_equal( "f", strs[5] );
    assert_string_equal( "g", strs[6] );
    assert_string_equal( "h", strs[7] );

    // Clean-up
    test_free( strs[0] );
    test_free( strs[1] );
    test_free( strs[2] );
    test_free( strs[3] );
    test_free( strs[4] );
    test_free( strs[5] );
    test_free( strs[6] );
    test_free( strs[7] );
    test_free( strs );
}

static void parse_tree_too_deep(void **state) {
    char *path = "a.b.c.d.e.f.g.h.i";
    char** strs = qtree_split_path( path );
    assert_null( strs );
}

int qtree_test() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown( bit_mask, qtree_setup, qtree_teardown ),
        cmocka_unit_test_setup_teardown( point_index, qtree_setup, qtree_teardown ),
        cmocka_unit_test_setup_teardown( point_index_in_custom_q, qtree_setup, qtree_teardown ),
        cmocka_unit_test_setup_teardown( point_index_max_in_custom_q, qtree_setup, qtree_teardown ),
        cmocka_unit_test_setup_teardown( node_path_max_in_custom_q, qtree_setup, qtree_teardown ),
        cmocka_unit_test_setup_teardown( point_index_min_in_custom_q, qtree_setup, qtree_teardown ),
        cmocka_unit_test_setup_teardown( node_path_min_in_custom_q, qtree_setup, qtree_teardown ),
        cmocka_unit_test_setup_teardown( node_path, qtree_setup, qtree_teardown ),
        cmocka_unit_test_setup_teardown( node_path_in_custom_q, qtree_setup, qtree_teardown ),
        cmocka_unit_test_setup_teardown( parse_null, qtree_setup, qtree_teardown ),
        cmocka_unit_test_setup_teardown( parse_a, qtree_setup, qtree_teardown ),
        cmocka_unit_test_setup_teardown( parse_abc, qtree_setup, qtree_teardown ),
        cmocka_unit_test_setup_teardown( parse_name_long, qtree_setup, qtree_teardown ),
        cmocka_unit_test_setup_teardown( parse_dot_ends, qtree_setup, qtree_teardown ),
        cmocka_unit_test_setup_teardown( parse_dot_starts, qtree_setup, qtree_teardown ),
        cmocka_unit_test_setup_teardown( parse_name_too_long, qtree_setup, qtree_teardown ),
        cmocka_unit_test_setup_teardown( parse_tree_deep, qtree_setup, qtree_teardown ),
        cmocka_unit_test_setup_teardown( parse_tree_too_deep, qtree_setup, qtree_teardown ),
    };

    return cmocka_run_group_tests( tests, NULL, NULL );
}
