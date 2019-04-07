#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "../defs.h"
#include "../mem.h"
#include "tree.h"

#define _is_empty(x) ((x)->root == NULL)

TTree* tree_new() {
    TTree *tree = (TTree *) mem_malloc( sizeof( TTree ) );
    tree->root = NULL;
    return tree;
}

void tree_free( TTree* tree ) {
    assert ( _is_empty(tree) && TREE_RELEASENONEMPTYLIST );
    mem_free( tree );
}

void* tree_insert( TTree* tree, char* path, void* new_data, int parents, int replace ) {
    assert ( !_is_empty(tree) && TREE_NOROOT );

    // A current node of the tree.
    TNode* tree_node = tree->root;
    // The existing data of a node. This is a value of TNode.data.
    void* old_data = NULL;

    // The special case: Insert data into the root node. Note that we will
    // handle this here in order to avoid memory allocations/releases.
    if ( path == NULL ) {
        if ( tree_node->data ) {
            if ( replace ) {
                old_data = tree_node->data;
                tree_node->data = new_data;
                return old_data;
            } else {
#ifdef LOGGING
                printf("Error in tree_insert: %d\n", ERROR_NO_REPLACEMENT );
#endif // LOGGING
            }
        }
        tree_node->data = new_data;
        return NULL;                
    }

    // Names of the levels of the tree.
    char** lvl_names = tree_split_path( path );
    // A name of the level of the tree.
    char* lvl_name = NULL;
    // The index to the lvl_names.
    int index = 0;
    // If 0, there is no error.
    int error = SUCCESS;

    // Insert the node(s). Note that the function is not transactional meaning
    // that it may create parent nodes even the operation is not completed due
    // to errors.
    while( !error && index < TREE_MAX_DEPTH && ( lvl_name = lvl_names[ index ] ) != NULL ) {
        // We need this when deciding whether to create a parent or not.
        int is_last = index == TREE_MAX_DEPTH - 1 || lvl_names[ index + 1 ] == NULL;
        // 1 if we find the tree node that has a name lvl_name.
        int found = 0;
        // The current list node.
        struct Node *child = dbllist_head( tree_node->children );
        // Children -> +[Node]--------------+ -> ...
        //             | ->data             |
        //             |  +[TNode]--------+ |
        //             |  | ->data        | |
        //             |  |  +[void*]---+ | |
        //             |  |  |          | | |
        //             :  :  :          : : :
        while( child ) {
            if ( strcmp( lvl_name, ((TNode *) child->data)->name ) == 0 ) {
#ifdef LOGGING
                printf("Node found: %s\n", lvl_name );
#endif // LOGGING
                found = 1;
                break;
            }
            child = child->next;
        }

        if ( found ) {
            if ( !is_last ) {
                tree_node = (TNode *) child->data;
                index++;
                continue;
            }
            // Handle the case where the child is the last one and we will
            // replace its data.
            if ( replace ) {
                tree_node = (TNode *) child->data;
                old_data = tree_node->data;
                tree_node->data = new_data;
                break;
            } else {
#ifdef LOGGING
                printf("Error in tree_insert: %d\n", ERROR_NO_REPLACEMENT );
#endif // LOGGING
                error = ERROR_NO_REPLACEMENT;
                break;
            }
        } else {
            if ( is_last || parents ) {
                TNode *new_node = (TNode *) mem_malloc( sizeof( TNode ) );
                new_node->name = lvl_name;
                new_node->data = is_last ? new_data : NULL;
                new_node->children = dbllist_new();
                // Insert the new node. Preserve the order.
                dbllist_push_to_end( tree_node->children, new_node );
                // Move on.
                tree_node = new_node;
            } else {
#ifdef LOGGING
                printf("Error in tree_insert: %d\n", ERROR_NO_PARENT );
#endif // LOGGING
                error = ERROR_NO_PARENT;
                break;
            }
        }
        index++;
    }

    if( error ) {
        _clean_up( lvl_names );
        return NULL;
    }

    mem_free( lvl_names );

    return old_data;
}

TNode* tree_find( TTree *tree, char* path ) {
}

TNode* tree_remove( TTree *tree, char* path, TNode node ) {
}

char** tree_split_path( char *path ) {

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

void* tree_del( TTree *tree ) {
    // Traverse the tree. Add all the nodes to the list.
    // Delete nodes from the list.
}

void _clean_up( char** names ) {
    for( int i = 0; i < TREE_MAX_DEPTH; i++ ) {
        if ( names[i] != NULL ) {
            mem_free( names[i] );
        }
    }
    mem_free( names );
}
