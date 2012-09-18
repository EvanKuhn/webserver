//==============================================================================
// Miscellaneous utility functions
//
// Evan Kuhn, 2012-09-17
//==============================================================================
#ifndef UTILS_H
#define UTILS_H

// TODO - write tests

// Return the input string, or "" if input is null
const char* safe_cstr(const char* s);

// A version of strsep() that also trims each token of surrounding whitespace.
// See 'man strsep' for usage details.
char* strsep_trim(char** str, const char* delims);

#endif // UTILS_H