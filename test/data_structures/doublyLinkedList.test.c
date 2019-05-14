#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <cmocka.h>

#include "../../src/mem.h"
#include "../../src/data_structures/doublyLinkedList.h"

struct DblTest {
    dbllist_t* list;
};

//  ****************************************
//  Fixtures
//  ****************************************

static int dbll_setup(void **state) {
    struct DblTest *test_struct = test_malloc( sizeof( struct DblTest ) );
    test_struct->list = dbllist_new();
    *state = test_struct;

    return 0;
}

static int dbll_teardown(void **state) {
    dbllist_free( ( ( struct DblTest * ) *state )->list );
    test_free( *state );

    return 0;
}

//  ****************************************
//  Misc functions
//  ****************************************

static int* new_data( int value ) {
    int* new_data = ( int* ) test_malloc( sizeof( int ) );
    *new_data = value;
    return new_data;
}

static void release_node( void* data ) {
}

static void free_data( void* data ) {
    if ( data ) {
        mem_free( data );
    }
}

static void empty_list(void **state) {
    dbllist_t* list = ( ( struct DblTest * ) *state )->list;
    assert_null( dbllist_head( list ) );
    assert_null( dbllist_tail( list ) );
    assert_true( dbllist_is_empty( list ) );
}

static void append_to_two_empty_lists(void **state) {
    dbllist_t* dst = ( ( struct DblTest * ) *state )->list;
    dbllist_t* src = dbllist_new();

    dbllist_append( dst, src );
    assert_true( dbllist_size( dst ) == 0 );

    dbllist_free( src );
}

static void append_to_one_empty_list(void **state) {
    dbllist_t* dst = ( ( struct DblTest * ) *state )->list;
    dbllist_t* src = dbllist_new();

    int *one = test_malloc( sizeof( int ) );
    *one = 1;

    dblnode_t* node1 = dbllist_push( dst, one );

    assert_int_equal( dbllist_size( dst ), 1 );
    assert_int_equal( dbllist_size( src ), 0 );
    assert_ptr_equal( dbllist_head( dst ), node1 );

    dbllist_append( dst, src );

    assert_int_equal( dbllist_size( dst ), 1 );
    assert_int_equal( dbllist_size( src ), 0 );
    assert_ptr_equal( dbllist_head( dst ), node1 );

    test_free( dbllist_pop( dst ) );
    dbllist_free( src );
}

static void append_to_nonempty_list(void **state) {
    dbllist_t* dst = ( ( struct DblTest * ) *state )->list;
    dbllist_t* src = dbllist_new();

    int *one = test_malloc( sizeof( int ) );
    *one = 1;

    int *two = test_malloc( sizeof( int ) );
    *two = 2;

    dblnode_t* node1 = dbllist_push( dst, one );
    dblnode_t* node2 = dbllist_push( src, two );

    assert_int_equal( dbllist_size( dst ), 1 );
    assert_int_equal( dbllist_size( src ), 1 );
    assert_ptr_equal( dbllist_head( dst ), node1 );
    assert_ptr_equal( dbllist_tail( src ), node2 );

    dbllist_append( dst, src );

    assert_int_equal( dbllist_size( dst ), 2 );
    assert_int_equal( dbllist_size( src ), 1 );
    assert_int_equal( 1, * ( (int *) dbllist_head( dst )->data ) );
    assert_int_equal( 2, * ( (int *) dbllist_tail( dst )->data ) );
    assert_int_equal( 2, * ( (int *) dbllist_head( src )->data ) );
    assert_ptr_equal( dbllist_head( dst ), node1 );
    assert_ptr_equal( dbllist_tail( src ), node2 );

    test_free( dbllist_pop( dst ) );
    test_free( dbllist_pop( dst ) );
    dbllist_pop( src );
    dbllist_free( src );
}

static void join_two_empty_lists(void **state) {
    dbllist_t* lst_0 = dbllist_new();
    dbllist_t* lst_1 = dbllist_new();
    // API Call
    dbllist_t* lst = dbllist_join( lst_0, lst_1 );
    // Verify
    assert_non_null( lst );
    assert_int_equal( 0, dbllist_size( lst ) );
    // Clean-up
    dbllist_free( lst );
    dbllist_free( lst_1 );
    dbllist_free( lst_0 );
}

