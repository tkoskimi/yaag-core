#ifdef NONE
void qtree_dft( tnode_t* root, dbllist_t* lst ) {
    if ( !root ) {
        return;
    }
    // Simple case. No children.
    if ( !root->children ) {
        // Check the collisions.
        dbllist_t *objs = ( dbllist_t* ) root->data;
        dblnode_t *node = dbllist_head( objs );
        // 
        while ( node->next ) {
            // This is wrong. It breaks something, but what? This is the implementation of the q-tree
            // and here we should just do q-tree related things. To test that read the program
            // and if you use some terms that are outside of the scope of the q-tree, something is wrong.
            // One can generalize this by using call-backs but they decrease readability of the code.
            // Tässä on se ongelma, että q-puun sisäistä rakennetta on sen sisäiset rakenteet, eikä ulkopuolisten pitäisi päästä
            // niihin kuin abstraktioiden kautta. Esim. get_children, get_
            if ( ( game_obj_t* )( node->data )->collide( ( game_obj_t* )( node->next->data ) ) ) {
                // Yes they do.
            }
        }
        return dbllist_append( dbllist_new(), ( dbllist_t* ) root->data );
    } else {
        dbllist_t* abc = dbllist_new();
        dblnode_t* child = dbllist_head( root->children );
        while( child ) {
            ( dbllist_t* ) lst = qtree_dft( child->data, NULL );
            dbllist_append( abc, lst );
            dbllist_remove( lst, NULL ); // Remove the child's list.
            child = child->next;
        }
        dbllist_append( ( dbllist_t* ) root->data, abc );
        dbllist_remove( abc, NULL );
    }
    
}
#endif

#ifdef DEBUG
unsigned int _physics_curr_step = 0;

// Returns the current step (>0) of the simulation
//
// @precondition None
// @postcondition None
// @return The current step of the simulation
unsigned int physics_current_step() {
    return _physics_curr_step;
}

void physics_next_step( unsigned int step_count ) {}
void physics_prev_step( unsigned int step_count ) {}
unsigned int physics_add_rigid_body() {}
void physics_del_rigid_body() {}
void physics_print_bsp() {}
void physics_print_state() {}
void physics_print_obj( int guid ) {

}
#endif // #ifdef DEBUG
