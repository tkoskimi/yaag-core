// @author Tuomas Koskimies
#ifndef _game_
#define _game_

#include "obj.h"
#include "./data_structures/doublyLinkedList.h"

typedef struct game_obj_t {
    struct obj_t* obj;
    int type;
    int x;
    int y;
    int w;
    int h;
    int v;
    dbllist_t *events;
    int (*update)( int dt );
    void *data;
} game_obj_t;

// Initializes this game
//
// @return The end result of the initialization
int init();

// Updates the game objects
//
// @param dt The time delta
// @return The end result of the loop
int loop( int dt );

#endif // #ifndef _game_
