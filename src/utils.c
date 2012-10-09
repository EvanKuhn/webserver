#include "utils.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

const char* timestamp(timebuf_t buffer) {
  time_t rawtime;
  time(&rawtime);
  struct tm* timeinfo = gmtime(&rawtime);
  strftime(buffer, 22, "%Y%m%d-%H:%M:%S UTC", timeinfo);
  return buffer;
}
