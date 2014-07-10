#include "curry.h"

Thunk new_thunk(Thunk prev, void* param,
		Thunk (*func)(Thunk), char* type) {	
	Thunk new_thunk = (Thunk) malloc(sizeof(d_Thunk));
	new_thunk -> prev_args = prev; //TODO: Null check above
	new_thunk -> input = param;
	new_thunk -> func = func;
	new_thunk -> type = type;
	return new_thunk;
}

Thunk nil(Thunk e) {
	printf("ERROR: FUNCTION NOT INITIALIZED\n");
	return e;
}

void clear(Thunk t) {
	if(t == NULL) return;
	if(t -> prev_args == NULL) {
		if(t -> input != NULL) {
			free(t -> input);
			t -> input = NULL;
		}
	} else {
		Thunk old_prev = t->prev_args;
		Thunk old_input = (Thunk)(t -> input);
		t -> prev_args = NULL;
		t -> input = NULL;
		clean(old_prev);
		if(old_input != NULL) {
			clean(old_input);
		}
	}
}

void clean(Thunk t) {
	clear(t);
	free(t);
}

Thunk eval(Thunk t) {
	Thunk cur = t;
	while(cur -> prev_args != NULL) {
		Thunk prev = cur;

		cur = (cur -> func)(cur);
		clean(prev);
	}
	return cur;
}

int thunk_to_int(Thunk val) {
	Thunk cur;
	int result;

	if(strcmp(val -> type, "I")) {
		printf("ERROR: THUNK IS NOT AN INT\n");
		return 0;
	}
	
	cur = eval(val);
	result = *(int *)(cur -> input);
	clean(cur);
	return result;
}

Thunk int_to_thunk(int val) {
	int* p_val = (int*) malloc(sizeof(int)); //TODO: Null check here
	(*p_val) = val;
	return new_thunk(NULL, p_val, nil, "I");
}

// Because bool doesn't exist. Thunks
int is_prefix(char* subject, char* target) {
	char* sub = subject;
	char* tar = target;
	while(*tar && *sub && *(sub) == *(tar)) {
		sub++;
		tar++;
	}
	return *tar == '\0';
}

// Returns applied thunk or Null
Thunk apply(Thunk func, Thunk param) {
	char* func_type = (func -> type) + 1;
	char* param_type = param -> type;
	char* new_type;
	if(!is_prefix(func_type, param_type)) {
		return NULL;
	}
	new_type = func_type + strlen(param_type);
	return new_thunk(func, param, func -> func, new_type);
}

// Creates a new copy of the thunk, but with new copies
// of input and prev_arg
Thunk clone(Thunk x) {
	Thunk new_args = x -> prev_args;
	void* new_input = x -> input;

	if(new_args != NULL) {
		new_args = clone(new_args);
		new_input = clone((Thunk)new_input);
	} else if(new_input != NULL) {
		//TODO: Null check below
		//TODO: And extend this to arbitrary types
		void* new_loc = malloc(sizeof(int));
		memcpy(new_loc, new_input, sizeof(int));
		new_input = new_loc;
	}
	return new_thunk(new_args, new_input,
			x -> func, x -> type);
}
