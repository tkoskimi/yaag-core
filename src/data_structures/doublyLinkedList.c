// Doubly Linked List
//
// [Implementation details]
//
// (c) Tuomas Koskimies, 2018

#include <assert.h>
#include <stdlib.h>

#include "./doublyLinkedList.h"
#include "../defs.h"
#include "../mem.h"

#define _is_empty(x) ((x)->size == 0)

dbllist_t* dbllist_new() {
    dbllist_t *list = (dbllist_t *) mem_malloc( sizeof( dbllist_t ) ); 
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
    return list;
}

void dbllist_free( dbllist_t* list ) {
    assert ( _is_empty(list) && DBLL_RELEASENONEMPTYLIST );
    mem_free(list);
}

dbllist_t* dbllist_append( dbllist_t* dst, dbllist_t* src ) {
    if ( src != NULL && !_is_empty( src ) ) {
        dblnode_t *node = src->head;
        while ( node != NULL ) {
            dbllist_push_to_end( dst, node->data );
            node = node->next;
        }
    }
    return dst;
}

dbllist_t* dbllist_join( dbllist_t* lst_0, dbllist_t* lst_1 ) {
    dbllist_t *new_lst = dbllist_new();
    dbllist_append( new_lst, lst_0 );
    dbllist_append( new_lst, lst_1 );
    return new_lst;
}

dblnode_t* dbllist_push( dbllist_t* list, void* new_data ) {
    assert( new_data != NULL && DBLL_NEWNULL );

    dblnode_t *node = (dblnode_t*) mem_malloc( sizeof( dblnode_t ) );
    node->data = new_data;
    node->next = list->head;
    node->prev = NULL;
    list->head = node;

    // Set the tail if the list is empty. Note that the tail will not change
    // later if using push function. If the list is non-empty, then the node
    // will not be the tail and we have to set-up the second node.
    if ( _is_empty(list) ) {
        list->tail = node;
    } else {
        node->next->prev = node;
    }

    list->size++;

    return node;
}

dblnode_t* dbllist_push_to_end( dbllist_t* list, void* new_data ) {
    assert( new_data != NULL && DBLL_NEWNULL );

    dblnode_t *node = (dblnode_t*) mem_malloc( sizeof( dblnode_t ) );
    node->data = new_data;
    node->next = NULL;
    node->prev = list->tail;
    list->tail = node;

    // Set the head if the list is empty. Note that the head will not change
    // later if using push_to_end function. If the list is non-empty, then the
    // node will not be the head and we have to set-up the second last node.
    if ( _is_empty( list ) ) {
        list->head = node;
    } else {
        node->prev->next = node;
    }

    list->size++;

    return node;
}

void* dbllist_pop( dbllist_t *list ) {
    assert( !_is_empty(list) && DBLL_POPEMPTY );

    if ( !_is_empty(list) ) {
        dblnode_t *node = list->head;
        void *data = node->data;
        
        // If the last element is popped, then the list will have no more the
        // head and tail.
        if ( list->size == 1 ) {
            list->head = NULL;
            list->tail = NULL;
        } else {
            list->head = node->next;
            list->head->prev = NULL;
        }

        list->size--;

        // We release the node only; releasing the data is out of scope.
        mem_free(node);

        return data;
    } else {
        return NULL;
    }
}

void* dbllist_delete( dbllist_t *list, void *data ) {
    if ( !_is_empty(list) ) {
        dblnode_t *node = list->head;

        while( 1 ) {
            if ( node->data == data ) {
                break;
            }
            if ( node->next == NULL ) {
                return NULL; // Not found
            }
            node = node->next; // This goes on so the loop will end
        }

        // If the only element is removed, the list won't have a head or
        // tail
        if ( list->size == 1 ) {
            list->head = NULL;
            list->tail = NULL;
        } else if ( list->head == node ) { // The 1st one
            list->head = node->next;
            node->next->prev = node->prev;
        } else if ( node->next == NULL ) { // The last one
            node->prev->next = NULL;
            list->tail = node->prev;
        } else { // Basic case
            node->prev->next = node->next;
            node->next->prev = node->prev;
        }

        list->size--;

        // Release the memory
        mem_free(node);

        return data;
    } else {
        return NULL;
    }
};

int dbllist_is_empty( dbllist_t *list ) {
    return list->size == 0;
}

int dbllist_size( dbllist_t *list ) {
    return list->size;
}

dblnode_t* dbllist_head( dbllist_t *list ) {
    return list->head;
}

dblnode_t* dbllist_tail( dbllist_t *list ) {
    return list->tail;
}

void dbllist_remove( dbllist_t *list, void (*free)(void *) ) {
    dblnode_t *node = NULL;
    // Remove nodes from the head until the list is empty
    while( ( node = list->head ) != NULL ) {
#ifdef PRESERVE_CONSISTENCY
        // If the node is not the last one, update the link.
        if ( node->next ) {
            node->next->prev = NULL;
        }
#endif
        list->head = node->next;
        list->size--;
        if ( free != NULL ) {
            free( node->data );
        }
        mem_free( node );
    }
    // The list is now empty. Set the tail
    list->tail = NULL;
}
