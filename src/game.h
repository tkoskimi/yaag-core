// @author Tuomas Koskimies
#ifndef _game_
#define _game_

#include "./data_structures/doublyLinkedList.h"

typedef struct {
    int type;
    int x;
    int y;
    int w;
    int h;
    int v;
    dbllist_t *events;
    int (*update)( int dt );
    void *data;
} GameObject;

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
