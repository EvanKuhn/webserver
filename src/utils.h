//==============================================================================
// Miscellaneous utility functions
//
// Evan Kuhn, 2012-09-17
//==============================================================================
#ifndef UTILS_H
#define UTILS_H

// Return the input string, or "" if input is null
const char* safe_cstr(const char* s);

// Trim surrounding whitespace from the string.
// - Adds a new null-separator to remove trailing whitespace.
// - Returns a pointer to the first character after leading whitespace.
// - Returns NULL if input is NULL.
char* trim(char* str);

#endif // UTILS_H