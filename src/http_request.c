#include "http_request.h"
#include <stdio.h>
#include <string.h>

void http_request_parse(HttpRequest* request, char* text) {
  // Get first line: HTTP version, method, URL
  char* curline = strsep(&text, "\n");
  printf("HTTP line 1: %s\n", curline);

  // Parse headers
  while(curline) {
    // Get next line. Break if "\r\n" is reached (or in our case, "\r\0").
    curline = strsep(&text, "\n");
    if(!curline) {
      printf("Unexpected end of text while parsing HTTP request\n");
      break;
    }
    if(curline[0] == '\r' && curline[1] == '\0') {
      break;
    }

    // Parse headers
    printf("HTTP header: %s\n", curline); //TODO
  }
  printf("DONE WITH HEADERS\n");

  printf("Remaining text: %s\n", text);
  printf("- is null? %i\n", (text == NULL));

  // Depending on type, get input data
  //TODO
}

void http_request_free(HttpRequest* request) {
  //TODO
}