static void join_two_nonempty_lists(void **state) {
    dbllist_t* lst_0 = dbllist_new();
    dbllist_t* lst_1 = dbllist_new();
    int* zero = new_data( 0 );
    int* one = new_data( 1 );
    dbllist_push( lst_0, zero );
    dbllist_push( lst_1, one );
    // API Call
    dbllist_t* lst = dbllist_join( lst_0, lst_1 );
    // Verify
    assert_non_null( lst );
    assert_int_equal( 2, dbllist_size( lst ) );
    assert_ptr_equal( zero, dbllist_head( lst )->data );
    assert_ptr_equal( one, dbllist_tail( lst )->data );
    // Clean-up
    dbllist_remove( lst, NULL );
    dbllist_remove( lst_1, free_data );
    dbllist_remove( lst_0, free_data );
    dbllist_free( lst );
    dbllist_free( lst_1 );
    dbllist_free( lst_0 );
}

static void push_to_empty(void **state) {
    dbllist_t* list = ( ( struct DblTest * ) *state )->list;
    int *value = test_malloc( sizeof( int ) );

    // Insert one value
    *value = 1;
    struct dblnode_t* node = dbllist_push( list, value );

    assert_false( dbllist_is_empty( list ) );
    assert_int_equal( dbllist_size( list ), 1 );
    assert_ptr_equal( dbllist_head( list ), dbllist_tail( list ) );
    assert_ptr_equal( dbllist_head( list ), node );
    assert_null( node->next );
    assert_null( node->prev );

    test_free( dbllist_pop( list ) );
}

static void push_to_non_empty(void **state) {
    dbllist_t* list = ( ( struct DblTest * ) *state )->list;
    int *zero = test_malloc( sizeof( int ) );
    int *one = test_malloc( sizeof( int ) );

    *zero = 0;
    *one = 1;
    struct dblnode_t* node0 = dbllist_push( list, zero );
    struct dblnode_t* node1 = dbllist_push( list, one );

    assert_false( dbllist_is_empty( list ) );
    assert_int_equal( dbllist_size( list ), 2 );
    assert_ptr_equal( dbllist_head( list ), node1 );
    assert_ptr_equal( dbllist_tail( list ), node0 );
    assert_ptr_equal( node1->next, node0 );
    assert_null( node1->prev );
    assert_null( node0->next );
    assert_ptr_equal( node0->prev, node1 );

    test_free( dbllist_pop( list ) );
    test_free( dbllist_pop( list ) );
}

static void push_end_to_empty(void **state) {
    dbllist_t* list = ( ( struct DblTest * ) *state )->list;
    int *value = test_malloc( sizeof( int ) );

    // Insert one value
    *value = 1;
    struct dblnode_t* node = dbllist_push_to_end( list, value );

    assert_false( dbllist_is_empty( list ) );
    assert_int_equal( dbllist_size( list ), 1 );
    assert_ptr_equal( dbllist_head( list ), dbllist_tail( list ) );
    assert_ptr_equal( dbllist_head( list ), node );
    assert_null( node->next );
    assert_null( node->prev );

    test_free( dbllist_pop( list ) );
}

static void push_end_to_non_empty(void **state) {
    dbllist_t* list = ( ( struct DblTest * ) *state )->list;
    int *zero = test_malloc( sizeof( int ) );
    int *one = test_malloc( sizeof( int ) );

    *zero = 0;
    *one = 1;
    struct dblnode_t* node0 = dbllist_push_to_end( list, zero );
    struct dblnode_t* node1 = dbllist_push_to_end( list, one );

    assert_false( dbllist_is_empty( list ) );
    assert_int_equal( dbllist_size( list ), 2 );
    assert_ptr_equal( dbllist_head( list ), node0 );
    assert_ptr_equal( dbllist_tail( list ), node1 );
    assert_ptr_equal( node0->next, node1 );
    assert_ptr_equal( node1->prev, node0 );
    assert_null( node0->prev );
    assert_null( node1->next );

    test_free( dbllist_pop( list ) );
    test_free( dbllist_pop( list ) );
}

