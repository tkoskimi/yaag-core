// @author Tuomas Koskimies
#ifndef _physics_
#define _physics_

#include "./data_structures/doublyLinkedList.h"
#include "./data_structures/quad_tree.h"

typedef struct {
    int guid;
    int type;
    dbllist_t* objs;
} physics_obj_t;

// 
typedef struct {
    // Geometry
    unsigned int w;
    unsigned int h;
    // Mechanics
    int x;
    int y;
    int vx;
    int vy;
    int ax;
    int ay;
    int Lx;
    int Ly;
    unsigned int m;
} physics_body_t;

typedef struct {

} physics_collider_2D_t;

qtree_t* physics_construct_bsp( tnode_t* root );
qtree_t* physics_update_bsp();
void physics_check_collisions( tnode_t* root, dbllist_t* lst );
int physics_check_two_bodies( physics_obj_t* obj_0, physics_obj_t* obj_1 );

#endif // _physics_