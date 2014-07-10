#ifndef FILE_SPOYDER_HEADER
#define FILE_SPOYDER_HEADER
#ifndef FILE_CURRY_HEADER
#include <stdio.h>
#include <stdlib.h>
#endif

typedef struct {
	int _size;
	char* _line;
} d_Str, *Str;

Str new_str();
void double_len(Str string);
void clean_str(Str string);
char* to_string(Str string);
int set_line(Str string);

#endif
