// @todo TD1. The tree_remove() should remove the first node as well
// 
// @todo TD2. How the lvl_names are handled in tree_insert(). Will they leak?
//
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

void* tree_insert( TTree* tree, char* path, void* new_data, int parents, void (*insert)( int, void*, void* ) ) {
    assert ( !_is_empty(tree) && TREE_NOROOT );

    // A current node of the tree.
    TNode* tree_node = tree->root;

    // The special case: Insert data into the root node. Note that we will
    // handle this here in order to avoid memory allocations/releases.
    if ( path == NULL ) {
        if ( tree_node->data ) {
#ifdef LOGGING
            printf("Error in tree_insert: %d\n. Use tree_find to modify", ERROR_NO_REPLACEMENT );
#endif // LOGGING
            return NULL;
        }
        tree_node->data = new_data;
        return new_data;                
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
        Node *child = dbllist_head( tree_node->children );
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
            // TD2. The node has found. No need for the name. Release it.
            mem_free( lvl_name );

            if ( !is_last ) {
                tree_node = (TNode *) child->data;
                index++;
                continue;
            }
            // Handle the case where the child is the last one.
            if ( child->data ) {
#ifdef LOGGING
                printf("Warning in tree_insert: %d\n. Use tree_find to modify", WARNING_VALUE_REPLACEMENT );
#endif // LOGGING
                insert( WARNING_VALUE_REPLACEMENT, ((TNode *) child->data)->data, new_data );
            } else {
                ((TNode *) child->data)->data = new_data;
                insert( error, NULL, new_data );
            }
        } else {
            if ( is_last || parents ) {
                TNode *new_node = (TNode *) mem_malloc( sizeof( TNode ) );
                new_node->name = lvl_name;
                new_node->data = is_last ? new_data : NULL;
                new_node->children = dbllist_new();
                // Insert the new node. Preserve the order.
                dbllist_push_to_end( tree_node->children, new_node );
                // Call the insert function.
                insert( error, NULL, new_data );
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

    // TD2. This will release the names and the array of names.
    if( error ) {
        _clean_up( lvl_names );
        return NULL;
    }

    // TD2. This will release only the array.
    mem_free( lvl_names );

    return new_data;
}

TNode* tree_find( TTree *tree, char* path, DblLinkedList **list ) {
    assert ( !_is_empty(tree) && TREE_NOROOT );

    // A current node of the tree in this traversal.
    TNode* tree_node = tree->root;
    // The list where the node is found.
    DblLinkedList *new_list = NULL;

    // Handle special case.

    if ( path == NULL ) {
        *list = tree_node->children;
        return tree_node;
    }

    // Names of the hierarchy levels of the tree.
    char** lvl_names = tree_split_path( path );
    // A name of the current level of the tree.
    char* lvl_name = NULL;
    // The index to the lvl_names.
    int index = 0;
    // The current child (list) node.
    Node *child = NULL;

    while( index < TREE_MAX_DEPTH && ( lvl_name = lvl_names[ index ] ) != NULL ) {
        int found = 0;
        child = dbllist_head( tree_node->children );
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

        if ( !found ) {
#ifdef LOGGING
            printf( "Error in tree_insert: %d\n", ERROR_NOT_FOUND );
#endif // LOGGING
            return NULL;
        }

        new_list = tree_node->children;
        tree_node = (TNode *) child->data;
        index++;
    }

    *list = new_list;

    return tree_node;
}

void tree_remove( TTree *tree, char* path, void (*free)( void* ) ) {
    DblLinkedList *list = NULL;

    TNode *tree_node = tree_find( tree, path, &list );

    if ( tree_node != NULL ) {
        _tree_remove_subtree( list, free );
    }

    free( tree_node->data );
}

void _tree_remove_subtree( DblLinkedList* list, void (*free)( void* ) ) {
    if ( list != NULL ) {
#ifdef LOGGING
        printf( "%s: %lx\n", "Removing a list", (long unsigned int) list );
#endif
        while( !dbllist_is_empty( list ) ) {
            TNode* tree_node = (TNode *) dbllist_pop( list );
#ifdef LOGGING
            printf( "%s: %lx\n", "Moving to a node", (long unsigned int) tree_node );
#endif            
            if ( tree_node->children ) {
                if ( dbllist_size( tree_node->children ) ){
                    _tree_remove_subtree( tree_node->children, free );
                }
            }
            if ( tree_node->name ) {
                mem_free( tree_node->name );
            }
            free( tree_node->data );
        }
        dbllist_free( list );
        list = NULL;
#ifdef LOGGING
        printf("%s: %lx\n", "The list is released. Exiting", (long unsigned int) list);
#endif
    }
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

DblLinkedList* tree_to_list( TTree* tree ) {
    // A current node of the tree in this traversal.
    TNode* tree_node = tree->root;
    // The list where the node is found.
    DblLinkedList *new_list = dbllist_new();

    if ( tree_node ) {
        // Add the root to the list.
        dbllist_push_to_end( new_list, tree_node );
        // Handle all the children.
        if ( tree_node->children ) {
            _tree_to_list( tree_node->children, new_list );
        }
    }

    return new_list;
}

void _tree_to_list( DblLinkedList* list, DblLinkedList* new_list ) {
    if ( dbllist_size( list ) ) {
        Node *node = dbllist_head( list );
        while( node ) {
            dbllist_push_to_end( new_list, node->data );
            if ( ( (TNode *) node->data )->children ) {
                _tree_to_list( ( (TNode *) node->data )->children, new_list );
            }
            node = node->next;
        }
    }
}

void _clean_up( char** names ) {
    for( int i = 0; i < TREE_MAX_DEPTH; i++ ) {
        if ( names[i] != NULL ) {
            mem_free( names[i] );
        }
    }
    mem_free( names );
}
