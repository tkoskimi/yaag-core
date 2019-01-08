#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <cmocka.h>

#include "../../src/mem.h"
#include "../../src/data_structures/doublyLinkedList.h"

struct DblTest {
    struct DblLinkedList* list;
};

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

static void empty_list(void **state) {
    struct DblLinkedList* list = ( ( struct DblTest * ) *state )->list;
    assert_null( dbllist_head( list ) );
    assert_null( dbllist_tail( list ) );
    assert_true( dbllist_is_empty( list ) );
}

static void push_to_empty(void **state) {
    struct DblLinkedList* list = ( ( struct DblTest * ) *state )->list;
    int *value = test_malloc( sizeof( int ) );

    // Insert one value
    *value = 1;
    struct Node* node = dbllist_push( list, value );

    assert_false( dbllist_is_empty( list ) );
    assert_int_equal( dbllist_size( list ), 1 );
    assert_ptr_equal( dbllist_head( list ), dbllist_tail( list ) );
    assert_ptr_equal( dbllist_head( list ), node );
    assert_null( node->next );
    assert_null( node->prev );

    test_free( dbllist_pop( list ) );
}

static void push_to_non_empty(void **state) {
    struct DblLinkedList* list = ( ( struct DblTest * ) *state )->list;
    int *zero = test_malloc( sizeof( int ) );
    int *one = test_malloc( sizeof( int ) );

    *zero = 0;
    *one = 1;
    struct Node* node0 = dbllist_push( list, zero );
    struct Node* node1 = dbllist_push( list, one );

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

static void pop_to_empty(void **state) {
    struct DblLinkedList* list = ( ( struct DblTest * ) *state )->list;
    int *zero = test_malloc( sizeof( int ) );

    *zero = 0;

    struct Node* node0 = dbllist_push( list, zero );
    void *data = dbllist_pop( list );

    assert_true( dbllist_is_empty( list ) );
    assert_int_equal( dbllist_size( list ), 0 );
    assert_null( dbllist_head( list ) );
    assert_null( dbllist_tail( list ) );
    assert_ptr_equal( data, zero );

    test_free( data );
}

static void pop_to_non_empty(void **state) {
    struct DblLinkedList* list = ( ( struct DblTest * ) *state )->list;
    int *zero = test_malloc( sizeof( int ) );
    int *one = test_malloc( sizeof( int ) );
    int *two = test_malloc( sizeof( int ) );

    *zero = 0;
    *one = 1;
    *two = 2;

    struct Node* node0 = dbllist_push( list, zero );
    struct Node* node1 = dbllist_push( list, one );
    struct Node* node2 = dbllist_push( list, two );

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

static void multiple_pushes_pops(void **state) {
}

static void create_delete(void **state) {
}

static void clr(void **state) {
}

int dbll_test() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown( empty_list, dbll_setup, dbll_teardown ),
        cmocka_unit_test_setup_teardown( push_to_empty, dbll_setup, dbll_teardown ),
        cmocka_unit_test_setup_teardown( push_to_non_empty, dbll_setup, dbll_teardown ),
        cmocka_unit_test_setup_teardown( pop_to_empty, dbll_setup, dbll_teardown ),
        cmocka_unit_test_setup_teardown( pop_to_non_empty, dbll_setup, dbll_teardown )
    };

    return cmocka_run_group_tests( tests, NULL, NULL );
}
