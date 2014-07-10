#include "spoyder.h"

Str new_str() {
	int init_size = 128;
	char* line = malloc(init_size * sizeof(char));
	Str string = malloc(sizeof(d_Str));
	
	if(line == NULL || string == NULL) {
		free(line);
		free(string);
		return NULL;
	}
	
	string->_size = init_size;
	string->_line = line;
	return string;
}

void double_len(Str string) {
	char* old_str, * new_str;
	int index = 0, old_size;
	
	if(string == NULL || string->_line == NULL) return;
	old_str = string->_line;
	old_size = string->_size;
	new_str = malloc(2 * old_size * sizeof(char));
	
	if(new_str == NULL) return;
	for(; index < old_size; index++) {
		*(new_str + index) = *(old_str + index);
	}
	
	free(old_str);
	string->_line = new_str;
}

void clean_str(Str string) {
	if(string == NULL) return;
	free(string->_line);
	free(string);
}

char* to_string(Str string) {
	if(string == NULL) return NULL;
	return string->_line;
}

//For initial input and takes an arbitrary
//length until a '\n' newline char appears
//Returns the length of the line
int set_line(Str string) {
	int size = string->_size, index = 0;
	char temp;
	char* line = string->_line;
	do {
		scanf("%c", &temp);
		if(temp == '\n') {
			temp = '\0';
		}
		if(index == size) {
			double_len(string);
			size = string->_size;
			line = string->_line;
		}
		*(line + index) = temp;
		index++;
	} while(temp != '\0');
	return index;
}
