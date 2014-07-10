#ifndef FILE_HOTLINE_HEADER
#define FILE_HOTLINE_HEADER
#ifndef FILE_FUNKY_HEADER
#include "funky.h"
#endif
#ifndef FILE_DYNOSAUR_HEADER
#include "dynosaur.h"
#endif
#ifndef FILE_BOOKER_HEADER
#include "booker.h"
#endif

int scan_line(int max_chars, char* buffer);
Thunk match_word(int word_len, char* subject);
Thunk parse_line(int num_chars, char* buffer);

#endif
