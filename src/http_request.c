#include "http_request.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//==============================================================================
// Utilities
//==============================================================================
// Examine the input string and find the next token
// - Splits tokens by whitespace
// - Ignores multiple whitespace chars
// - Ignores leading / trailing whitespace
// - Places a null-terminator at end of token
// - Returns the length of the token
// - Returns the token's beginning and ending chars via tok_begin and tok_end
size_t next_token(char* str, char** tok_begin, char** tok_end) {
  // Find first non-whitespace char, or null terminator
  char* first = str;
  while(first && isspace(*first)) {
    ++first;
  }

  // Advance to next whitespace char, or null terminator
  char* last = first;
  while(last && !isspace(*last)) {
    ++last;
  }

  // Terminate token
  if(last) *last = 0;

  // Set output vars and return length of string
  *tok_begin = first;
  *tok_end = last;
  return (last - first);
}

//==============================================================================
// Enums
//==============================================================================
enum EHttpVersion http_version_from_str(const char* str) {
  if(!strcmp(str, "HTTP/1.0")) return HTTP_VERSION_1_0;
  if(!strcmp(str, "HTTP/1.1")) return HTTP_VERSION_1_1;
  return HTTP_VERSION_UNKNOWN;
}

enum EHttpMethod http_method_from_str(const char* str) {
  if(!strcmp(str, "GET" )) return HTTP_METHOD_GET;
  if(!strcmp(str, "HEAD")) return HTTP_METHOD_HEAD;
  if(!strcmp(str, "POST")) return HTTP_METHOD_POST;
  if(!strcmp(str, "PUT" )) return HTTP_METHOD_PUT;
  return HTTP_METHOD_UNKNOWN;
}

//==============================================================================
// HttpHeader
//==============================================================================
void http_header_init(HttpHeader* header) {
  header->key = 0;
  header->val = 0;
}

void http_header_free(HttpHeader* header) {
  if(header->key) free(header->key);
  if(header->val) free(header->val);
  http_header_init(header);
}

void http_header_set_key(HttpHeader* header, const char* key) {
  if(header->key) {
    free(header->key);
  }
  const size_t keysize = strlen(key);
  header->key = malloc(keysize + 1);
  strcpy(header->key, key);
}

void http_header_set_val(HttpHeader* header, const char* val) {
  if(header->val) {
    free(header->val);
  }
  const size_t valsize = strlen(val);
  header->val = malloc(valsize + 1);
  strcpy(header->val, val);
}

//==============================================================================
// HttpRequest
//==============================================================================
void http_request_init(HttpRequest* request) {
  request->version = HTTP_VERSION_UNKNOWN;
  request->method = HTTP_METHOD_UNKNOWN;
  request->uri = 0;
  request->num_headers = 0;
  request->headers = 0;
  request->body = 0;
}

void http_request_free(HttpRequest* request) {
  if(request->uri)     free(request->uri);
  if(request->headers) free(request->headers);
  if(request->body)    free(request->body);
  http_request_init(request);
}

// Parse the HTTP request line
// Format:  Method SP Request-URI SP HTTP-Version CRLF
// Example: GET / HTTP/1.1
bool http_request_parse_request_line(HttpRequest* request, char* line) {
  char* first = 0;
  char* last = 0;

  // Token 1: HTTP method
  if(!next_token(line, &first, &last)) {
    fprintf(stderr, "Error parsing HTTP header line: didn't find method (token 1)");
    return false;
  }
  request->method = http_method_from_str(first);
  if(request->method == HTTP_METHOD_UNKNOWN) {
    fprintf(stderr, "Error parsing HTTP header line: unknown method \"%s\"\n", first);
    return false;
  }

  // Token 2: URI
  first = last + 1;
  size_t toklen = next_token(line, &first, &last);
  if(!toklen) {
    fprintf(stderr, "Error parsing HTTP header line: didn't find request URI (token 2)");
    return false;
  }
  request->uri = malloc(toklen + 1);
  memcpy(request->uri, first, toklen + 1);

  // Token 3: HTTP version
  first = last + 1;
  if(!next_token(line, &first, &last)) {
    fprintf(stderr, "Error parsing HTTP header line: didn't find HTTP version (token 3)");
    return false;
  }
  request->version = http_version_from_str(first);
  if(request->method == HTTP_VERSION_UNKNOWN) {
    fprintf(stderr, "Unrecognized HTTP version \"%s\"\n", first);
    return false;
  }
  return true;
}

void http_request_parse(HttpRequest* request, char* text) {
  // Clear any existing data and reset the object
  http_request_free(request);

  // Parse the HTTP request line (the first line): HTTP method, URI, version
  char* curline = strsep(&text, "\n");
  printf("HTTP request: %s\n", curline); //TODO
  http_request_parse_request_line(request, curline);

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

    // Store header key and value
    printf("HTTP header: %s\n", curline); //TODO
    //TODO: parse header line, split into key and value, trim both, and copy them
  }

  // Store the remaining text as the body
  if(text) {
    const size_t textlen = strlen(text);
    request->body = malloc(textlen + 1);
    memcpy(request->body, text, textlen + 1);
  }
}
