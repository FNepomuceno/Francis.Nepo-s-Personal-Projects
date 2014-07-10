#ifndef FILE_FUNKY_HEADER
#define FILE_FUNKY_HEADER
#ifndef FILE_CURRY_HEADER
#include "curry.h"
#endif

typedef struct function {
	char* name;
	Thunk (*func)(Thunk);
	char* type;
} d_Func, *p_Func;

d_Func new_func(char* new_name,
	Thunk (*new_func)(Thunk), char* new_type);
Thunk func_to_thunk(d_Func funky);

#endif
