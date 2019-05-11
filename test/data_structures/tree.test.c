#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmocka.h>

#include "../../src/data_structures/tree.h"
#include "../../src/data_structures/doublyLinkedList.h"
#include "../../src/defs.h"
#include "../../src/mem.h"

#ifdef UNIT_TESTING
extern void* _test_malloc(const size_t size, const char* file, const int line);
#define malloc(size) _test_malloc(size, __FILE__, __LINE__)
#endif
typedef struct {
    tree_t* tree;
} tree_test_t;

//  ****************************************
//  Fixtures
//  ****************************************

static int setup(void **state) {
    tree_test_t *test_struct = malloc( sizeof( tree_test_t ) );
    *state = test_struct;

    return 0;
}

static int teardown(void **state) {
    free( *state );

    return 0;
}

//  ****************************************
//  Misc functions
//  ****************************************

static void clr_mem_data( void *data ) {
    if ( data ) {
        mem_free( data );
    }
}

static tree_t* setup_empty_tree( tree_test_t* tt ) {
    return ( tt->tree = tree_new() );
}

static tree_t* setup_tree_with_root( tree_test_t* tt, tnode_t* root ) {
    tt->tree = tree_new();
    tt->tree->root = root;
    return tt->tree;
}

static tnode_t* new_tnode( char* name, int value ) {
    tnode_t* new_tnode = ( tnode_t* ) test_malloc( sizeof( tnode_t ) );

    int* data = ( int* ) test_malloc( sizeof( int ) );
    *data = value;

    new_tnode->name = name;
    new_tnode->data = data;
    new_tnode->parent = NULL;
    new_tnode->children = dbllist_new();

    return new_tnode;
}

static char* get_tnode_name( tnode_t* tnode ) {
    return tnode->name;
}

static int get_tnode_int_value( tnode_t* tnode ) {
    return *( (int *) tnode->data );
}

static void free_int( void *data ) {
    if ( data ) {
        mem_free( data );
    }
}

// *************
// tree_insert()
// *************

// UC: User inserts the root node
static void insert_root( void **state ) {
    tnode_t* new_tnode_0 = new_tnode( NULL, 0 );
    tree_t* empty_tree = setup_empty_tree( (tree_test_t*) *state );
    // ********** API Call **********
    tree_insert( empty_tree, empty_tree->root, new_tnode_0 );
    // ********** Verify **********
    assert_ptr_equal( new_tnode_0, empty_tree->root );
    assert_null( get_tnode_name( new_tnode_0 ) );
    assert_int_equal( 0, get_tnode_int_value( new_tnode_0 ) );
    // Clean-up.
    dbllist_free( new_tnode_0->children );
    test_free( new_tnode_0->data );
    test_free( new_tnode_0->name );
    test_free( new_tnode_0 );
    test_free( empty_tree );
}

// UC: User inserts a new child
static void insert_child( void **state ) {
    tnode_t* root = new_tnode( NULL, 0 );
    tnode_t* new_tnode_1 = new_tnode( NULL, 1 );
    tree_t* tree = setup_tree_with_root( (tree_test_t*) *state, root );
    // ********** API Call **********
    tree_insert( tree, tree->root, new_tnode_1 );
    // ********** Verify **********
    tnode_t* child = dbllist_head( tree->root->children )->data;
    assert_ptr_equal( new_tnode_1, child );
    assert_null( get_tnode_name( child ) );
    assert_int_equal( 1, get_tnode_int_value( child ) );
    // Clean-up.
    test_free( new_tnode_1->data );
    test_free( new_tnode_1->name );
    dbllist_remove( new_tnode_1->children, free_int );
    dbllist_free( new_tnode_1->children );
    test_free( root->data );
    test_free( root->name );
    dbllist_remove( root->children, free_int );
    dbllist_free( root->children );
    test_free( root );
    test_free( tree );
}

