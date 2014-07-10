#ifndef FILE_CURRY_HEADER
#define FILE_CURRY_HEADER
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct curry {
	struct curry* prev_args;
	void* input;	// usually a Thunk, but can
					//be a pointer to a primitive
	struct curry* (*func)(struct curry*);
					// change this to int* *func maybe?
	char* type;
} d_Thunk, *Thunk;

Thunk new_thunk(Thunk prev, void* param,
		Thunk (*func)(Thunk), char* type);
Thunk nil(Thunk e);
void clean(Thunk t);
void clear(Thunk t);
Thunk eval(Thunk t);
int thunk_to_int(Thunk val);
Thunk int_to_thunk(int val);
int is_prefix(char* subject, char* target);
Thunk apply(Thunk func, Thunk param);
Thunk clone(Thunk x);

#endif
