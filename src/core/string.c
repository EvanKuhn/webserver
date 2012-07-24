#include "string.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//==============================================================================
// Struct definition and private function declarations
//==============================================================================
struct string {
  char*  buf;    // Character buffer
  size_t size;   // Number of chars in buffer
  size_t cap;    // Size of buffer (excludes space for trailing \0 char)
};

// Increase the string's buffer size by at least 'len' chars
// TODO: make this set the capacity to at least 'len', rather than adding 'len' chars
void string_grow_capacity(string* str, size_t len);

//==============================================================================
// Public functions
//==============================================================================
string* string_new() {
  string* str = malloc(sizeof(string));
  str->buf  = NULL;
  str->size = 0;
  str->cap  = 0;
  return str;
}

void string_free(string* str) {
  if(str->buf) free(str->buf);
  free(str);
}

void string_set(string* str, char* other) {
  size_t other_len = strlen(other);
  if(other_len == 0 && str->size == 0) {
    return;
  }
  if(str->cap < other_len) {
    if(str->buf) free(str->buf);
    str->cap = (sizeof(char) * other_len);
    str->buf = malloc(str->cap + 1);
  }
  memcpy(str->buf, other, other_len);
  memset(str->buf + other_len, '\0', (str->cap + 1) - other_len);
  str->size = other_len;
}

void string_clear(string* str) {
  if(str->buf) memset(str->buf, '\0', str->cap + 1);
  str->size = 0;
}

void string_release (string* str) {
  if(str->buf) free(str->buf);
  str->size = 0;
  str->cap = 0;
}

void string_resize(string* str, size_t size) {
  // Not enough capacity. Grow.
  if(str->cap < size) {
    string_grow_capacity(str, size);
  }
  // String too small. Add characters.
  if(str->size < size) {
    memset(str->buf + str->size, ' ', size - str->size);
    str->buf[size] = '\0';
  }
  // String too large. Add null chars.
  else if(str->size > size) {
    memset(str->buf + size, '\0', str->size - size);
  }
  str->size = size;
}

char* string_cstr(const string* str) {
  return (str->buf ? str->buf : "");
}

size_t string_size(const string* str) {
  return str->size;
}

size_t string_capacity(const string* str) {
  return str->cap;
}

bool string_empty(const string* str) {
  return (str->size == 0);
}

bool string_equal(const string* a, const string* b) {
  if(a->size != b->size) return false;  // Diff size
  if(!a->buf && !b->buf) return true;   // Both empty
  return strcmp(a->buf, b->buf) == 0;   // Compare characters
}

bool string_equal_cstr(const string* a, const char* b) {
  return strcmp(string_cstr(a), b) == 0;
}

void string_append_char(string* str, char c) {
  string_append_cstrn(str, &c, 1);
}

void string_append_cstr(string* str, const char* cstr) {
  string_append_cstrn(str, cstr, strlen(cstr));
}

void string_append_cstrn(string* str, const char* cstr, size_t len) {
  const size_t empty_space = (str->cap - str->size);
  // Make sure there is space in the string's buffer
  if(empty_space < len) {
    string_grow_capacity(str, len-empty_space);
  }
  // Append the chars. No need to set trailing \0 char.
  memcpy(str->buf + str->size, cstr, len);
  str->size += len;
}

void string_append_str(string* str, string* other) {
  string_append_cstrn(str, other->buf, other->size);
}

void string_trim(string* str) {
  string_ltrim(str);
  string_rtrim(str);
}

void string_ltrim(string* str) {
  // Loop until we hit a non-whitespace char
  size_t i = 0;
  for( ; i<str->size; ++i) {
    if(!isspace(str->buf[i])) break;
  }
  // Shift the string down, over the whitespace, then pad the rest with \0
  // - 'i' equals the number of whitespace characters up front
  size_t newsize = str->size - i;
  if(newsize > 0) {
    memcpy(str->buf, str->buf + i, newsize);
  }
  memset(str->buf + newsize, '\0', str->cap - newsize);
  str->size = newsize;
}

void string_rtrim(string* str) {
  // Loop until we hit a non-whitespace char
  size_t i = 0;
  for(i=str->size; i>0; --i) {
    if(!isspace(str->buf[i-1])) break;
  }
  // 'i' is the new size
  string_resize(str, i);
}

void string_upcase(string* str) {
  const char offset = 'A' - 'a';
  for(size_t i=0; i<str->size; ++i) {
    char c = str->buf[i];
    if(c >= 'a' && c <= 'z') str->buf[i] += offset;
  }
}

void string_downcase(string* str) {
  const char offset = 'a' - 'A';
  for(size_t i=0; i<str->size; ++i) {
    char c = str->buf[i];
    if(c >= 'A' && c <= 'Z') str->buf[i] += offset;
  }
}

//==============================================================================
// Private functions
//==============================================================================
// Increase the string's buffer size by at least 'len' chars
void string_grow_capacity(string* str, size_t len) {
  // Initialize the buffer if NULL
  if(!str->buf) {
    str->buf = malloc(sizeof(char) * (len + 1));
    str->cap = len;
    memset(str->buf, '\0', len + 1);
  }
  // Otherwise resize and copy
  else {
    // Calculate the new size by doubling the current size
    size_t newsize = str->size + len;
    if(newsize < str->size * 2) newsize = str->size * 2;

    // Allocate and copy
    size_t oldsize = str->size;
    char*  oldbuf  = str->buf;
    char*  newbuf  = malloc(sizeof(char) * (newsize + 1));

    memcpy(newbuf, oldbuf, oldsize);
    memset(newbuf + oldsize, '\0', newsize - oldsize);
    free(oldbuf);

    str->buf = newbuf;
    str->cap = newsize;
  }
}
