// @todo TD1. The tree_remove() should remove the first node as well
// 
// @todo TD2. How the lvl_names are handled in tree_insert(). Will they leak?
//
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../defs.h"
#include "../mem.h"
#include "tree.h"

#define _is_empty(x) ((x)->root == NULL)

tree_t* tree_new() {
    tree_t *tree = (tree_t *) mem_malloc( sizeof( tree_t ) );
    tree->root = NULL;
    return tree;
}

void tree_free( tree_t* tree ) {
    assert ( _is_empty(tree) && TREE_RELEASENONEMPTYLIST );
    mem_free( tree );
}

// @param tree The tree where the node is added to
// @param parent The parent node for the inserted node
// @param new_node The node to be inserted as a child for the parent
// @return The inserted node for chaining
tnode_t* tree_insert( tree_t* tree, tnode_t* parent, tnode_t* new_node ) {
    assert ( new_node && TREE_NONODE );

    if ( parent ) {
        if ( !parent->children ) {
            parent->children = dbllist_new();
        }
        new_node->parent = parent;
        dbllist_push_to_end( parent->children, new_node );
    } else {
        new_node->parent = NULL;
        tree->root = new_node;
    }

    return new_node;
}

// @param tree The tree that contains the root
// @param node The node of the subtree that is to be removed
// @param free The pointer to a callback function
void tree_remove( tree_t* tree, tnode_t* node, void (*_free)( void* ) ) {
    assert ( tree && TREE_NOTREE );

    // Handle special case.
    if ( node == NULL ) {
        return;
    }
    // Remove the subtree excluding the node.
    if ( node->children ) {
        _tree_remove_subtree( node->children, _free );
    }
    // Remove the node from its parent's list.
    if ( node->parent ) {
        dbllist_t *parents_children = node->parent->children;
        dbllist_delete( parents_children, node );
        if ( dbllist_is_empty( parents_children ) ) {
            dbllist_free( parents_children );
            node->parent->children = NULL;
        }
    } else {
        tree->root = NULL;
    }
    // Remove the node itself.
    _free( node->data );
    mem_free( node->name );
    mem_free( node );
}

// @param children The list of the children and their subtrees to be removed
// @param free The pointer to a callback function. The function must release
//             the data of the removed node
void _tree_remove_subtree( dbllist_t* children, void (*_free)( void* ) ) {
    assert ( children && TREE_NOLIST );

    while( !dbllist_is_empty( children ) ) {
        tnode_t* tnode = (tnode_t *) dbllist_pop( children );
        if ( tnode->children ) {
            _tree_remove_subtree( tnode->children, _free );
        }
        _free( tnode->data );
        mem_free( tnode->name );
        mem_free( tnode );
    }
    dbllist_free( children );
}

// @param root The root of the subtree that is to be converted to a list
// @return A list of the nodes of the tree
dbllist_t* tree_to_list( tnode_t* root ) {
    assert ( root && TREE_NOROOT );

    // The list where the nodes are added to.
    dbllist_t *output = dbllist_new();

    dbllist_push( output, root );
    if ( root->children ) {
        _tree_to_list( root->children, output );
    }

    return output;
}

// @param children The list of children
// @param output The output list
void _tree_to_list( dbllist_t* children, dbllist_t* output ) {
    if ( dbllist_size( children ) ) {
        dblnode_t *node = dbllist_head( children );
        while( node ) {
            dbllist_push_to_end( output, node->data );
            if ( ( (tnode_t *) node->data )->children ) {
                _tree_to_list( ( (tnode_t *) node->data )->children, output );
            }
            node = node->next;
        }
    }
}
