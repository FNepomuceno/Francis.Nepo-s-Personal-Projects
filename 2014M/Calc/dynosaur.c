#include "dynosaur.h"

Thunk_list new_lst() {
	int size = 10;
	Thunk* data = malloc(size * sizeof(Thunk));
	Thunk_list list = malloc(sizeof(Thunk_ary));

	if(list == NULL) return NULL;
	list -> _size = size;
	list -> _length = 0;
	list -> _data = data;
	return list;
}

int size(Thunk_list list) {
	if(list == NULL) return -1;
	return (list -> _length);
}

void clean_lst(Thunk_list list) {
	if(list == NULL) {
		return;
	}
	if(list -> _data != NULL) {
		int index = 0, len = size(list);
		
		for(; index < len; index++) {
			clean(*((list -> _data) + index));
		}
		
		free(list -> _data);
	}
	free(list);
}

Thunk* top (Thunk_list list) {
	if(list == NULL) return NULL;
	return ((list -> _data) + size(list));
}

void push(Thunk entry, Thunk_list list) {
	if(list == NULL) {
		return;
	}
	if(list -> _length == list -> _size) {
		int old_size = list -> _size;
		int new_size = 2 * old_size;
		int index = 0;
		Thunk* old_list = list -> _data;
		Thunk* new_list = malloc(new_size * sizeof(Thunk));

		if(new_list == NULL) return;
		for(; index < old_size; index++) {
			*(new_list + index) = *(old_list + index);
		}
		
		list -> _data = new_list;
		list -> _size = new_size;
		free(old_list);
	}
	*top(list) = entry;
	(list -> _length)++;
}

Thunk peek(Thunk_list list) {
	if(list == NULL) return NULL;
	return *(top(list)-1);
}

Thunk pop(Thunk_list list) {
	Thunk entry;
	if(list == NULL || list -> _length == 0) return NULL;
	entry = peek(list);
	(list -> _length)--;
	return entry;
}

void move_to_list(Thunk entry, Thunk_list list) {
	if(entry == NULL || list == NULL) return;
	Thunk func, entr = entry, app;
	while(size(list) > 0) {
		func = pop(list);
		app = apply(func, entr);
		if(app != NULL) {
			entr = app;
		} else {
			push(func, list);
			break;
		}
	}
	push(entr, list);
}
