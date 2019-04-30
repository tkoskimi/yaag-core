#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <cmocka.h>

#include "../../src/mem.h"
#include "../../src/data_structures/quad_tree.h"

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

typedef struct {
    QStruct* q;
} QuadTest;


//
//   Test Fixtures
//

static int quad_setup(void **state) {
    QuadTest *test_struct = test_malloc( sizeof( QuadTest ) );
    test_struct->q = quad_new();
    *state = test_struct;

    return 0;
}

static int quad_setup_tree_root(void **state) {
    QuadTest *test_struct = test_malloc( sizeof( QuadTest ) );
    test_struct->q = quad_new();
    *state = test_struct;

    TNode *root = (TNode *) mem_malloc( sizeof( TNode ) );
    test_struct->q->qtree->root = root;
    test_struct->q->qtree->root->name = NULL;
    test_struct->q->qtree->root->data = NULL;
    test_struct->q->qtree->root->children = dbllist_new();

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
    assert_int_equal( 0x3, _bit_mask_010(0,2) );
    assert_int_equal( 0x1c, _bit_mask_010(2,5) );
    assert_int_equal( 0x00f0, _bit_mask_010(4,8) );
}

static void test_index(void **state) {
    QStruct* q = ( ( QuadTest * ) *state )->q;

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
    assert_int_equal( 0x0, quad_point_index( q, 0x10, 0x10 ) );
    assert_int_equal( 0xc, quad_point_index( q, 0x20, 0x20 ) );
    assert_int_equal( -1, quad_point_index( q, 0x30, 0x20 ) );
    
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

static void test_limits_custom_q(void **state) {
    // Max
    QStruct* q = quad_new_and_init( 0x0, 0x0, 16, 8);

    int x0;
    int y0;

    x0 = ( 0x87 << ( q->dim - q->depth ) );
    y0 = ( 0x74 << ( q->dim - q->depth ) );
    assert_int_equal( 0x953a, quad_point_index(q, x0, y0));

    char* name = NULL;
    assert_string_equal( "10.01.01.01.00.11.10.10", name = quad_node_path( q, 0x953a, 0x953a ) );
    mem_free( name );

    // Min
    q = quad_new_and_init( 0x0, 0x0, 1, 1);

    x0 = ( 0x0 << ( q->dim - q->depth ) );
    y0 = ( 0x1 << ( q->dim - q->depth ) );
    assert_int_equal( 0x1, quad_point_index(q, x0, y0));

    x0 = ( 0x1 << ( q->dim - q->depth ) );
    y0 = ( 0x1 << ( q->dim - q->depth ) );
    assert_int_equal( 0x3, quad_point_index(q, x0, y0));

    assert_string_equal( "01", name = quad_node_path( q, 0x1, 0x1 ) );
    assert_string_equal( "11", name = quad_node_path( q, 0x3, 0x3 ) );
    mem_free( name );
}

static void test_node_path(void **state) {
    QStruct* q = ( ( QuadTest * ) *state )->q;

    assert_null( quad_node_path(q, -1, 0));
    assert_null( quad_node_path(q, 0, -1));
    assert_null( quad_node_path(q, -1, -1));

    char* name = NULL;
    assert_string_equal( "00", name = quad_node_path( q, 0x8, 0x4 ) );
    mem_free( name );

    assert_string_equal( "00", name = quad_node_path( q, 0x4, 0x8 ) );
    mem_free( name );

    assert_string_equal( "00.00", name = quad_node_path( q, 0x2, 0x1 ) );
    mem_free( name );

    assert_string_equal( "00.00", name = quad_node_path( q, 0x1, 0x2 ) );
    mem_free( name );

    assert_string_equal( "00.00.00", name = quad_node_path( q, 0x0, 0x0 ) );
    mem_free( name );

    assert_string_equal( "00.00.11", name = quad_node_path( q, 0x3, 0x3 ) );
    mem_free( name );

    assert_string_equal( "10.01.00", name = quad_node_path( q, 0x24, 0x24 ) );
    mem_free( name );
}

static void test_node_path_in_custom_q(void **state) {
    QStruct* q = quad_new_and_init( 0x10, 0x10, 6, 4);

    char* name = NULL;
    assert_string_equal( "00.01.10.11", name = quad_node_path( q, 0x1b, 0x1b ) );
    mem_free( name );
}

// The callback function
//
// @param err The error value
// @param data The data of the tree node
static void _clr_item(void *data) {
    if ( data ) {
        mem_free( data );
    }
}

static void _clr_go(void *data) {
    if ( data ) {
        test_free( data );
    }
}

static void _clr_gos(void *data) {
    if ( data ) {
        DblLinkedList* l = ( DblLinkedList * ) data;
        dbllist_remove( l, _clr_go );
    }
}

void _insert( int err, void* old_data, void* new_data ) {
    if ( err == SUCCESS ) {
        return;
    } else if ( err == WARNING_VALUE_REPLACEMENT ) {
        dbllist_append( (DblLinkedList *) old_data, (DblLinkedList *) new_data );
        // There is just one game object in this list. Pop it and release the list.
        dbllist_remove( (DblLinkedList *) new_data, NULL );
        dbllist_free( (DblLinkedList *) new_data );
    }
}

static void insert_node_into_empty_qtree(void **state) {
    QStruct* q = ( ( QuadTest * ) *state )->q;

    TNode *root = (TNode *) mem_malloc( sizeof( TNode ) );
    q->qtree->root = root;
    q->qtree->root->name = NULL;
    q->qtree->root->data = NULL;
    q->qtree->root->children = dbllist_new();

    GameObject *go = (GameObject *) test_malloc( sizeof( GameObject ) );
    go->x = 0;
    go->y = 0;
    go->w = 1;
    go->h = 1;

    DblLinkedList* gos = dbllist_new();
    dbllist_push( gos, go );
    quad_insert( q, go->x, go->y, go->x + go->w, go->y + go->h, gos, _insert );

    // Find the inserted node.
    DblLinkedList* children = NULL;
    TNode* tnode = tree_find( q->qtree, "00.00.00", &children);

    // We get the information from the return value of tree_find.
    assert_string_equal( "00", tnode->name );
    assert_ptr_equal( gos, tnode->data );
    assert_ptr_equal( go, ((DblLinkedList *)tnode->data)->head->data);

    tree_remove( q->qtree, NULL, _clr_gos );

    // ToDo: tree_remove(.., NULL, ..) must remove everything
    mem_free( q->qtree->root );
    q->qtree->root = NULL;
}

// A use case: User adds one object into the empty qtree
//
// This test demonstrates also how the user can add arbitrary objects into the
// tree. In this case, we use integers.
static void insert_empty_qtree( void ** state ) {
    QStruct* q = ( ( QuadTest * ) *state )->q;

    // This should be put in a fixture.
}

static void insert_three_nodes_qtree(void **state) {
    QStruct* q = ( ( QuadTest * ) *state )->q;

    TNode *root = (TNode *) mem_malloc( sizeof( TNode ) );
    q->qtree->root = root;
    q->qtree->root->name = NULL;
    q->qtree->root->data = NULL;
    q->qtree->root->children = dbllist_new();

    GameObject *go1 = (GameObject *) test_malloc( sizeof( GameObject ) );
    go1->x = 0;
    go1->y = 0;
    go1->w = 1;
    go1->h = 1;

    GameObject *go2 = (GameObject *) test_malloc( sizeof( GameObject ) );
    go2->x = 0x80;
    go2->y = 0;
    go2->w = 1;
    go2->h = 1;

    GameObject *go3 = (GameObject *) test_malloc( sizeof( GameObject ) );
    go3->x = 0;
    go3->y = 0;
    go3->w = 1;
    go3->h = 1;

    DblLinkedList* gos = NULL;

    gos = dbllist_new();
    dbllist_push( gos, go1 );
    quad_insert( q, go1->x, go1->y, go1->x + go1->w, go1->y + go1->h, gos, _insert );

    gos = dbllist_new();
    dbllist_push( gos, go2 );
    quad_insert( q, go2->x, go2->y, go2->x + go2->w, go2->y + go2->h, gos, _insert );

    gos = dbllist_new();
    dbllist_push( gos, go3 );
    quad_insert( q, go3->x, go3->y, go3->x + go3->w, go3->y + go3->h, gos, _insert );

    DblLinkedList* children = NULL;
    TNode* tnode = tree_find( q->qtree, "00.00.00", &children);

    assert_ptr_equal( go1, ((DblLinkedList *)tnode->data)->head->data);
    assert_ptr_equal( go3, ((DblLinkedList *)tnode->data)->tail->data);

    tnode = tree_find( q->qtree, "00.00.10", &children);

    assert_string_equal( "10", tnode->name );
    assert_ptr_equal( go2, ((DblLinkedList *)tnode->data)->head->data);

    tree_remove( q->qtree, NULL, _clr_gos );
    mem_free( q->qtree->root );
    q->qtree->root = NULL;
}

int quad_test() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown( bit_masks, quad_setup, quad_teardown ),
        cmocka_unit_test_setup_teardown( test_index, quad_setup, quad_teardown ),
        cmocka_unit_test_setup_teardown( test_index_in_custom_q, quad_setup, quad_teardown ),
        cmocka_unit_test_setup_teardown( test_limits_custom_q, quad_setup, quad_teardown ),
        cmocka_unit_test_setup_teardown( test_node_path, quad_setup, quad_teardown ),
        cmocka_unit_test_setup_teardown( test_node_path_in_custom_q, quad_setup, quad_teardown ),
        cmocka_unit_test_setup_teardown( insert_node_into_empty_qtree, quad_setup, quad_teardown ),
        cmocka_unit_test_setup_teardown( insert_three_nodes_qtree, quad_setup, quad_teardown ),
    };

    return cmocka_run_group_tests( tests, NULL, NULL );
}