// UC: User inserts two children
static void insert_two_children( void **state ) {
    tnode_t* root = new_tnode( NULL, 0 );
    tnode_t* new_tnode_1 = new_tnode( NULL, 1 );
    tnode_t* new_tnode_2 = new_tnode( NULL, 2 );
    tree_t* tree = setup_tree_with_root( (tree_test_t*) *state, root );
    // ********** API Call **********
    tree_insert( tree, tree->root, new_tnode_1 );
    tree_insert( tree, tree->root, new_tnode_2 );
    // ********** Verify **********
    tnode_t* head = dbllist_head( tree->root->children )->data;
    assert_ptr_equal( new_tnode_1, head );
    assert_null( get_tnode_name( head ) );
    assert_int_equal( 1, get_tnode_int_value( head ) );
    tnode_t* tail = dbllist_tail( tree->root->children )->data;
    assert_ptr_equal( new_tnode_2, tail );
    assert_null( get_tnode_name( tail ) );
    assert_int_equal( 2, get_tnode_int_value( tail ) );
    // Clean-up.
    test_free( new_tnode_1->data );
    test_free( new_tnode_1->name );
    dbllist_remove( new_tnode_1->children, free_int );
    dbllist_free( new_tnode_1->children );
    test_free( new_tnode_2->data );
    test_free( new_tnode_2->name );
    dbllist_remove( new_tnode_2->children, free_int );
    dbllist_free( new_tnode_2->children );
    test_free( root->data );
    test_free( root->name );
    dbllist_remove( root->children, free_int );
    dbllist_free( root->children );
    test_free( root );
    test_free( tree );
}

// *************
// tree_remove()
// *************

// UC: User removes an empty tree
static void remove_empty_tree( void **state ) {
    tree_t* empty_tree = setup_empty_tree( (tree_test_t*) *state );
    // ********** API Call **********
    tree_remove( empty_tree, NULL, free_int );
    // ********** Verify **********
    assert_null( empty_tree->root );
    // Clean-up.
    test_free( empty_tree );
}

// UC: User removes a root only tree
static void remove_root_only_tree( void **state ) {
    tnode_t* root = new_tnode( NULL, 0 );
    tree_t* tree = setup_tree_with_root( (tree_test_t*) *state, root );
    // ********** API Call **********
    tree_remove( tree, root, free_int );
    // ********** Verify **********
    assert_null( tree->root );
    // Clean-up.
    test_free( tree );
}

// UC: User removes the only child from the node
static void remove_only_child( void **state ) {
    tnode_t* root = new_tnode( NULL, 0 );
    tnode_t* new_tnode_1 = new_tnode( NULL, 1 );
    tree_t* tree = setup_tree_with_root( (tree_test_t*) *state, root );
    tree_insert( tree, tree->root, new_tnode_1 );
    // ********** Verify the set-up **********
    assert_int_equal( 1, dbllist_size( root->children ) );
    assert_ptr_equal( new_tnode_1, dbllist_tail( root->children )->data );
    // ********** API Call **********
    tree_remove( tree, new_tnode_1, free_int );
    // ********** Verify **********
    assert_null( root->children );
    // Clean-up.
    test_free( root->data );
    test_free( root->name );
    test_free( root );
    test_free( tree );
}

// UC: User removes one node from the node that has multiple children
static void remove_one_of_two_nodes( void **state ) {
    tnode_t* root = new_tnode( NULL, 0 );
    tnode_t* new_tnode_1 = new_tnode( NULL, 1 );
    tnode_t* new_tnode_2 = new_tnode( NULL, 2 );
    tree_t* tree = setup_tree_with_root( (tree_test_t*) *state, root );
    tree_insert( tree, tree->root, new_tnode_1 );
    tree_insert( tree, tree->root, new_tnode_2 );
    // ********** Verify the set-up **********
    assert_int_equal( 2, dbllist_size( root->children ) );
    assert_ptr_equal( new_tnode_2, dbllist_tail( root->children )->data );
    // ********** API Call **********
    tree_remove( tree, new_tnode_2, free_int );
    // ********** Verify **********
    assert_int_equal( 1, dbllist_size( root->children ) );
    assert_ptr_equal( new_tnode_1, dbllist_head( root->children )->data );
    // Clean-up.
    test_free( new_tnode_1->data );
    test_free( new_tnode_1->name );
    dbllist_remove( new_tnode_1->children, free_int );
    dbllist_free( new_tnode_1->children );
    test_free( root->data );
    test_free( root->name );
    dbllist_remove( root->children, free_int );
    dbllist_free( root->children );
    test_free( root );
    test_free( tree );
}

