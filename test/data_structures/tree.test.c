#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmocka.h>

#include "../../src/data_structures/tree.h"
#include "../../src/data_structures/doublyLinkedList.h"
#include "../../src/mem.h"

typedef struct {
    TTree* tree;
} TreeTest;

static TNode *get_tnode_from_heads( TNode *root, int depth ) {
    TNode *node = root;
    for( int i = 0; i < depth; i++ ) {
        node = (TNode *) ( (struct Node *) dbllist_head( node->children ) )->data;
    }
    return node;
}

static TNode *get_tnode_from_tails( TNode *root, int depth ) {
    TNode *node = root;
    for( int i = 0; i < depth; i++ ) {
        node = (TNode *) ( (struct Node *) dbllist_tail( node->children ) )->data;
    }
    return node;
}

static int tree_setup(void **state) {
    TreeTest *test_struct = test_malloc( sizeof( TreeTest ) );
    test_struct->tree = tree_new();
    *state = test_struct;

    return 0;
}

static int tree_teardown(void **state) {
    tree_free( ( ( TreeTest * ) *state )->tree );
    test_free( *state );

    return 0;
}

static void parse_ok_path_1(void **state) {
    char *path = "a.b.c";
    char** strs = tree_split_path( path );
    assert_string_equal( "a", strs[0] );
    assert_string_equal( "b", strs[1] );
    assert_string_equal( "c", strs[2] );
    assert_null( strs[3] );
    return;
}

static void parse_ok_path_2(void **state) {
    char *path = "itisamax.b";
    char** strs = tree_split_path( path );
    assert_string_equal( "itisamax", strs[0] );
    assert_string_equal( "b", strs[1] );
    return;
}

static void parse_ok_path_3(void **state) {
    char *path = "a";
    char** strs = tree_split_path( path );
    assert_string_equal( "a", strs[0] );
    assert_null( strs[1] );
    return;
}

static void parse_nok_path_1(void **state) {
    char *path = "a.";
    char** strs = tree_split_path( path );
    assert_null( strs );
    return;
}

static void parse_nok_path_2(void **state) {
    char *path = ".a";
    char** strs = tree_split_path( path );
    assert_null( strs );
    return;
}

static void parse_nok_path_3(void **state) {
    char *path = "thisistoo.b";
    char** strs = tree_split_path( path );
    assert_null( strs );
    return;
}

static void parse_deep_tree(void **state) {
    char *path = "a.b.c.d.e.f.g.h";
    char** strs = tree_split_path( path );
    assert_string_equal( "a", strs[0] );
    assert_string_equal( "b", strs[1] );
    assert_string_equal( "c", strs[2] );
    assert_string_equal( "d", strs[3] );
    assert_string_equal( "e", strs[4] );
    assert_string_equal( "f", strs[5] );
    assert_string_equal( "g", strs[6] );
    assert_string_equal( "h", strs[7] );
    return;
}

static void parse_tree_too_deep(void **state) {
    char *path = "a.b.c.d.e.f.g.h.i";
    char** strs = tree_split_path( path );
    assert_null( strs );
    return;
}

static void insert_node_into_empty_tree(void **state) {
    TTree* tree = ( ( TreeTest * ) *state )->tree;

    TNode *root = (TNode *) test_malloc( sizeof( TNode ) );
    tree->root = root;
    tree->root->name = NULL;
    tree->root->data = NULL;
    tree->root->children = dbllist_new();

    void* ret_value = NULL;
    char *path = "a";
    int *zero = test_malloc( sizeof( int ) );

    // Set data
    *zero = 0;

    ret_value = tree_insert( tree, path, zero, 0, 0 );

    TNode* node = (TNode *) dbllist_tail( tree->root->children )->data;
    assert_int_equal( 1, dbllist_size( tree->root->children ) );
    assert_string_equal( "a", node->name );
    assert_ptr_equal( zero, node->data );
    assert_true( dbllist_is_empty( node->children ) );

    dbllist_clr( tree->root->children );
    test_free( zero );
    test_free( root );
    tree->root = NULL;
    return;
}

