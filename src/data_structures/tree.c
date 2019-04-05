#include <assert.h>
#include <ctype.h>
#include <stdio.h>

#include "../defs.h"
#include "../mem.h"
#include "tree.h"

#define _is_empty(x) ((x)->root != NULL)

TTree* tree_new() {
    TTree *tree = (TTree *) mem_malloc( sizeof( TTree ) );
    return tree;
}

void tree_free( TTree* tree ) {
    assert ( _is_empty(tree) && TREE_RELEASENONEMPTYLIST );
    mem_free(tree);
}

TNode* tree_insert( TTree* tree, char* path, void* new_data ) {
    if ( path == NULL || path[0] == '\0' ) { // No path
        return NULL;
    }
    // Split path
    // Go through the array. Create new nodes along the way
    // Insert the data. If the node exists update the data
}

TNode* tree_remove( TTree *tree, char* path, TNode node ) {
    // Split the path
    // Find the node. If exists, then remove it from the list
}

char** tree_split_path( char *path ) {

    int num_of_lvls = 0; // The number of levels in the path - 1
    int index_curr = 0; // The current read index
    int index_prev = -1; // The index of the previous '.'
    int error = 0; // If 0, there is no errors

    char** lvl_names = mem_malloc( sizeof( char* ) * TREE_MAX_DEPTH );
    for ( int i = 0; i < TREE_MAX_DEPTH; i++ ) {
        lvl_names[i] = NULL;
    }

    while( 1 ) {
        char c = path[index_curr];
#ifdef DEBUG
        // Do the sanity check
        if ( !isalpha(c) && !isdigit(c) && c != '.' && c != '\0' ) {
            error = ERROR_INVALID_CHAR;
            break;
        }
#endif // DEBUG
        // '+1' because the last acceptable char is '.', e.g. 'itisamax.'
        if ( index_curr - index_prev > LEVEL_NAME_MAX_LENGTH + 1 ) {
            error = ERROR_NAME_TOO_LONG;
            break;
        }
        if ( num_of_lvls == TREE_MAX_DEPTH ) {
            error = ERROR_TREE_TOO_DEEP;
            break;
        }
        if ( c == '.' || c == '\0' ) {
            // If the level has no name, raise the error
            if ( index_curr == index_prev + 1 ) {
                error = ERROR_NO_NAME;
                break;
            }
            // 'index_curr - index_prev' is the length
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
#endif // DEBUG
        return NULL;
    }

    return lvl_names;
}

void* tree_del( TTree *tree ) {
    // Traverse the tree. Add all the nodes to the list.
    // Delete nodes from the list.
}

