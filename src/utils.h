//==============================================================================
// Miscellaneous utility functions
//
// Evan Kuhn, 2012-09-17
//==============================================================================
#ifndef UTILS_H
#define UTILS_H

typedef char timebuf_t[22];

// Return the input string, or "" if input is null
const char* safe_cstr(const char* s);

// Trim surrounding whitespace from the string.
// - Adds a new null-separator to remove trailing whitespace.
// - Returns a pointer to the first character after leading whitespace.
// - Returns NULL if input is NULL.
char* trim(char* str);

// Construct and return a timestamp string.
// - Fills the 'buffer' param with the string.
// - Returns a pointer to the first character in the input buffer.
const char* timestamp(timebuf_t buffer);

#endif // UTILS_H