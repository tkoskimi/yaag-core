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

struct Node {
    void *data;
    struct Node *next;
    struct Node *prev;
};

int add(struct Node** head, void* new_data, size_t new_data_size);

int del(struct Node** head, struct Node* del_data);

struct Node* first(struct Node** head);

struct Node* last(struct Node** head);

int clr(struct Node** head);