static void insert_node_into_nonempty_tree(void **state) {
    TTree* tree = ( ( TreeTest * ) *state )->tree;

    TNode *root = (TNode *) test_malloc( sizeof( TNode ) );
    tree->root = root;
    tree->root->name = NULL;
    tree->root->data = NULL;
    tree->root->children = dbllist_new();

    void* ret_value = NULL;
    char *path = "a.b";
    int *zero = test_malloc( sizeof( int ) );

    // Set data
    *zero = 0;

    // Parents flag is off

    ret_value = tree_insert( tree, path, zero, 0, 0 );

    assert_null( ret_value );
    assert_true( dbllist_is_empty( tree->root->children ) );

    // Parents flag is on

    ret_value = tree_insert( tree, path, zero, 1, 0 );
    assert_null( ret_value );
    assert_string_equal( "a", get_tnode_from_tails( root, 1 )->name );
    assert_string_equal( "b", get_tnode_from_tails( root, 2 )->name );
    assert_ptr_equal( zero, get_tnode_from_tails( root, 2 )->data );

    dbllist_clr( get_tnode_from_tails( root, 2 )->children );
    mem_free( get_tnode_from_tails( root, 2 )->name );
    dbllist_clr( get_tnode_from_tails( root, 1 )->children );
    mem_free( get_tnode_from_tails( root, 1 )->name );
    dbllist_clr( tree->root->children );
    test_free( zero );
    test_free( root );
    tree->root = NULL;
    return;
}

static void insert_two_nodes(void **state) {
    TTree* tree = ( ( TreeTest * ) *state )->tree;

    TNode *root = (TNode *) test_malloc( sizeof( TNode ) );
    tree->root = root;
    tree->root->name = NULL;
    tree->root->data = NULL;
    tree->root->children = dbllist_new();

    void* ret_value = NULL;
    char *path1 = "a.b";
    char *path2 = "a.c";
    int *zero = test_malloc( sizeof( int ) );
    int *one = test_malloc( sizeof( int ) );

    // Set data
    *zero = 0;
    *one = 0;

    // Parents flag is on

    ret_value = tree_insert( tree, path1, zero, 1, 0 );
    assert_null( ret_value );
    
    ret_value = tree_insert( tree, path2, one, 1, 0 );
    assert_null( ret_value );

    assert_string_equal( "a", get_tnode_from_tails( root, 1 )->name );
    assert_string_equal( "b", get_tnode_from_heads( root, 2 )->name );
    assert_string_equal( "c", get_tnode_from_tails( root, 2 )->name );
    assert_ptr_equal( zero, get_tnode_from_heads( root, 2 )->data );
    assert_ptr_equal( one, get_tnode_from_tails( root, 2 )->data );

    dbllist_clr( get_tnode_from_tails( root, 2 )->children );
    mem_free( get_tnode_from_tails( root, 2 )->name );
    dbllist_clr( get_tnode_from_heads( root, 2 )->children );
    mem_free( get_tnode_from_heads( root, 2 )->name );
    dbllist_clr( get_tnode_from_tails( root, 1 )->children );
    mem_free( get_tnode_from_tails( root, 1 )->name );
    dbllist_clr( tree->root->children );
    test_free( one );
    test_free( zero );
    test_free( root );
    tree->root = NULL;
    return;
}

static void insert_node_and_do_replacement(void **state) {
    char *path = "a.b.c.d.e.f.g.h.i";
    char** strs = tree_split_path( path );
    assert_null( strs );
    return;
}

static void clr(void **state) {
}

int tree_test(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown( parse_ok_path_1, tree_setup, tree_teardown ),
        cmocka_unit_test_setup_teardown( parse_ok_path_2, tree_setup, tree_teardown ),
        cmocka_unit_test_setup_teardown( parse_ok_path_3, tree_setup, tree_teardown ),
        cmocka_unit_test_setup_teardown( parse_nok_path_1, tree_setup, tree_teardown ),
        cmocka_unit_test_setup_teardown( parse_nok_path_2, tree_setup, tree_teardown ),
        cmocka_unit_test_setup_teardown( parse_nok_path_3, tree_setup, tree_teardown ),
        cmocka_unit_test_setup_teardown( parse_deep_tree, tree_setup, tree_teardown ),
        cmocka_unit_test_setup_teardown( parse_tree_too_deep, tree_setup, tree_teardown ),
        cmocka_unit_test_setup_teardown( insert_node_into_empty_tree, tree_setup, tree_teardown ),
        cmocka_unit_test_setup_teardown( insert_node_into_nonempty_tree, tree_setup, tree_teardown ),
        cmocka_unit_test_setup_teardown( insert_two_nodes, tree_setup, tree_teardown ),
    };

    return cmocka_run_group_tests( tests, NULL, NULL );
}
