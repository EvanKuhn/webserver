#include "std_string.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//==============================================================================
// Struct definition
//==============================================================================
struct string {
  char*  buf;    // Character buffer
  size_t size;   // Number of chars in buffer
  size_t cap;    // Size of buffer (excludes space for trailing \0 char)
};

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

void string_set(string* str, const char* other) {
  size_t other_len = strlen(other);
  // Both are empty. Do nothing.
  if(other_len == 0 && str->size == 0) {
    return;
  }
  // Capacity too small. Free buffer, then duplicate string.
  if(str->cap < other_len) {
    if(str->buf) free(str->buf);
    str->buf = strdup(other);
    str->size = other_len;
    str->cap = other_len;
  }
  // Capacity big enough. Copy string to buffer.
  else {
    memcpy(str->buf, other, other_len+1);
    str->size = other_len;
  }
}

void string_clear(string* str) {
  // Zero-out the buffer
  if(str->buf) {
    bzero(str->buf, str->cap+1);
    str->size = 0;
  }
}

void string_release(string* str) {
  // Free the buffer entirely
  if(str->buf) free(str->buf);
  str->buf = 0;
  str->size = 0;
  str->cap = 0;
}

void string_reserve(string* str, size_t target) {
  // Figure out if we need to grow or shrink the capacity
  const bool need_to_grow   = (str->cap < target && target > 0);
  const bool need_to_shrink = (str->cap > target && str->size <= target);

  if(need_to_grow || need_to_shrink) {
    // Figure out the new capacity
    size_t newcap = target;
    if(need_to_grow) {
      newcap = str->cap * 2;
      if(newcap < target) newcap = target;
    }

    // Allocate new memory and copy the existing string
    char* newbuf = malloc(newcap + 1);
    bzero(newbuf, newcap + 1);
    memcpy(newbuf, str->buf, str->size);

    // Replace the old string with the new one
    free(str->buf);
    str->buf = newbuf;
    str->cap = newcap;
  }
}

void string_resize(string* str, size_t size) {
  // If growing size, add spaces
  if(str->size < size) {
    string_reserve(str, size);
    for(size_t i=str->size; i<size; ++i) {
      str->buf[i] = ' ';
    }
  }
  // If shrinking capacity, set 0's
  else if(str->size > size) {
    bzero(str->buf + size, str->size - size);
  }

  // Finally, set new size
  str->size = size;
}

const char* string_cstr(const string* str) {
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
  if(a == b)             return true;   // Same string object
  if(a->size != b->size) return false;  // Diff size
  if(!a->buf && !b->buf) return true;   // Both empty
  return strcmp(a->buf, b->buf) == 0;   // Compare characters
}

bool string_equal_cstr(const string* a, const char* b) {
  return strcmp(string_cstr(a), b) == 0;
}

// TODO - what if we append a null-terminator? What to do?

void string_append_char(string* str, char c) {
  if(!c) return;
  string_append_cstrn(str, &c, 1);
}

void string_append_cstr(string* str, const char* cstr) {
  string_append_cstrn(str, cstr, strlen(cstr));
}

void string_append_cstrn(string* str, const char* cstr, size_t len) {
  if(len == 0) return;
  const size_t empty_space = (str->cap - str->size);
  // Check the string for null terminators in the first 'len' chars
  len = strnlen(cstr, len);
  // Make sure there is space in the string's buffer
  if(empty_space < len) {
    string_reserve(str, len-empty_space);
  }
  // Append the chars and add a null terminator
  strncpy(str->buf + str->size, cstr, len);
  str->size += len;
  str->buf[str->size] = 0;
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
  if(i == 0) return;
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
