#include "http_request.h"
#include "types/http_request_structs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//==============================================================================
// HttpHeader
//==============================================================================
void http_header_init(HttpHeader* header) {
  header->key = 0;
  header->val = 0;
}

void http_header_set_key(HttpHeader* header, char* key) {
  if(header->key) {
    free(header->key);
  }
  const size_t keysize = strlen(key);
  header->key = malloc(keysize + 1);
  strcpy(header->key, key);
}

void http_header_set_val(HttpHeader* header, char* val) {
  if(header->val) {
    free(header->val);
  }
  const size_t valsize = strlen(val);
  header->val = malloc(valsize + 1);
  strcpy(header->val, val);
}

void http_header_free(HttpHeader* header) {
  if(header->key) free(header->key);
  if(header->val) free(header->val);
  header->key = 0;
  header->val = 0;
}

//==============================================================================
// HttpRequest
//==============================================================================
HttpRequest* http_request_new() {
  HttpRequest* request = malloc(sizeof(HttpRequest));
  request->type = HTTP_REQUEST_TYPE_UNKNOWN;
  request->headers = 0;
  request->num_headers = 0;
  request->body = 0;
  return request;
}

void http_request_free(HttpRequest* request) {
  http_request_clear(request);
  free(request);
}

void http_request_clear(HttpRequest* request) {
  // Free allocated resources
  if(request->headers) free(request->headers);
  if(request->body) free(request->body);
  // Reset values
  request->type = HTTP_REQUEST_TYPE_UNKNOWN;
  request->headers = 0;
  request->num_headers = 0;
  request->body = 0;
}

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

  printf("Remaining text: %s\n", (text ? text : "(null)"));
  printf("- length: %zu\n", (text ? strlen(text) : 0));

  // Depending on type, get input data
  //TODO
}

enum EHttpVersion http_request_get_version(HttpRequest* request) {
  return request->version;
}

enum EHttpRequestType http_request_get_type(HttpRequest* request) {
  return request->type;
}

size_t http_request_num_headers(HttpRequest* request) {
  return request->num_headers;
}

HttpHeader* http_request_get_headers(HttpRequest* request) {
  return request->heders;
}

char* http_request_get_body(HttpRequest* request) {
  return request->body;
}
