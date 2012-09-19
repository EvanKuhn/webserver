#include "utils.h"
#include <string.h>
#include <ctype.h>

const char* safe_cstr(const char* s) {
  return (s ? s : "");
}

//TODO - we can't modify string literals! Hmm. Copy string and return copy?

char* trim(char* str) {
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

char* strsep_trim(char** str, const char* delims) {
  // Tokenize
  char* token = strsep(str, delims);

  // Trim
  if(token) {
    // Trim left
    char c = *token;
    while(c && isspace(c)) {
      c = *(++token);
    }

    // Trim right
    const size_t toklen = strlen(token);
    for(size_t i=0; i<toklen; ++i) {
      size_t index = toklen - i - 1;
      if(!isspace(token[index])) {
        token[index+1] = 0;
        break;
      }
    }
  }
  return token;
}
