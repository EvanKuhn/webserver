//==============================================================================
// String class, based heavily on C++ std::string
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
void string_set     (string* str, const char* to); // Won't shrink capacity
void string_clear   (string* str);                 // Set size to 0, leave capacity
void string_release (string* str);                 // Set size and capacity to 0
void string_reserve (string* str, size_t size);    // Ensure capacity >= size
void string_resize  (string* str, size_t size);    // Won't shrink capacity. Adds spaces.

// Getters
// - Size is the length of the string, in characters.
// - Capacity is the total internal space available. Grows as needed.
const char*  string_cstr     (const string* str);
size_t       string_size     (const string* str);
size_t       string_capacity (const string* str);
bool         string_empty    (const string* str);

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
void string_trim     (string* str);
void string_ltrim    (string* str);
void string_rtrim    (string* str);
void string_upcase   (string* str);
void string_downcase (string* str);

#endif // STRING_H
