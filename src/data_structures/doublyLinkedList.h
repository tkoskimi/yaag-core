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

typedef struct dblnode_t {
    void *data;
    struct dblnode_t *next;
    struct dblnode_t *prev;
} dblnode_t;

// @todo: This should be moved to the .c file.
typedef struct dbllist_t {
    int size;
    dblnode_t *head;
    dblnode_t *tail;
} dbllist_t;

dbllist_t* dbllist_new();

void dbllist_free( dbllist_t* list );

// Adds a source list (src) to the end of the destination list (dst)
//
// @param dst The pointer to the destination list
// @param src The pointer to the source list
// @return The destination list
dbllist_t* dbllist_append( dbllist_t* dst, dbllist_t* src );

// Inserts a new node at the beginning of the list
//
// @param list The pointer to the list where the node is pushed into
// @param new_data The data of the node
// @return The newly inserted node
dblnode_t* dbllist_push( dbllist_t* list, void* new_data );

// Inserts a new node at the end of the list
//
// @param list The pointer to the list where the node is pushed into
// @param new_data The data of the node
// @return The newly inserted node
dblnode_t* dbllist_push_to_end( dbllist_t* list, void* new_data );

// Pops the first node from the list
//
// @param list The pointer to the list where the node is popped from
// @return The pointer to the node, or NULL
void* dbllist_pop( dbllist_t *list );

// Removes the node from the list
//
// @param list The pointer to the list where the node is popped from
// @param data The pointer to the data of the node. This defines the identity
//             of the node
// @return The pointer to the data of the node
void* dbllist_delete( dbllist_t *list, void *data );

// @param list The pointer to the list
// @return Zero if the list is empty
int dbllist_is_empty( dbllist_t *list );

// @param list The pointer to the list
// @return The size of the list
int dbllist_size( dbllist_t *list );

// @param list The pointer to the list
// @return The pointer to the first element of the list
dblnode_t* dbllist_head( dbllist_t *list );

// @param list The pointer to the list
// @return The pointer to the last element of the list
dblnode_t* dbllist_tail( dbllist_t *list );

void dbllist_remove( dbllist_t *list, void (*free)(void *) );

#endif // _dbllist_