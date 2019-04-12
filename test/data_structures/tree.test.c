#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmocka.h>

#include "../../src/data_structures/tree.h"
#include "../../src/data_structures/doublyLinkedList.h"
#include "../../src/defs.h"
#include "../../src/mem.h"

typedef struct {
    TTree* tree;
} TreeTest;

static TNode *get_tnode_from_heads( TNode *root, int depth ) {
    TNode *node = root;
    for( int i = 0; i < depth; i++ ) {
        node = (TNode *) ( (Node *) dbllist_head( node->children ) )->data;
    }
    return node;
}

static TNode *get_tnode_from_tails( TNode *root, int depth ) {
    TNode *node = root;
    for( int i = 0; i < depth; i++ ) {
        node = (TNode *) ( (Node *) dbllist_tail( node->children ) )->data;
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

    dbllist_remove( tree->root->children, NULL );
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

    dbllist_remove( get_tnode_from_tails( root, 2 )->children, NULL );
    mem_free( get_tnode_from_tails( root, 2 )->name );
    dbllist_remove( get_tnode_from_tails( root, 1 )->children, NULL );
    mem_free( get_tnode_from_tails( root, 1 )->name );
    dbllist_remove( tree->root->children, NULL );
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

    dbllist_remove( get_tnode_from_tails( root, 2 )->children, NULL );
    mem_free( get_tnode_from_tails( root, 2 )->name );
    dbllist_remove( get_tnode_from_heads( root, 2 )->children, NULL );
    mem_free( get_tnode_from_heads( root, 2 )->name );
    dbllist_remove( get_tnode_from_tails( root, 1 )->children, NULL );
    mem_free( get_tnode_from_tails( root, 1 )->name );
    dbllist_remove( tree->root->children, NULL );
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

static void find_a_node_from_2nd_level(void **state) {
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

    ret_value = tree_insert( tree, path, zero, 1, 0 );
    assert_null( ret_value );
    assert_string_equal( "a", get_tnode_from_tails( root, 1 )->name );
    assert_string_equal( "b", get_tnode_from_tails( root, 2 )->name );
    assert_ptr_equal( zero, get_tnode_from_tails( root, 2 )->data );

    TNode *tree_node = NULL;
    DblLinkedList *list = NULL;

    // The root.
    tree_node = tree_find( tree, NULL, &list );
    assert_null( tree_node->name );
    assert_null( tree_node->data );
    assert_int_equal( 1, dbllist_size( tree_node->children ) );
    assert_ptr_equal( list, tree_node->children );

    // The 1st level.
    list = NULL;
    tree_node = tree_find( tree, "a", &list );

    assert_int_equal( 1, dbllist_size( list ) );
    assert_string_equal( "a", tree_node->name );
    assert_null( tree_node->data );
    assert_int_equal( 1, dbllist_size( tree_node->children ) );
    assert_ptr_equal( list, root->children );

    // The 2nd level.
    list = NULL;
    tree_node = tree_find( tree, "a.b", &list );
    assert_string_equal( "b", tree_node->name );
    assert_ptr_equal( zero, tree_node->data );
    assert_int_equal( 0, dbllist_size( tree_node->children ) );
    assert_ptr_equal( list, get_tnode_from_tails( root, 1 )->children );

    // Not found in the 1st level.
    list = NULL;
    tree_node = tree_find( tree, "c", &list );
    assert_null( tree_node );
    assert_null( list );

    // Not found in the 1st level.
    list = NULL;
    tree_node = tree_find( tree, "a.c", &list );
    assert_null( tree_node );
    assert_null( list );

    dbllist_remove( get_tnode_from_tails( root, 2 )->children, NULL );
    mem_free( get_tnode_from_tails( root, 2 )->name );
    dbllist_remove( get_tnode_from_tails( root, 1 )->children, NULL );
    mem_free( get_tnode_from_tails( root, 1 )->name );
    dbllist_remove( tree->root->children, NULL );
    test_free( zero );
    test_free( root );
    tree->root = NULL;
    return;
}

static void clr_data(void *data) {
    if (data) {
        test_free( data );
    }
}

static void remove_tree(void **state) {
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

    // Parents flag is on
    ret_value = tree_insert( tree, path, zero, 1, 0 );
    TNode* b = get_tnode_from_tails( root, 2 );
    TNode* a = get_tnode_from_tails( root, 1 );

#ifdef LOGGING
    printf("Original addresses\n");
    printf("%s: %lx\n", "zero", (long unsigned int) zero);

    printf("%s: %lx\n", "root", (long unsigned int) root);
    printf("%s: %lx\n", "root's children", (long unsigned int) root->children);
    printf("%s: %lx\n", "a", (long unsigned int) a);
    printf("%s: %lx\n", "a's name", (long unsigned int) a->name);
    printf("%s: %lx\n", "a's children", (long unsigned int) a->children);
    printf("%s: %lx\n", "b", (long unsigned int) b);
    printf("%s: %lx\n", "b's name", (long unsigned int) b->name);
    printf("%s: %lx\n", "b's children", (long unsigned int) b->children);
#endif

    assert_null( ret_value );

    tree_remove( tree, NULL, clr_data );

    test_free( root );
    tree->root = NULL;

    return;
}

static void remove_subtree(void **state) {
    TTree* tree = ( ( TreeTest * ) *state )->tree;

    TNode *root = (TNode *) test_malloc( sizeof( TNode ) );
    tree->root = root;
    tree->root->name = NULL;
    tree->root->data = NULL;
    tree->root->children = dbllist_new();

    char *path = NULL;

    void* ret_value = NULL;
    path = "a.b";
    int *zero = test_malloc( sizeof( int ) );
    *zero = 0;
    ret_value = tree_insert( tree, path, zero, 1, 0 );

    assert_null( ret_value );

    path = "a.c";
    int *one = test_malloc( sizeof( int ) );
    *one = 0;
    ret_value = tree_insert( tree, path, one, 1, 0 );

    assert_null( ret_value );

    TNode* c = get_tnode_from_heads( root, 2 );
    TNode* b = get_tnode_from_tails( root, 2 );
    TNode* a = get_tnode_from_tails( root, 1 );

#ifdef LOGGING
    printf("Original addresses\n");
    printf("%s: %lx\n", "zero", (long unsigned int) zero);

    printf("%s: %lx\n", "root", (long unsigned int) root);
    printf("%s: %lx\n", "root's children", (long unsigned int) root->children);
    printf("%s: %lx\n", "a", (long unsigned int) a);
    printf("%s: %lx\n", "a's name", (long unsigned int) a->name);
    printf("%s: %lx\n", "a's children", (long unsigned int) a->children);
    printf("%s: %lx\n", "b", (long unsigned int) b);
    printf("%s: %lx\n", "b's name", (long unsigned int) b->name);
    printf("%s: %lx\n", "b's children", (long unsigned int) b->children);
    printf("%s: %lx\n", "c", (long unsigned int) c);
    printf("%s: %lx\n", "c's name", (long unsigned int) c->name);
    printf("%s: %lx\n", "c's children", (long unsigned int) c->children);
#endif

    tree_remove( tree, "a", clr_data );

    assert_int_equal( 0, dbllist_size( root->children ) );

    dbllist_remove( tree->root->children, NULL );
    test_free( root );
    tree->root = NULL;

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
        cmocka_unit_test_setup_teardown( find_a_node_from_2nd_level, tree_setup, tree_teardown ),
        cmocka_unit_test_setup_teardown( remove_tree, tree_setup, tree_teardown ),
        cmocka_unit_test_setup_teardown( remove_subtree, tree_setup, tree_teardown ),
    };

    return cmocka_run_group_tests( tests, NULL, NULL );
}

