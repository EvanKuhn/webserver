#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

const char* safe_cstr(const char* s) {
  return (s ? s : "");
}

char* trim(char* str) {
  // Check for null
  if(!str) return NULL;

  // Trim trailing whitespace by adding null terminator
  const size_t len = strlen(str);
  for(size_t i=len; i>0; --i) {
    if(!isspace(str[i-1])) {
      str[i] = 0;
      break;
    }
  }

  // Advance 'str' to trim leading whitespace
  while(*str && isspace(*str)) {
    ++str;
  }
  return str;
}

void string_buffer_init(StringBuffer* buf, size_t capacity) {
  if(capacity == 0) capacity = 1;
  buf->chars = malloc(capacity);
  buf->size = 0;
  buf->cap = capacity;
  bzero(buf->chars, capacity);
}

void string_buffer_free(StringBuffer* buf) {
  if(buf->chars) free(buf->chars);
  buf->chars = 0;
  buf->size = 0;
  buf->cap = 0;
}

void string_buffer_append(StringBuffer* buf, const char* str) {
  const size_t length = strlen(str);
  const size_t mincap = buf->size + length + 1;
  // Reallocate if needed
  if(buf->cap < mincap) {
    size_t newcap = buf->cap * 2;
    if(newcap < mincap) newcap = mincap;
    buf->chars = realloc(buf->chars, newcap);
    buf->cap = newcap;
  }
  // Copy contents
  memcpy(buf->chars + buf->size, str, length);
  buf->size += length;
}
