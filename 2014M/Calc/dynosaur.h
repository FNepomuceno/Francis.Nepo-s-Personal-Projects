#ifndef FILE_DYNOSAUR_HEADER
#define FILE_DYNOSAUR_HEADER
#ifndef FILE_CURRY_HEADER
#include "curry.h"
#endif

typedef struct {
	int _size;
	int _length;
	Thunk* _data;
} Thunk_ary, *Thunk_list;

Thunk_list new_lst();
int size(Thunk_list list);
void clean_lst(Thunk_list list);
Thunk* top (Thunk_list list);
void push(Thunk entry, Thunk_list list);
Thunk peek(Thunk_list list);
Thunk pop(Thunk_list list);
void move_to_list(Thunk entry, Thunk_list list);

#endif
