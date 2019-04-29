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

DblLinkedList* dbllist_new() {
    DblLinkedList *list = (DblLinkedList *) mem_malloc( sizeof( DblLinkedList ) ); 
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
    return list;
}

void dbllist_free( DblLinkedList* list ) {
    assert ( _is_empty(list) && DBLL_RELEASENONEMPTYLIST );
    mem_free(list);
}

DblLinkedList* dbllist_append( DblLinkedList* dst, DblLinkedList* src ) {
    if ( src != NULL && !_is_empty( src ) ) {
        Node *node = src->head;
        while ( node != NULL ) {
            dbllist_push_to_end( dst, node->data );
            node = node->next;
        }
    }
    return dst;
}

Node* dbllist_push( DblLinkedList* list, void* new_data ) {
    assert( new_data != NULL && DBLL_NEWNULL );

    Node *node = (Node*) mem_malloc( sizeof( Node ) );
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

Node* dbllist_push_to_end( DblLinkedList* list, void* new_data ) {
    assert( new_data != NULL && DBLL_NEWNULL );

    Node *node = (Node*) mem_malloc( sizeof( Node ) );
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

void* dbllist_pop( DblLinkedList *list ) {
    assert( !_is_empty(list) && DBLL_POPEMPTY );

    if ( !_is_empty(list) ) {
        Node *node = list->head;
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

void* dbllist_delete( DblLinkedList *list, void *data ) {
    if ( !_is_empty(list) ) {
        Node *node = list->head;

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

int dbllist_is_empty( DblLinkedList *list ) {
    return list->size == 0;
}

int dbllist_size( DblLinkedList *list ) {
    return list->size;
}

Node* dbllist_head( DblLinkedList *list ) {
    return list->head;
}

Node* dbllist_tail( DblLinkedList *list ) {
    return list->tail;
}

void dbllist_remove( DblLinkedList *list, void (*free)(void *) ) {
    Node *node = NULL;
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
