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
    TTree *qtree;
    unsigned int x0;
    unsigned int y0;
    unsigned int x1;
    unsigned int y1;
    unsigned int dim;
    unsigned int depth;
} QStruct;

unsigned int _bit_mask_011(unsigned int x);

unsigned int _bit_mask_010(unsigned int x, unsigned int y);

// Creates a new array of QNodes
QStruct* quad_new();

// Creates a new array of QNodes
QStruct* quad_new_and_init(
    unsigned int x0,
    unsigned int y0,
    unsigned int dim_in_bits,
    unsigned int depth_of_qtree );

// Releases the array
void quad_free( QStruct *q );

int quad_point_index( QStruct* qstruct, unsigned int x0, unsigned int y0 );

char* quad_node_path( QStruct* q, int index_tl, int index_br );

// Inserts a new node to the array
// QNode* quad_insert_rcrsv( QArray* tree, void* new_data, int x0, int y0, int x1, int y1 );

// QNode* _quad_insert_rcrsv( QNode* node, void* new_data, int x0, int y0, int x1, int y1, int tl_x, int tl_y, int br_x, int br_y, int d );

// QNode* new_qnode();

// int _get_quad( int mp_x, int mp_y, int x, int y );

// void quad_del_subtree();

// void* quad_del( QArray *tree, void* data );

// // Deletes the node from the array

// // @return The index of the array of the game object
// int quad_index( int x0, int y0, int x1, int y1 );

#endif // _quadtree_