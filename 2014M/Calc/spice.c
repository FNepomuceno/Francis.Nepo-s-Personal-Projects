#include "booker.h"
#include "curry.h"
#include "dynosaur.h"
#include "funky.h"
#include "hotline.h"
#include "spoyder.h"

void string_test() {
	Str string = new_str();
	int length = set_line(string);
	char* line = to_string(string);
	printf("%s (%d)\n", line, length);
	if(is_prefix(line, "Hello")) {
		char* txt = line + strlen("Hello");
		printf("starts with:\nHello\n");
		printf("and ends with:\n%s\n", txt);
	} else {
		printf("does not start with\nHello\n");
	}
	clean_str(string);
}

void int_thunk_test() {
	int i = 1;
	Thunk int_thunk = int_to_thunk(i);
	int thunk_int = thunk_to_int(int_thunk);
	if(i == thunk_int) {
		printf("this works\n");
	}
	clean(int_thunk);
}

void plus_test() {
	int a = 2, b = 3;
	d_Func f_plus = new_func("plus", plus, ">I>II");
	Thunk t_plus = func_to_thunk(f_plus);
	Thunk v_two = int_to_thunk(a);
	Thunk v_three = int_to_thunk(b);
	Thunk t_plus_two = apply(t_plus, v_two);
	Thunk t_plus_two_2 = clone(t_plus_two);
	Thunk t_plus_two_three = apply(t_plus_two, v_three);
	Thunk t_plus_two_2_three = apply(t_plus_two_2, t_plus_two_three);
	Thunk v_plus_two_2_three = plus(t_plus_two_2_three);

	printf("%d + %d + %d = %d\n", a, a, b, thunk_to_int(v_plus_two_2_three));
	
	clean(t_plus_two_2_three);
}

void wrapper_loop() {
	Str string = new_str();
	int length;
	char* line;
	printf("Enter your stuff, or leave a blank line to exit\n");
	while(1) {
		Thunk expr = NULL;
		int val;

		printf("@ ");
		length = set_line(string);
		line = to_string(string);
		if(length == 1) break;

		expr = parse_line(length, line);
		if(expr != NULL) {
			val = thunk_to_int(expr);
			printf("answer is %d\n", val);
		}
	}
	clean_str(string);
}

int main() {
	wrapper_loop();
	return 0;
}

// So long, and thunks for all the curry
