// Quad tree
// 
// [Implementation details]
//
// (c) Tuomas Koskimies, 2019

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "../defs.h"
#include "../mem.h"
#include "./doublyLinkedList.h"
#include "./quad_tree.h"
#include "./tree.h"

#define _is_empty(x) ((x)->root == NULL)

// Note. The names must not be longer than LEVEL_NAME_MAX_LENGTH (see tree.h).
char* QTREE_PATHS[] = { "00.", "01.", "10.", "11." };

inline unsigned int _bit_mask_011(unsigned int n) {
    return (n >= sizeof(unsigned) * CHAR_BIT) ? (unsigned) -1 : (1U << n) - 1;
}

inline unsigned int _bit_mask_010(unsigned int m, unsigned int n) {
    return ( m == n ) ? 0 : (((unsigned) -1 >> (32 - (n))) & ~((1U << (m)) - 1));
} 

static dbllist_t* _add_4_children( tnode_t* parent, dbllist_t* children ) {
    for ( int i = 0; i < 4; i++ ) {
        tnode_t* node = tree_new_node( parent, NULL, NULL, 0 );
        dbllist_push_to_end( children, node );
    }
}

static tnode_t* _get_child( dbllist_t* children, int index_quad ) {
    assert ( index_quad >= 0 && QUAD_ILLEGALPARAM );
    assert ( index_quad <= 4 && QUAD_ILLEGALPARAM );

    dblnode_t* node = ( dblnode_t* ) dbllist_head( children );
    for ( int i = 0; i < 4; i++ ) {
        if ( i == index_quad ) {
            return ( tnode_t* ) node->data;
        }
        node = node->next;
    }
    // We should never be here!
    assert ( NULL );
}

qtree_t* qtree_new() {
    qtree_t *q = (qtree_t *) mem_malloc( sizeof( qtree_t ) );
    q->tree = tree_new();
    q->x0 = 0;
    q->y0 = 0;
    q->x1 = ( 1 << REGION_DIM_IN_BITS ) - 1;
    q->y1 = ( 1 << REGION_DIM_IN_BITS ) - 1;
    q->dim = REGION_DIM_IN_BITS;
    q->depth = DEPTH_OF_QTREE;
    return q;
}

qtree_t* qtree_init( qtree_t *q,
        unsigned int x0,
        unsigned int y0,
        unsigned int dim_in_bits,
        unsigned int depth_of_qtree ) {
    assert ( dim_in_bits <= COORDINATE_SIZE_IN_BITS / 2 );
    assert ( depth_of_qtree <= TREE_MAX_DEPTH );

    q->x0 = x0;
    q->y0 = y0;
    q->x1 = ( 1 << dim_in_bits ) + x0 - 1;
    q->y1 = ( 1 << dim_in_bits ) + y0 - 1;
    q->dim = dim_in_bits;
    q->depth = depth_of_qtree;
    return q;
}

void qtree_free( qtree_t *q ) {
    mem_free( q->tree );
    mem_free( q );
}

