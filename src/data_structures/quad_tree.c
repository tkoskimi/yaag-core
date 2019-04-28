// Quad tree
// 
// [Implementation details]
//
// (c) Tuomas Koskimies, 2019

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../defs.h"
#include "../mem.h"
#include "./quad_tree.h"
#include "./tree.h"

// Note. The names must not be longer than LEVEL_NAME_MAX_LENGTH (see tree.h).
char* QUAD_PATHS[] = { "00.", "01.", "10.", "11." };

inline unsigned int _bit_mask_011(unsigned int n) {
    return (n >= sizeof(unsigned) * CHAR_BIT) ? (unsigned) -1 : (1U << n) - 1;
}

inline unsigned int _bit_mask_010(unsigned int m, unsigned int n) {
    return ( m == n ) ? 0 : (((unsigned) -1 >> (32 - (n))) & ~((1U << (m)) - 1));
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
    assert ( dim_in_bits <= COORDINATE_SIZE_IN_BITS / 2 );
    assert ( depth_of_qtree <= TREE_MAX_DEPTH );

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

    // If the rectangle is outside of the region, return -1.
    if ( x0 < q->x0 || y0 < q->y0 ) {
        return COORDINATE_OUSIDE;
    }
    if ( x0 > q->x1 || y0 > q->y1 ) {
        return COORDINATE_OUSIDE;
    }

    int dim = q->dim;
    int depth = q->depth;
    int x0s = ( x0 - q->x0 ) >> ( dim - depth );
    int y0s = ( y0 - q->y0 ) >> ( dim - depth );
    // Here the shifted coordinate (x0s and y0s) has a following form:
    //
    // MSB                                         1:st bit
    //                          <------ dim_in_bits ------>
    // +-----+-----+--...-+-----+-----+-------+--...+-----+
    // |  0  |  0  |  ... |  0  |x0s.n|x0s.n-1|  ...+x0s.1|
    // +-----+-----+--...-+-----+-----+-------+--...+-----+
    //
    // The last bits, 1 - n, define the modulo 2^( dim_in_bits - depth ), the
    // bits 2 - n modulo 2^( dim_in_bits - depth + 1 ) and so on. These modulos
    // build up a hierarchy of the quadrants in one dimension; In two
    // dimension cases, we need both coordinates, x and y.
    //
    // Two bits, e.g., x0s.n and y0s.n define the ( depth_of_qtree - n + 1 ):th
    // level of quadrants. Hence, we pack the index in the following way:
    //
    // MSB                                                             1:st bit
    //    +-----+--...+-----+---------+---------+--...+-----+-----+-----+-----+
    //    |  0  |--...|  0  |x0s.depth|y0s.depth|  ...|x0s.2|y0s.2|x0s.1|y0s.1|
    //    +-----+--...+-----+---------+---------+--...+-----+-----+-----+-----+
    //
    // For example, if x0s == 0x2 and y0s == 0x3, the index will be b1101 == 0xd.
    for ( int i = 0; i < depth; i++ ) {
        index += ( ( x0s & ( 1 << i ) ) << i + 1 ) + ( ( y0s & ( 1 << i ) ) << i );
    }

    return index;
}

char* quad_node_path( QStruct* q, int index_tl, int index_br ) {
    if ( index_tl == COORDINATE_OUSIDE || index_br == COORDINATE_OUSIDE ) {
        return NO_QUAD;
    }
    // Reserve space for the name. The path has three letters, e.g., "01."
    // except the last one that ends to the null character, '\0'.
    char* name = (char *) mem_malloc( 3 * DEPTH_OF_QTREE * sizeof( char ) );
    // Initialize the name for the strcat().
    name[0] = '\0';
    // Loop the indexes through until we find a different quadrant or the end.
    // There are multiple variables in the loop:
    //
    // 1) i in ( 2, 2 * depth) is the first bit of the index of the quadrant
    // 2) j in ( 0, 2 * depth - 2) is the first bit not included in the index
    // 3) k in ( 0, 3 * depth - 1) is the index of the last char of the path
    int k = -1;
    for ( int i = 2 * q->depth, j = i - 2; i > 0; i -= 2, j -= 2, k += 3 ) {
        int mask = _bit_mask_010( j, i );
        int mask_tl = index_tl & mask;
        int mask_br = index_br & mask;
        if ( mask_tl != mask_br ) {
            break;
        } 
        strcat( name, QUAD_PATHS[ mask_tl >> j ] );
    }
    if ( k < 0 ) {
        k = 0;
    }
    name[ k ] = '\0';
    return name;
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
 */
