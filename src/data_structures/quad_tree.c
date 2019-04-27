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

inline unsigned int _bit_mask_011(unsigned int x) {
    return (x >= sizeof(unsigned) * CHAR_BIT) ? (unsigned) -1 : (1U << x) - 1;
}

inline unsigned int _bit_mask_010(unsigned int x, unsigned int y) {
    return ( x == y ) ? 0 : (((unsigned) -1 >> (32 - (y))) & ~((1U << (x)) - 1));
} 

QStruct* quad_new() {
    QStruct *q = (QStruct *) mem_malloc( sizeof( QStruct ) );
    q->qtree = tree_new();
    q->x0 = 0;
    q->y0 = 0;
    q->x1 = ( 1 << REGION_DIM_IN_BITS ) - 1;
    q->y1 = ( 1 << REGION_DIM_IN_BITS ) - 1;
    q->dim = REGION_DIM_IN_BITS;
    q->depth = DEPTH_OF_QTREE;
    return q;
}

QStruct* quad_new_and_init(
        unsigned int x0,
        unsigned int y0,
        unsigned int dim_in_bits,
        unsigned int depth_of_qtree ) {
    QStruct *q = (QStruct *) mem_malloc( sizeof( QStruct ) );
    q->qtree = tree_new();
    q->x0 = x0;
    q->y0 = y0;
    q->x1 = ( 1 << dim_in_bits ) + x0 - 1;
    q->y1 = ( 1 << dim_in_bits ) + y0 - 1;
    q->dim = dim_in_bits;
    q->depth = depth_of_qtree;
    return q;
}

void quad_free( QStruct *q ) {
}

int quad_point_index( QStruct* q, unsigned int x0, unsigned int y0 ) {
    int index = 0;

    // If the rectangle is outside of the region, return 0.
    if ( x0 < q->x0 || y0 < q->y0 ) {
        return 0;
    }
    if ( x0 > q->x1 || y0 > q->y1 ) {
        return 0;
    }

    int dim = q->dim;
    int depth = q->depth;
    int x0s = ( x0 - q->x0 ) >> ( dim - depth );
    int y0s = ( y0 - q->y0 ) >> ( dim - depth );
    // The index will be as follows:
    //
    // MSB                         LSB
    //    ...+-----+-----+-----+-----+
    //    ...|y0s.2|x0s.2|y0s.1|x0s.1|
    //    ...+-----+-----+-----+-----+
    //
    // For example, if x0s == 0x2 and y0s == 0x3, the index is b1011 == 0xb
    for ( int i = 0; i < depth; i++ ) {
        index += ( ( x0s & ( 1 << i ) ) << i + 1 ) + ( ( y0s & ( 1 << i ) ) << i );
    }

    return index;
}

/**
 * Four disjoint subquadrants.
 *
 * ->> x
 * +----------+----------+ 
 * |          |          |
 * |          |          |
 * |          | +------+ |
 * |          | |//////|<+--- a box
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
// QNode* quad_insert_rcrsv( QArray* tree, void* new_data, int x0, int y0, int x1, int y1 ) {
//     return _quad_insert_rcrsv( NULL, new_data, x0, y0, x1, y1, tree->x, tree->y, tree->w, tree->h, tree->d );
// }

// QNode* _quad_insert_rcrsv( QNode* node, void* new_data, int x0, int y0, int x1, int y1, int tl_x, int tl_y, int br_x, int br_y, int d ) {
//     assert( node && QUAD_TREE_NULL );
//     assert( ( x0 < x1 ) && ( y0 < y1 ) && QUAD_BOX_POS_ORIENTATION );
//     assert( ( x1 - x0 <= br_x - tl_x ) && ( y1 - y0 <= br_y - tl_y ) && QUAD_TOO_BIG );

//     // The middle point
//     int mp_x = tl_x + ( br_x - tl_x )/2; // Ex. 10 + (13 - 10)/2 = 11
//     int mp_y = tl_y + ( br_y - tl_y )/2;
//     // qn indicates the quadrant of pn
//     int q0 = _get_quad( mp_x, mp_y, x0, y0 );
//     int q1 = _get_quad( mp_x, mp_y, x1, y1 );

//     // If the quadrants of p0 and p1 are different, or if the size of the
//     // quadrant is less or equal than the minimum, then the data is stored
//     // in this node.
//     if ( ( q0 != q1 ) || ( mp_x - tl_x <= d ) || ( mp_y - tl_y <= d ) ) {
//         // if ( node->boxes ) {
//         //     // Todo: Change the type of the boxe
//         //     dbllist_push( node->boxes[0], new_data );
//         // }
//     } else {
//         if ( q0 == 0 ) {
//             if ( node->tl == NULL ) {
//                 node->tl = new_qnode();
//             }
//             _quad_insert_rcrsv( node->tl, new_data, x0, y0, x1, y1, tl_x, tl_y, mp_x, mp_y, d );
//         } else if ( q0 == 1 ) {
//             if ( node->tr == NULL ) {
//                 node->tr = new_qnode();
//             }
//             _quad_insert_rcrsv( node->tr, new_data, x0, y0, x1, y1, mp_x + 1, tl_y, br_x, mp_y, d);
//         } else if ( q0 == 2 ) {
//             if ( node->bl == NULL ) {
//                 node->bl = new_qnode();
//             }
//             _quad_insert_rcrsv( node->bl, new_data, x0, y0, x1, y1, tl_x, mp_y + 1, mp_x, br_y, d );
//         } else {
//             if ( node->br == NULL ) {
//                 node->br = new_qnode();
//             }
//             _quad_insert_rcrsv( node->br, new_data, x0, y0, x1, y1, mp_x + 1, mp_y + 1, br_x, br_y, d);
//         }
//     }
// }

// QNode* new_qnode() {
//     QNode* node = ( QNode* ) mem_malloc( sizeof( QNode ) );
//     //node->boxes = dbllist_new();
//     node->tl = NULL;
//     node->tr = NULL;
//     node->bl = NULL;
//     node->bl = NULL;
//     return node;
// }

// int _get_quad( int mp_x, int mp_y, int x, int y ) {
//     int q = 0;
//     if ( x > mp_x ) q += 1;
//     if ( y > mp_y ) q += 2;
//     return q;
// }

// void quad_del_subtree() {
// }

// void* quad_del( QArray *tree, void* data ) {
// }