int qtree_point_index( qtree_t* q, unsigned int x0, unsigned int y0 ) {
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

// Returns a level index where both indexes belong to
//
// @precondition q != NULL
// @postcondition None
// @param q The pointer to the tree
// @param index_0 The index of the 1st quadrant
// @param index_1 The index of the 2nd quadrant
// @return Returns a path of the quadrant where both indexes belong to. For example, if
//         index_0 == b10011100 and index_1 == b10010000, the returned value
//         is 2
unsigned int qtree_common_quad( qtree_t *q, int index_0, int index_1 ) {
    assert( q && QUAD_NOQTREE );

    for ( unsigned int i = 0; i < q->depth; i++ ) {
        int n = 2 * ( q->depth - i );
        int k_0 = ( index_0 & _bit_mask_010( n - 2, n ) ) >> ( n - 2 );
        int k_1 = ( index_1 & _bit_mask_010( n - 2, n ) ) >> ( n - 2 );
        if ( k_0 != k_1 ) {
            return i;
        }
    }
}

// Returns the node at the end of the branch
//
// @precondition q != NULL
// @precondition q->tree != NULL
// @param q The pointer to the tree where the branch is created to
// @param num_of_levels The number of levels to be created
// @param index The index of the branch
// @return The pointer to the node at the end of the branch, or NULL
tnode_t* qtree_get_node( qtree_t *q, unsigned int num_of_levels, int index ) {
    assert( q && QUAD_NOQTREE );
    assert( q->tree && QUAD_NOTREE );

    tnode_t* curr_node = q->tree->root;

    // Handle special case.
    if ( !curr_node ) {
        return NULL;
    }

    for ( unsigned int i = 0; i < num_of_levels; i++ ) {
        int n = 2 * ( q->depth - i );
        int k = ( index & _bit_mask_010( n - 2, n ) ) >> ( n - 2 );
        // Update the current node.
        if ( !curr_node->children ) {
            return NULL;
        }
        curr_node = _get_child( curr_node->children, k );
    }

    return curr_node;
}

// Creates a branch that has the given index into the given tree
//
// @precondition q != NULL
// @precondition q->tree != NULL
// @precondition 0 <= num_of_levels <= q->depth
// @postcondition qtree_get_node( q, num_of_levels, index ) != NULL after qtree_branch( q, index )
// @param q The pointer to the tree where the branch is created to
// @param num_of_levels The number of levels to be created
// @param index The index of the branch
// @return The pointer to the last node of the branch
tnode_t* qtree_branch( qtree_t *q, unsigned int num_of_levels, int index ) {
    assert( q && QUAD_NOQTREE );
    assert( q->tree && QUAD_NOTREE );
    assert( num_of_levels <= q->depth && QUAD_TOODEEP );

    tnode_t* curr_node = q->tree->root;

    // Handle special case.
    if ( curr_node == NULL ) {
        curr_node = tree_new_node( NULL, NULL, NULL, 0 );
        q->tree->root = curr_node;
    }

    for ( unsigned int i = 0; i < num_of_levels; i++ ) {
        // The bit index of the current level.
        int n = 2 * ( q->depth - i );
        // The index of the node in the current level.
        int k = ( index & _bit_mask_010( n - 2, n ) ) >> ( n - 2 );
        // Update the current node.
        if ( !curr_node->children ) {
            curr_node->children = dbllist_new();
            _add_4_children( curr_node, curr_node->children );
        }
        curr_node = _get_child( curr_node->children, k );
    }

    return curr_node;
}

// Inserts a data to the node at the end of the given branch
//
// @precondition q != NULL
// @postcondition qtree_get_node( q, num_of_levels, index ) != NULL after
//                qtree_insert( q, num_of_levels, index, 1, NULL )
// @param q The pointer to the tree where the branch is created to
// @param num_of_levels The level where the node is inserted to
// @param index The index of the branch
// @param parent Make parent nodes as needed (calls qtree_branch( a, num_of_levels, index ))
// @param data The data that is added to the tree
// @return The pointer to the inserted node of the branch, or NULL
tnode_t* qtree_insert( qtree_t *q, unsigned int num_of_levels, int index, int parent, void *data ) {
    assert( q && QUAD_NOQTREE );
    assert( q->tree && QUAD_NOTREE );
    assert( num_of_levels <= q->depth && QUAD_TOODEEP );

    tnode_t* tnode = NULL;

    if ( parent ) {
        tnode = qtree_branch( q, num_of_levels, index );
    } else {
        tnode = qtree_get_node( q, num_of_levels, index );
    }

    // Handle the case when the node does not exist.
    if ( tnode == NULL ) {
        return NULL;
    }

    if ( !tnode->data ) {
        tnode->data = ( void * ) dbllist_new();
    }
    dbllist_push_to_end( ( dbllist_t* ) tnode->data, data );

    return tnode;
}

// [Depracated] Returns a path of the quadrant that contains both points, tl and br
//
// @param q The pointer to the quad structure.
// @param index_tl The index of the quadrant that contains the tl.
// @param index_br The index of the quadrant that contains the br.
// @return The path, e.g. "00.01"; NULL if there is no quadrant that contains the
//      tl and/or br.
// @depracated
char* qtree_node_path( qtree_t* q, int index_tl, int index_br ) {
    if ( index_tl == COORDINATE_OUSIDE || index_br == COORDINATE_OUSIDE ) {
        return NO_QUAD;
    }
    // Reserve space for the name. The path has three letters, e.g., "01."
    // except the last one that ends to the null character, '\0'.
    char* name = (char *) mem_malloc( ( 3 * q->depth + 1 ) * sizeof( char ) );
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
        strcat( name, QTREE_PATHS[ mask_tl >> j ] );
    }
    if ( k < 0 ) {
        k = 0;
    }
    name[ k ] = '\0';
    return name;
}

// [Depracated] Splits the given path in parts
//
// @param path The path to be splitted. The format of the path is 'a.b', where
//             '.' is the separator
// @return The array of the strings, e.g., [ "a", "b" ]
// @depracated
char** qtree_split_path( char *path ) {

    // Special case. For the root, the path == NULL.
    if ( path == NULL ) {
        return NULL;
    }

    // The number of levels in the path - 1.
    int num_of_lvls = 0;
    // The current read index.
    int index_curr = 0;
    // The index of the previous '.'.
    int index_prev = -1;
    // If 0, there is no errors.
    int error = 0;

    char** lvl_names = mem_malloc( sizeof( char* ) * TREE_MAX_DEPTH );
    for ( int i = 0; i < TREE_MAX_DEPTH; i++ ) {
        lvl_names[i] = NULL;
    }

    while( 1 ) {
        char c = path[index_curr];
#ifdef DEBUG
        // Do the sanity check.
        if ( !isalpha(c) && !isdigit(c) && c != '.' && c != '\0' ) {
            error = ERROR_INVALID_CHAR;
            break;
        }
#endif // DEBUG
        // '+1' because the last acceptable char is '.', e.g. 'itisamax.'.
        if ( index_curr - index_prev > LEVEL_NAME_MAX_LENGTH + 1 ) {
            error = ERROR_NAME_TOO_LONG;
            break;
        }
        if ( num_of_lvls == TREE_MAX_DEPTH ) {
            error = ERROR_TREE_TOO_DEEP;
            break;
        }
        if ( c == '.' || c == '\0' ) {
            // If the level has no name, raise the error.
            if ( index_curr == index_prev + 1 ) {
                error = ERROR_NO_NAME;
                break;
            }
            // 'index_curr - index_prev' is the length.
            char *lvl_name = mem_malloc( sizeof( char ) * ( index_curr - index_prev ) );
            for ( int i = index_prev, j = 0; i < index_curr; i++, j++ ) {
                lvl_name[ j ] = path[ i + 1 ];
            }
            lvl_name[ index_curr - index_prev - 1 ] = '\0';
            lvl_names[ num_of_lvls++ ] = lvl_name;
            index_prev = index_curr;
        }
        // This will ensure that the loop will end.
        if ( c == '\0' ) {
            break;
        }
        index_curr += 1;
    }

    if ( error ) {
        for( int i = 0; i < TREE_MAX_DEPTH; i++ ) {
            if ( lvl_names[i] != NULL ) {
                mem_free( lvl_names[i] );
            }
        }
        mem_free( lvl_names );
#ifdef LOGGING
        printf("Error when splitting a path %s: %d\n", path, error);
#endif // LOGGING
        return NULL;
    }

    return lvl_names;
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
