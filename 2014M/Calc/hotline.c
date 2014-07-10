#include "hotline.h"

// buffer better at least max_chars + 1 spots
int scan_line(int max_chars, char* buffer) {
	int index = 0;
	for(index = 0; index < max_chars; index++) {
		char temp;
		scanf("%c", &temp);
		if(temp == '\n') {
			temp = '\0';
		}
		*(buffer + index) = temp;
		if(temp == '\0') {
			return index;
		}
	}
	*(buffer + index) = '\0';
	return index;
}

Thunk match_word(int word_len, char* subject) {
	int length = 5, index = 0;
	d_Func f_ary[length];
	f_ary[ 0] = new_func("+",  plus,   ">I>II");
	f_ary[ 1] = new_func(">", again, ">>II>II");
	f_ary[ 2] = new_func("-", minus,   ">I>II");
	f_ary[ 3] = new_func("*", times,   ">I>II");
	f_ary[ 4] = new_func("/",  divs,   ">I>II");

	if(word_len <= 0) return NULL;
	
	for(; index < length; index++) {
		if(strlen(f_ary[index].name) == word_len &&
				is_prefix(subject, f_ary[index].name)) {
			return func_to_thunk(f_ary[index]);
		}
	}
	if(*subject - '0' >= 0 && *subject - '0' < 10) {
		int num = atoi(subject);
		return int_to_thunk(num);
	} else {
		return NULL;
	}
}

Thunk parse_line(int num_chars, char* buffer) {
	int index = 0;
	char* word = buffer;
	int start_index = 0, end_index = 0, len = -1;
	Thunk last = NULL;
	Thunk_list list = new_lst(); //TODO: Null check this

	for(; index < num_chars &&
			*(buffer + index) != 0; index++) {
		if(*(buffer + index) == ' ') {
			end_index = index;
			len = end_index - start_index;
			
			move_to_list(match_word(len, word), list);
			
			start_index = index + 1;
			word = buffer + start_index;
		}
	}
	len = index - start_index;
	move_to_list(match_word(len, word), list);
	if(size(list) == 1 && *(peek(list)->type) != '>') {
		last = pop(list);
	} else {
		printf("Line is invalid\n");
	}
	clean_lst(list);
	return last;
}
