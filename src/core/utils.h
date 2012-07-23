//==============================================================================
// Miscellaneous utility functions
//
// Evan Kuhn, 2012-02-28
//==============================================================================
#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stdio.h>

// Read the next char, word or line from the stream
char read_char(FILE* stream);
bool read_word(FILE* stream, char* buf, size_t bufsize);
bool read_line(FILE* stream, char* buf, size_t bufsize);

// Convert the input string to upper or lower case
void upcase  (char* str);
void downcase(char* str);

#endif UTILS_H
