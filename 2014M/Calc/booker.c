#include "booker.h"

Thunk plus(Thunk b) {
	int val_a, val_b;
	Thunk t_a, t_b;
	if(b -> func != plus || strcmp(b -> type, "I") != 0) {
		printf("ERROR: INCOMPATIBLE THUNK (PLUS)\n");
		return NULL;
	}
	
	t_a = b -> prev_args;
	t_b = b;
	
	val_a = thunk_to_int(t_a -> input);
	t_a -> input = NULL;
	val_b = thunk_to_int(t_b -> input);
	b -> input = NULL;
	return int_to_thunk(val_a + val_b);
}

Thunk minus(Thunk b) {
	int val_a, val_b;
	Thunk t_a, t_b;
	if(b -> func != minus|| strcmp(b -> type, "I") != 0) {
		printf("ERROR: INCOMPATIBLE THUNK (MINUS)\n");
		return NULL;
	}
	
	t_a = b -> prev_args;
	t_b = b;
	
	val_a = thunk_to_int(t_a -> input);
	t_a -> input = NULL;
	val_b = thunk_to_int(t_b -> input);
	b -> input = NULL;
	return int_to_thunk(val_a - val_b);
}

Thunk times(Thunk b) {
	int val_a, val_b;
	Thunk t_a, t_b;
	if(b -> func != times || strcmp(b -> type, "I") != 0) {
		printf("ERROR: INCOMPATIBLE THUNK (TIMES)\n");
		return NULL;
	}
	
	t_a = b -> prev_args;
	t_b = b;
	
	val_a = thunk_to_int(t_a -> input);
	t_a -> input = NULL;
	val_b = thunk_to_int(t_b -> input);
	b -> input = NULL;
	return int_to_thunk(val_a * val_b);
}

Thunk divs(Thunk b) {
	int val_a, val_b;
	Thunk t_a, t_b;
	if(b -> func != divs || strcmp(b -> type, "I") != 0) {
		printf("ERROR: INCOMPATIBLE THUNK (DIVS)\n");
		return NULL;
	}
	
	t_a = b -> prev_args;
	t_b = b;
	
	val_a = thunk_to_int(t_a -> input);
	t_a -> input = NULL;
	val_b = thunk_to_int(t_b -> input);
	b -> input = NULL;
	return int_to_thunk(val_a / val_b);
}

Thunk again(Thunk b) {
	Thunk t_func, t_func_1, t_arg;
	if(b -> func != again || strcmp(b -> type, "I") != 0) {
		printf("ERROR: INCOMPATIBLE THUNK (AGAIN)\n");
		return NULL;
	}
	
	t_arg = clone((Thunk) b -> input);
	t_func = clone((Thunk)(b -> prev_args -> input));
	t_func_1 = apply(clone(t_func), t_arg);
	t_func = apply(t_func, t_func_1);

	return t_func;
}
