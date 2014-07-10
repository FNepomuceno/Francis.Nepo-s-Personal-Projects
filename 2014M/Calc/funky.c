#include "funky.h"

d_Func new_func(char* new_name,
		Thunk (*new_func)(Thunk), char* new_type) {
	d_Func funky;
	funky.name = new_name;
	funky.func = new_func;
	funky.type = new_type;
	return funky;
}

Thunk func_to_thunk(d_Func funky) {
	return new_thunk(NULL, NULL, funky.func, funky.type);
}
