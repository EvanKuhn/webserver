//==============================================================================
// String class
//
// Evan Kuhn, 2012-02-25
//==============================================================================
#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <stdbool.h>

typedef struct string string;

// Allocate or free a string
string* string_new  ();
void    string_free (string* str);

// Setters
// - Check comments to see how these affect the internal storage capacity
void string_set     (string* str, char* other); // Won't shrink capacity
void string_clear   (string* str);              // Set size to 0, leave capacity
void string_release (string* str);              // Set size and capacity to 0
void string_resize  (string* str, size_t size); // Won't shrink capacity. Adds spaces.

// Getters
// - Size is the length of the string, in characters.
// - Capacity is the total internal space available. Grows as needed.
char*  string_cstr     (const string* str);
size_t string_size     (const string* str);
size_t string_capacity (const string* str);
bool   string_empty    (const string* str);

// Comparison
bool string_equal      (const string* a, const string* b);
bool string_equal_cstr (const string* a, const char* b);

// Append to a string
// - string_append_cstrn() appends 'len' chars from 'cstr' to 'str'
void string_append_char  (string* str, char c);
void string_append_cstr  (string* str, const char* cstr);
void string_append_cstrn (string* str, const char* cstr, size_t len);
void string_append_str   (string* str, string* other);

// Alterations
void string_trim     (string* str);  // TODO
void string_ltrim    (string* str);  // TODO
void string_rtrim    (string* str);  // TODO
void string_upcase   (string* str);  // TODO
void string_downcase (string* str);  // TODO

#endif // STRING_H
