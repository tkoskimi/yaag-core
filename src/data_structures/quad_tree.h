// Quad tree
// 
// A quad tree is a tree in which each internal node has four childeren. The
// quad trees are typically used to partition two-dimenstional space.
//
// The root of the tree is a node without ancestors.
//
// The head of the list is the first node. The tail of the list is the last node.
//
// The operations are insert and del.
//
// (c) Tuomas Koskimies, 2019

#ifndef _quadtree_
#define _quadtree_

#include "./doublyLinkedList.h"

// Messages for the diagnostics
#define QUAD_NEWNULL "New node cannot be Null"
#define QUAD_RELEASENONEMPTYLIST "Releasing non-empty tree"
#define QUAD_TOO_BIG "Box is bigger than the region"
#define QUAD_NONPOSITIVE_DIMENSIONS "Dimenstions must be positive"
#define QUAD_TREE_NULL "Tree must be non-null"
#define QUAD_BOX_POS_ORIENTATION "Bounding box must have negative orientation"

#define REGION_WIDTH 2^16
#define REGION_HEIGHT 2^16
#define ORIGO_X 0
#define ORIGO_Y 0
#define MIN_SIZE 32

// The problem is as follows. Assume a set of rectangles in a world coordinate system,
// namely {R}. We want to know which one of them intersects a given a rectangle R.

// Let's assume that we have a complete quad tree. We take the R and find all the rectangles that
// are not located into separate partitions. Two rectangles are in a separate partition if they do
// not have a common ancestor.

// How do you know that two rectangles have a common ancestor? One has to traverse the tree. If
// gets the rectangles Rn1, Rn2, ... from the level n, and R(n+1)1, R(n+1)2, ... on the level n+1,
// then the intersections between Rn1, Rn2, 

// Depth-first search is the one here. You will get one subtree at time. You will have
// 

//

struct QNode {
    int index;
    void* boxes[10];
    struct QNode *tl;
    struct QNode *tr;
    struct QNode *bl;
    struct QNode *br;
};

struct QTree;

struct QTree* quad_new();

void quad_free( struct QTree* tree );

struct QNode* quad_insert_rcrsv( struct QTree* tree, void* new_data, int x0, int y0, int x1, int y1 );

struct QNode* _quad_insert_rcrsv( struct QNode* node, void* new_data, int x0, int y0, int x1, int y1, int tl_x, int tl_y, int br_x, int br_y, int d );

struct QNode* new_qnode();

int _get_quad( int mp_x, int mp_y, int x, int y );

void quad_del_subtree();

void* quad_del( struct QTree *tree, void* data );

#endif // _quadtree_