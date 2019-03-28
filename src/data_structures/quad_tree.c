// Quad tree
// 
// [Implementation details]
//
// (c) Tuomas Koskimies, 2019

#include <assert.h>
#include <stdlib.h>

#include "../defs.h"
#include "../mem.h"
#include "./quad_tree.h"

struct QTree {
    struct QNode *root;
    int x;
    int y;
    int w;
    int h;
    int d;
};

struct QTree* quad_new() {
    struct QTree *quad = (struct QTree *) mem_malloc( sizeof( struct QTree ) );
    quad->root = NULL;
    quad->x = ORIGO_X;
    quad->y = ORIGO_Y;
    quad->w = REGION_WIDTH;
    quad->h = REGION_HEIGHT;
    quad->d = MIN_SIZE;
    return quad;
}

void quad_free( struct QTree* tree ) {
    assert(tree->root == NULL);
    mem_free(tree);
}

/**
 * Four disjoint subquadrants.
 *
 * ->> x
 * +----------+----------+ 
 * |          |          |
 * |          |          |
 * |          | +------+ |
 * |          | |      |<+--- A box
 * +----------+-+======+-+
 * |          |          |
 * |          |          |
 * |          |          |
 * |          |          |
 * +----------+----------+
 *
 * Q1: x0 <= x < x1, y0 <= y < y1
 * Q2: x1 <= x < x2, y0 <= y < y1
 * Q3: x0 <= x < x1, y1 <= y < y2
 * Q2: x1 <= x < x2, y1 <= y < y2
 *
 * Question: Which subquadrant containts the box? 
 */
struct QNode* quad_insert_rcrsv( struct QTree* tree, void* new_data, int x0, int y0, int x1, int y1 ) {
    if ( tree->root == NULL ) {
        tree->root = new_qnode();
    }
    return _quad_insert_rcrsv( tree->root, new_data, x0, y0, x1, y1, tree->x, tree->y, tree->w, tree->h, tree->d );
}

struct QNode* _quad_insert_rcrsv( struct QNode* node, void* new_data, int x0, int y0, int x1, int y1, int tl_x, int tl_y, int br_x, int br_y, int d ) {
    assert( node && QUAD_TREE_NULL );
    assert( ( x0 < x1 ) && ( y0 < y1 ) && QUAD_BOX_POS_ORIENTATION );
    assert( ( x1 - x0 <= br_x - tl_x ) && ( y1 - y0 <= br_y - tl_y ) && QUAD_TOO_BIG );

    // The middle point
    int mp_x = tl_x + ( br_x - tl_x )/2; // Ex. 10 + (13 - 10)/2 = 11
    int mp_y = tl_y + ( br_y - tl_y )/2;
    // qn indicates the quadrant of pn
    int q0 = _get_quad( mp_x, mp_y, x0, y0 );
    int q1 = _get_quad( mp_x, mp_y, x1, y1 );

    // If the quadrants of p0 and p1 are different, or if the size of the
    // quadrant is less or equal than the minimum, then the data is stored
    // in this node.
    if ( ( q0 != q1 ) || ( mp_x - tl_x <= d ) || ( mp_y - tl_y <= d ) ) {
        if ( node->boxes ) {
            // Todo: Change the type of the boxe
            dbllist_push( node->boxes[0], new_data );
        }
    } else {
        if ( q0 == 0 ) {
            if ( node->tl == NULL ) {
                node->tl = new_qnode();
            }
            _quad_insert_rcrsv( node->tl, new_data, x0, y0, x1, y1, tl_x, tl_y, mp_x, mp_y, d );
        } else if ( q0 == 1 ) {
            if ( node->tr == NULL ) {
                node->tr = new_qnode();
            }
            _quad_insert_rcrsv( node->tr, new_data, x0, y0, x1, y1, mp_x + 1, tl_y, br_x, mp_y, d);
        } else if ( q0 == 2 ) {
            if ( node->bl == NULL ) {
                node->bl = new_qnode();
            }
            _quad_insert_rcrsv( node->bl, new_data, x0, y0, x1, y1, tl_x, mp_y + 1, mp_x, br_y, d );
        } else {
            if ( node->br == NULL ) {
                node->br = new_qnode();
            }
            _quad_insert_rcrsv( node->br, new_data, x0, y0, x1, y1, mp_x + 1, mp_y + 1, br_x, br_y, d);
        }
    }
}

struct QNode* new_qnode() {
    struct QNode* node = ( struct QNode* ) mem_malloc( sizeof( struct QNode ) );
    //node->boxes = dbllist_new();
    node->tl = NULL;
    node->tr = NULL;
    node->bl = NULL;
    node->bl = NULL;
    return node;
}

int _get_quad( int mp_x, int mp_y, int x, int y ) {
    int q = 0;
    if ( x > mp_x ) q += 1;
    if ( y > mp_y ) q += 2;
    return q;
}

void quad_del_subtree() {
}

void* quad_del( struct QTree *tree, void* data ) {
}
