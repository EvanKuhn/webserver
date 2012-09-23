#include "utils.h"
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