static void pop_to_empty(void **state) {
    dbllist_t* list = ( ( struct DblTest * ) *state )->list;
    int *zero = test_malloc( sizeof( int ) );

    *zero = 0;

    struct dblnode_t* node0 = dbllist_push( list, zero );
    void *data = dbllist_pop( list );

    assert_true( dbllist_is_empty( list ) );
    assert_int_equal( dbllist_size( list ), 0 );
    assert_null( dbllist_head( list ) );
    assert_null( dbllist_tail( list ) );
    assert_ptr_equal( data, zero );

    test_free( data );
}

static void pop_to_non_empty(void **state) {
    dbllist_t* list = ( ( struct DblTest * ) *state )->list;
    int *zero = test_malloc( sizeof( int ) );
    int *one = test_malloc( sizeof( int ) );
    int *two = test_malloc( sizeof( int ) );

    *zero = 0;
    *one = 1;
    *two = 2;

    struct dblnode_t* node0 = dbllist_push( list, zero );
    struct dblnode_t* node1 = dbllist_push( list, one );
    struct dblnode_t* node2 = dbllist_push( list, two );

    void *data = dbllist_pop( list );

    assert_false( dbllist_is_empty( list ) );
    assert_int_equal( dbllist_size( list ), 2 );
    assert_ptr_equal( dbllist_head( list ), node1 );
    assert_ptr_equal( dbllist_tail( list ), node0 );
    assert_ptr_equal( node1->next, node0 );
    assert_null( node1->prev );
    assert_null( node0->next );
    assert_ptr_equal( node0->prev, node1 );
    assert_ptr_equal( data, two );

    test_free( data );
    test_free( dbllist_pop( list ) );
    test_free( dbllist_pop( list ) );
}

static void remove_from_empty(void **state) {
    dbllist_t* list = ( ( struct DblTest * ) *state )->list;

    int *zero = test_malloc( sizeof( int ) );

    *zero = 0;

    assert_int_equal( 0, dbllist_size( list ) );
    assert_null( dbllist_delete( list, zero ) );

    test_free( zero );
}

static void remove_the_first(void **state) {
    dbllist_t* list = ( ( struct DblTest * ) *state )->list;

    int *zero = test_malloc( sizeof( int ) );

    *zero = 0;

    struct dblnode_t* node0 = dbllist_push( list, zero );

    assert_int_equal( dbllist_size( list ), 1 );
    assert_ptr_equal( zero, dbllist_delete( list, zero ) );
    assert_int_equal( dbllist_size( list ), 0 );
    assert_null( dbllist_head( list ) );
    assert_null( dbllist_tail( list ) );

    test_free( zero );
}

static void remove_the_last(void **state) {
    dbllist_t* list = ( ( struct DblTest * ) *state )->list;
    int *zero = test_malloc( sizeof( int ) );
    int *one = test_malloc( sizeof( int ) );
    int *two = test_malloc( sizeof( int ) );

    *zero = 0;
    *one = 1;
    *two = 2;

    struct dblnode_t* node0 = dbllist_push( list, zero );
    struct dblnode_t* node1 = dbllist_push( list, one );
    struct dblnode_t* node2 = dbllist_push( list, two );

    assert_int_equal( dbllist_size( list ), 3 );
    assert_ptr_equal( zero, dbllist_delete( list, zero ) );
    assert_int_equal( dbllist_size( list ), 2 );
    assert_ptr_equal( dbllist_head( list ), node2 );
    assert_ptr_equal( dbllist_tail( list ), node1 );
    assert_ptr_equal( node1->prev, node2 );
    assert_null( node1->next );

    test_free( zero );
    test_free( dbllist_pop( list ) );
    test_free( dbllist_pop( list ) );
}

static void remove_the_middle(void **state) {
    dbllist_t* list = ( ( struct DblTest * ) *state )->list;
    int *zero = test_malloc( sizeof( int ) );
    int *one = test_malloc( sizeof( int ) );
    int *two = test_malloc( sizeof( int ) );

    *zero = 0;
    *one = 1;
    *two = 2;

    struct dblnode_t* node0 = dbllist_push( list, zero );
    struct dblnode_t* node1 = dbllist_push( list, one );
    struct dblnode_t* node2 = dbllist_push( list, two );

    assert_int_equal( dbllist_size( list ), 3 );
    assert_ptr_equal( node2->next, node1 );
    assert_ptr_equal( node1->prev, node2 );
    assert_ptr_equal( node1->next, node0 );
    assert_ptr_equal( node0->prev, node1 );

    assert_ptr_equal( one, dbllist_delete( list, one ) );
    assert_int_equal( dbllist_size( list ), 2 );

    assert_ptr_equal( node2->next, node0 );
    assert_ptr_equal( node0->prev, node2 );

    test_free( one );
    test_free( dbllist_pop( list ) );
    test_free( dbllist_pop( list ) );
}

