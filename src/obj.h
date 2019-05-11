#ifndef _obj_
#define _obj_

typedef struct {
	int (*_eq)( void * );
} obj_t;

#endif // #ifndef _obj_