// UC: User removes a subtree
static void remove_subtree( void **state ) {
    tnode_t* root = new_tnode( NULL, 0 );
    tnode_t* new_tnode_1 = new_tnode( NULL, 1 );
    tnode_t* new_tnode_2 = new_tnode( NULL, 2 );
    tnode_t* new_tnode_3 = new_tnode( NULL, 3 );
    tnode_t* new_tnode_4 = new_tnode( NULL, 4 );
    tree_t* tree = setup_tree_with_root( (tree_test_t*) *state, root );
    tree_insert( tree, tree->root, new_tnode_1 );
    tree_insert( tree, tree->root, new_tnode_2 );
    tree_insert( tree, new_tnode_2, new_tnode_3 );
    tree_insert( tree, new_tnode_2, new_tnode_4 );
    // ********** Verify the set-up **********
    assert_int_equal( 2, dbllist_size( root->children ) );
    assert_int_equal( 2, dbllist_size( new_tnode_2->children ) );
    assert_ptr_equal( new_tnode_3, dbllist_head( new_tnode_2->children )->data );
    assert_ptr_equal( new_tnode_4, dbllist_tail( new_tnode_2->children )->data );
    // ********** API Call **********
    tree_remove( tree, new_tnode_2, free_int );
    // ********** Verify **********
    assert_int_equal( 1, dbllist_size( root->children ) );
    assert_ptr_equal( new_tnode_1, dbllist_head( root->children )->data );
    // Clean-up.
    test_free( new_tnode_1->data );
    test_free( new_tnode_1->name );
    dbllist_remove( new_tnode_1->children, free_int );
    dbllist_free( new_tnode_1->children );
    test_free( root->data );
    test_free( root->name );
    dbllist_remove( root->children, free_int );
    dbllist_free( root->children );
    test_free( root );
    test_free( tree );
}

// UC: User removes a whole tree that has three levels
static void remove_tree( void **state ) {
    tnode_t* root = new_tnode( NULL, 0 );
    tnode_t* new_tnode_1 = new_tnode( NULL, 1 );
    tnode_t* new_tnode_2 = new_tnode( NULL, 2 );
    tnode_t* new_tnode_3 = new_tnode( NULL, 3 );
    tnode_t* new_tnode_4 = new_tnode( NULL, 4 );
    tree_t* tree = setup_tree_with_root( (tree_test_t*) *state, root );
    tree_insert( tree, tree->root, new_tnode_1 );
    tree_insert( tree, tree->root, new_tnode_2 );
    tree_insert( tree, new_tnode_2, new_tnode_3 );
    tree_insert( tree, new_tnode_2, new_tnode_4 );
    // ********** API Call **********
    tree_remove( tree, root, free_int );
    // ********** Verify **********
    assert_null( tree->root );
    // Clean-up.
    test_free( tree );
}

int tree_test(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown( insert_root, setup, teardown ),
        cmocka_unit_test_setup_teardown( insert_child, setup, teardown ),
        cmocka_unit_test_setup_teardown( insert_two_children, setup, teardown ),
        cmocka_unit_test_setup_teardown( remove_empty_tree, setup, teardown ),
        cmocka_unit_test_setup_teardown( remove_root_only_tree, setup, teardown ),
        cmocka_unit_test_setup_teardown( remove_only_child, setup, teardown ),
        cmocka_unit_test_setup_teardown( remove_one_of_two_nodes, setup, teardown ),
        cmocka_unit_test_setup_teardown( remove_subtree, setup, teardown ),
        cmocka_unit_test_setup_teardown( remove_tree, setup, teardown ),
    };

    return cmocka_run_group_tests( tests, NULL, NULL );
}

