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
#define TREE_NOROOT "Root does not exist"
#define TREE_RELEASENONEMPTYLIST "Releasing non-empty tree"
#define TREE_NEWNULL "New node cannot be Null"
#define TREE_NOPARENT "Parent does not exist"

// The max length of the name of the (tree) level
#define LEVEL_NAME_MAX_LENGTH 8
// The maximal depth of the tree
#define TREE_MAX_DEPTH 8

// Return values
#define SUCCESS                 0
#define ERROR_INVALID_CHAR      -10
#define ERROR_NAME_TOO_LONG     -15
#define ERROR_TREE_TOO_DEEP     -20
#define ERROR_NO_NAME           -25
#define ERROR_TREE_IS_EMPTY     -30
#define ERROR_NO_PARENT         -35
#define ERROR_NO_REPLACEMENT    -40
#define ERROR_NOT_FOUND    		-45

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
// @param parents Make parent as needed
// @param replace Replace old data if exists
// @return The old data [void *] of the tree node
void* tree_insert( TTree* tree, char* path, void* new_data, int parents, int replace );

// Finds a node from the tree
//
// @param tree The pointer to a tree where the node is to be inserted
// @param path A string to define the parent of the node; NULL if the node
//             is the root. The path is like 'a.b.c', where 'a', 'b' and 'c'
//             are the names of the nodes
// @param list The address to the list. The function will return the list where
//			   the node is stored if the node is found; Otherwise it is not
//			   modified
// @return The three node in the path or NULL
TNode* tree_find( TTree *tree, char* path, struct DblLinkedList **list );

// Removes the node and its subtree from the tree.
//
// @param tree The pointer to a tree where the node is to be removed
// @param path A string to define the parent of the node; NULL if the node
//             is the root. The path is like 'a.b.c', where 'a', 'b' and 'c'
//             are the names of the nodes
// @param free The pointer to a function that releases data members of the
//			   tree nodes
// @return The address parent of the node
void tree_remove( TTree *tree, char* path, void (*free)( void* ) );

// Splits the path in two parts.
//
// @param path A string to define the parent of the node; NULL if the node
//             is the root. The path is like 'a.b.c', where 'a', 'b' and 'c'
//             are the names of the nodes
// @return Let the path be 'a.b.c'. Then [0] == 'a' and [1] == 'b.c'
char** tree_split_path( char *path );

// Internal function. Removes the node and its subtree from the tree.
//
// @param list The pointer to the list that should be removed
// @param free The pointer to a function that releases data members of the
//			   tree nodes
// @return The address parent of the node
void _tree_remove_subtree( struct DblLinkedList* list, void (*free)( void* ) );

void _clean_up( char** names );

#endif // _tree_