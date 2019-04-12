// Doubly Linked List
//
// Linked list is a data structure consisting a sequence of nodes. Each node
// contains data and a reference to the next node in the sequence. The doubly
// linked list has also a reference pointing to the previous node.
//
// The head of the list is the first node. The tail of the list is the last node.
//
// The list operations are add, del, first, last and clr.
//
// (c) Tuomas Koskimies, 2018

#ifndef _dbllist_
#define _dbllist_

// Messages for the diagnostics
#define DBLL_NEWNULL "New node cannot be Null"
#define DBLL_POPEMPTY "Client pops en element from the empty list"
#define DBLL_RELEASENONEMPTYLIST "Releasing non-empty list"

// Return values
#define DBLL_SUCCESS		0
#define DBLL_LISTISEMPTY	-1
#define DBLL_NOTFOUND		-10

typedef struct Node {
    void *data;
    struct Node *next;
    struct Node *prev;
} Node;

// @todo: This should be moved to the .c file.
typedef struct DblLinkedList {
    int size;
    Node *head;
    Node *tail;
} DblLinkedList;

DblLinkedList* dbllist_new();

void dbllist_free( DblLinkedList* list );

// Inserts a new node at the beginning of the list
//
// @param list The pointer to the list where the node is pushed into
// @param new_data The data of the node
// @return The newly inserted node
Node* dbllist_push( DblLinkedList* list, void* new_data );

// Inserts a new node at the end of the list
//
// @param list The pointer to the list where the node is pushed into
// @param new_data The data of the node
// @return The newly inserted node
Node* dbllist_push_to_end( DblLinkedList* list, void* new_data );

// Pops the first node from the list
//
// @param list The pointer to the list where the node is popped from
// @return The pointer to the node, or NULL
void* dbllist_pop( DblLinkedList *list );

// Removes the node from the list
//
// @param list The pointer to the list where the node is popped from
// @param data The pointer to the data of the node. This defines the identity
//             of the node
// @return The pointer to the data of the node
void* dbllist_delete( DblLinkedList *list, void *data );

// @param list The pointer to the list
// @return Zero if the list is empty
int dbllist_is_empty( DblLinkedList *list );

// @param list The pointer to the list
// @return The size of the list
int dbllist_size( DblLinkedList *list );

// @param list The pointer to the list
// @return The pointer to the first element of the list
Node* dbllist_head( DblLinkedList *list );

// @param list The pointer to the list
// @return The pointer to the last element of the list
Node* dbllist_tail( DblLinkedList *list );

void dbllist_remove( DblLinkedList *list, void (*free)(void *) );

#endif // _dbllist_