static void list_clear(void **state) {
    dbllist_t* list = ( ( struct DblTest * ) *state )->list;
    int *zero = test_malloc( sizeof( int ) );
    int *one = test_malloc( sizeof( int ) );
    int *two = test_malloc( sizeof( int ) );

    *zero = 0;
    *one = 1;
    *two = 2;

    struct dblnode_t* node0 = dbllist_push( list, zero );
    struct dblnode_t* node1 = dbllist_push( list, one );
    struct dblnode_t* node2 = dbllist_push( list, two );

    dbllist_remove( list, NULL );

    assert_true( dbllist_is_empty( list ) );
    assert_null( dbllist_head( list ) );
    assert_null( dbllist_tail( list ) );

    test_free( zero );
    test_free( one );
    test_free( two );
}

static void list_clear_nodes(void **state) {
    dbllist_t* list = ( ( struct DblTest * ) *state )->list;
    int *zero = test_malloc( sizeof( int ) );
    int *one = test_malloc( sizeof( int ) );
    int *two = test_malloc( sizeof( int ) );

    *zero = 0;
    *one = 1;
    *two = 2;

    struct dblnode_t* node0 = dbllist_push( list, zero );
    struct dblnode_t* node1 = dbllist_push( list, one );
    struct dblnode_t* node2 = dbllist_push( list, two );

    dbllist_remove( list, release_node );

    assert_true( dbllist_is_empty( list ) );
    assert_null( dbllist_head( list ) );
    assert_null( dbllist_tail( list ) );

    test_free( zero );
    test_free( one );
    test_free( two );
}

static void clr(void **state) {
}

int dbll_test() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown( empty_list, dbll_setup, dbll_teardown ),
        cmocka_unit_test_setup_teardown( push_to_empty, dbll_setup, dbll_teardown ),
        cmocka_unit_test_setup_teardown( push_to_non_empty, dbll_setup, dbll_teardown ),
        cmocka_unit_test_setup_teardown( push_end_to_empty, dbll_setup, dbll_teardown ),
        cmocka_unit_test_setup_teardown( push_end_to_non_empty, dbll_setup, dbll_teardown ),
        cmocka_unit_test_setup_teardown( pop_to_empty, dbll_setup, dbll_teardown ),
        cmocka_unit_test_setup_teardown( pop_to_non_empty, dbll_setup, dbll_teardown ),
        cmocka_unit_test_setup_teardown( remove_from_empty, dbll_setup, dbll_teardown ),
        cmocka_unit_test_setup_teardown( remove_the_first, dbll_setup, dbll_teardown ),
        cmocka_unit_test_setup_teardown( remove_the_last, dbll_setup, dbll_teardown ),
        cmocka_unit_test_setup_teardown( remove_the_middle, dbll_setup, dbll_teardown ),
        cmocka_unit_test_setup_teardown( list_clear, dbll_setup, dbll_teardown ),
        cmocka_unit_test_setup_teardown( list_clear_nodes, dbll_setup, dbll_teardown ),
        cmocka_unit_test_setup_teardown( append_to_two_empty_lists, dbll_setup, dbll_teardown ),
        cmocka_unit_test_setup_teardown( append_to_one_empty_list, dbll_setup, dbll_teardown ),
        cmocka_unit_test_setup_teardown( append_to_nonempty_list, dbll_setup, dbll_teardown ),
        cmocka_unit_test_setup_teardown( join_two_empty_lists, dbll_setup, dbll_teardown ),
        cmocka_unit_test_setup_teardown( join_two_nonempty_lists, dbll_setup, dbll_teardown ),
    };

    return cmocka_run_group_tests( tests, NULL, NULL );
}
