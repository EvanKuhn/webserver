#include "utils.h"
#include <ctype.h>

char read_char(FILE* stream) {
  return fgetc(stream);
}

bool read_word(FILE* stream, char* buf, size_t bufsize) {
  char c = 0;
  size_t i = 0;
  bool status = true;

  // Read leading space characters
  while(1) {
    c = fgetc(stream);
    if(c == EOF) {
      status = false;
      break;
    }
    if(!isspace(c)) {
      buf[i++] = c;
      break;
    }
  }
  
  // Read each char from the stream until we hit a space or EOF
  // - Leave room for trailing null char at the end
  if(status) {
    for( ; i<bufsize-1; ++i) {
      c = fgetc(stream);
      if(isspace(c)) {
	break;
      }
      if(c == EOF) {
	status = feof(stream);
	break;
      }
      buf[i] = c;
    }
  }
  
  // Add a trailing null char and return
  buf[i] = 0;
  return status;
}

bool read_line(FILE* stream, char* buf, size_t bufsize) {
  const char* result = fgets(buf, bufsize, stream);
  return (result == buf);
}

void upcase(char* str) {
  int i=0;
  for( ; str[i]; ++i) {
    str[i] = toupper(str[i]);
  }
}

void downcase(char* str) {
  int i=0;
  for( ; str[i]; ++i) {
    str[i] = tolower(str[i]);
  }
}

