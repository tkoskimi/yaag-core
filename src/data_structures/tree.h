// A tree
// 
// A tree is a well-known data structure that can be defined recursively
// with a node and its children.
//
// @author Tuomas Koskimies

#ifndef _tree_
#define _tree_

#include "./doublyLinkedList.h"
#include "../obj.h"

// Messages for the diagnostics
#define TREE_NOTREE "Tree does not exist"
#define TREE_NOROOT "Root does not exist"
#define TREE_NOLIST "The list of children does not exist"
#define TREE_RELEASENONEMPTYLIST "Releasing non-empty tree"
#define TREE_NEWNULL "New node cannot be Null"
#define TREE_NOPARENT "Parent does not exist"
#define TREE_NONODE "Parameter node does not exist"

// The max length of the name of the (tree) level
#define LEVEL_NAME_MAX_LENGTH 8
// The maximal depth of the tree
#define TREE_MAX_DEPTH 8

// Return values
#define SUCCESS                     0
#define WARNING_VALUE_REPLACEMENT   10
#define ERROR_INVALID_CHAR          -10
#define ERROR_NAME_TOO_LONG         -15
#define ERROR_TREE_TOO_DEEP         -20
#define ERROR_NO_NAME               -25
#define ERROR_TREE_IS_EMPTY         -30
#define ERROR_NO_PARENT             -35
#define ERROR_NOT_FOUND             -45

typedef struct tnode_t {
    obj_t* obj;
    char *name;
    void *data;
    struct tnode_t* parent;
    dbllist_t *children;
} tnode_t;

typedef struct {
    tnode_t *root;
} tree_t;

// Creates a new tree
tree_t* tree_new();

// Creates and initializes a new node
tnode_t* tree_new_node( tnode_t* parent, char* name, void* data, int children );

// Releases the tree
void tree_free( tree_t* tree );

// Inserts a new node as a child of the parent
//
// @precondition new_node != NULL
// @postcondition new_node->parent == parent
// @postcondition new_node in parent->children
// @postcondition parent == NULL => tree->root == new_node && new_node->parent == NULL
// @param tree The tree where the node is added to
// @param parent The parent node for the inserted node
// @param new_node The node to be inserted as a child for the parent
// @return The inserted node for chaining
tnode_t* tree_insert( tree_t* tree, tnode_t* parent, tnode_t* new_node );

// Removes a node and its subtree from the tree
//
// @precondition tree != null
// @postcondition The node and its subtree are removed
// @postcondition If tree->root == node, then tree->root == NULL
// @param tree The tree that contains the root
// @param node The node of the subtree that is to be removed
// @param free The pointer to a callback function
void tree_remove( tree_t* tree, tnode_t* node, void (*_free)( void* ) );

// (Internal use only.) Removes a list of nodes and all of their subtrees
//
// @precondition children != NULL
// @postcondition The list of children and the subtree of each child are removed
// @param children The list of the children and their subtrees to be removed
// @param free The pointer to a callback function. The function must release
//             the data of the removed node
void _tree_remove_subtree( dbllist_t* children, void (*_free)( void* ) );

// Converts a tree to the list
//
// @precondition root != null
// @postcondition None
// @param root The root of the subtree that is to be converted to a list
// @return A list of the nodes of the tree
dbllist_t* tree_to_list( tnode_t* root );

// (Internal use only.) Appends recursively tree nodes to the list.
//
// @precondition children != null
// @postcondition None
// @param children The list of children
// @param output The output list
void _tree_to_list( dbllist_t* children, dbllist_t* output );

#endif // _tree_
