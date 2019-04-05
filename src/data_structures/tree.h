// A tree
// 
// A tree is a well-known data structure that can be defined recursively
// with a node and its children.
//
// @author Tuomas Koskimies

#ifndef _tree_
#define _tree_

#include "./doublyLinkedList.h"

// Messages for the diagnostics
#define TREE_RELEASENONEMPTYLIST "Releasing non-empty tree"
#define TREE_NEWNULL "New node cannot be Null"
#define TREE_NOPARENT "Parent does not exist"

// The max length of the name of the (tree) level
#define LEVEL_NAME_MAX_LENGTH 8
// The maximal depth of the tree
#define TREE_MAX_DEPTH 8

// Return values
#define ERROR_INVALID_CHAR  -10
#define ERROR_NAME_TOO_LONG -15
#define ERROR_TREE_TOO_DEEP -20
#define ERROR_NO_NAME       -25

typedef struct {
    char *name;
    void *data;
    struct DblLinkedList *children;
} TNode;

typedef struct {
    TNode *root;
} TTree;

TTree* tree_new();

void tree_free( TTree* tree );

// Inserts a new node to the tree
//
// @param tree The pointer to a tree where the node is to be inserted
// @param path A string to define the parent of the node; NULL if the node
//             is the root. The path is like 'a.b.c', where 'a', 'b' and 'c'
//             are the names of the nodes
// @param new_data The data to be added to the node
// @return The address of the new node
TNode* tree_insert( TTree* tree, char* path, void* new_data );

// Removes the node from the tree.
//
// @param tree The pointer to a tree where the node is to be removed
// @param path A string to define the parent of the node; NULL if the node
//             is the root. The path is like 'a.b.c', where 'a', 'b' and 'c'
//             are the names of the nodes
// @param node The node to be removed
// @return The address parent of the node
TNode* tree_remove( TTree *tree, char* path, TNode node );

// Splits the path in two parts.
//
// @param path A string to define the parent of the node; NULL if the node
//             is the root. The path is like 'a.b.c', where 'a', 'b' and 'c'
//             are the names of the nodes
// @return Let the path be 'a.b.c'. Then [0] == 'a' and [1] == 'b.c'
char** tree_split_path( char *path );

void* tree_del( TTree *tree );

#endif // _tree_