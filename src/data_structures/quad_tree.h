// Quad tree
// 
// A quad tree is a tree in which each internal node has four childeren. The
// quad trees are typically used to partition two-dimenstional space.
//
// The root of the tree is a node without ancestors.
//
// The head of the list is the first node. The tail of the list is the last node.
//
// (c) Tuomas Koskimies, 2019

#ifndef _quadtree_
#define _quadtree_

#include <limits.h>

#include "./doublyLinkedList.h"
#include "./tree.h"

// Messages for the diagnostics
#define QUAD_NOQTREE "Q-tree does not exist"
#define QUAD_NOTREE "Tree does not exist"
#define QUAD_NOROOT "The root does not exist"
#define QUAD_ILLEGALPARAM "Illegal parameter"
#define QUAD_TOODEEP "Number of levels exceeds the depth of the tree"
#define QUAD_TOO_BIG "Box is bigger than the region"
#define QUAD_NONPOSITIVE_DIMENSIONS "Dimenstions must be positive"
#define QUAD_BOX_POS_ORIENTATION "Bounding box must have negative orientation"

// Definitions
#define REGION_DIM_IN_BITS    10
#define DEPTH_OF_QTREE        3 

// Return values
#define NO_QUAD              NULL
#define COORDINATE_OUSIDE    -1

typedef struct {
    tree_t *tree;
    unsigned int x0;
    unsigned int y0;
    unsigned int x1;
    unsigned int y1;
    unsigned int dim;
    unsigned int depth;
} qtree_t;

// Returns a bit mask whose first n bits are 1s
//
// @param n The index of the first bit that is set to 1.
// @return The bit mask. For example, if n is 5 the return value is b00011111.
// @see https://stackoverflow.com/questions/8774567/
//      c-macro-to-create-a-bit-mask-possible-and-have-i-found-a-gcc-bug
unsigned int _bit_mask_011(unsigned int n);

// Returns a bit mask whose bits are 1s in the range (m+1,n); Others are 0s
//
// @param m The index of the last 0 before 1s.
// @param n The index of the last 1.
// @return The bit mask. For example, if m is 2 and n is 5 the return value is
//      b00011100.
// @see https://stackoverflow.com/questions/8774567/
//      c-macro-to-create-a-bit-mask-possible-and-have-i-found-a-gcc-bug
unsigned int _bit_mask_010(unsigned int m, unsigned int n);

qtree_t* qtree_new();

qtree_t* qtree_init( qtree_t *q,
        unsigned int x0,
        unsigned int y0,
        unsigned int dim_in_bits,
        unsigned int depth_of_qtree );

void qtree_free( qtree_t *q );

// Returns an index of a quadrants that the point is belonging to
//
// @param q The pointer to the quad structure.
// @param x0 The x coordinate of the point.
// @param y0 The y coordinate of the point.
// @return The index of the point. The value is explained below:
//
// The coordinates are shifted to start from the beginning of a memory unit.
// Hence, we are interested in coordinates that have following format:
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
// dimensions, we need both coordinates, x and y.
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
// @see Test cases
int qtree_point_index( qtree_t* qtree_t, unsigned int x0, unsigned int y0 );

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
unsigned int qtree_common_quad( qtree_t *q, int index_0, int index_1 );

// Returns the node at the end of the branch
//
// @precondition q != NULL
// @precondition q->tree != NULL
// @param q The pointer to the tree where the branch is created to
// @param num_of_levels The number of levels to be created
// @param index The index of the branch
// @return The pointer to the node at the end of the branch, or NULL
tnode_t* qtree_get_node( qtree_t *q, unsigned int num_of_levels, int index );

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
tnode_t* qtree_branch( qtree_t *q, unsigned int num_of_levels, int index );

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
tnode_t* qtree_insert( qtree_t *q, unsigned int num_of_levels, int index, int parent, void *data );

// Implements a general traverse functionality for the given subtree
//
// @precondition root != NULL
// @postcondition The callback may change the content of the tree
// @param root The root of the subtree that is being traversed
// @param f The callback function. It has two parameters. The first one is the
//          current node and the second one is a list that is collected from
//          the subtree.
void qtree_dft( tnode_t* root, dbllist_t* (*_f)( tnode_t* tnode, dbllist_t* lst ) );

// [Deprecated] Returns a path of the quadrant that contains both points, tl and br
//
// @param q The pointer to the quad structure.
// @param index_tl The index of the quadrant that contains the tl.
// @param index_br The index of the quadrant that contains the br.
// @return The path, e.g. "00.01"; NULL if there is no quadrant that contains the
//      tl and/or br.
// @deprecated
char* qtree_node_path( qtree_t* q, int index_tl, int index_br );

// [Deprecated] Splits the given path in parts
//
// @param path The path to be splitted. The format of the path is 'a.b', where
//             '.' is the separator
// @return The array of the strings, e.g., [ "a", "b" ]
// @deprecated
char** qtree_split_path( char *path );

#endif // _quadtree_