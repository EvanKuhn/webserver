//==============================================================================
// Miscellaneous utility functions
//
// Evan Kuhn, 2012-09-17
//==============================================================================
#ifndef UTILS_H
#define UTILS_H

#include <sys/types.h>

// Return the input string, or "" if input is null
const char* safe_cstr(const char* s);

// Trim surrounding whitespace from the string.
// - Adds a new null-separator to remove trailing whitespace.
// - Returns a pointer to the first character after leading whitespace.
// - Returns NULL if input is NULL.
char* trim(char* str);

// A very simple string buffer
typedef struct StringBuffer {
  char* chars;  // Character buffer
  size_t size;  // Size of the string stored in the buffer
  size_t cap;   // Size of the character buffer
} StringBuffer;

void string_buffer_init(StringBuffer* buf, size_t capacity);
void string_buffer_free(StringBuffer* buf);
void string_buffer_append(StringBuffer* buf, const char* str);

#endif // UTILS_H