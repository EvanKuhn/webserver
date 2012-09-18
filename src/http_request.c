#include "http_request.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

//==============================================================================
// Enums
//==============================================================================
enum EHttpVersion http_version_from_string(const char* str) {
  if(!strcmp(str, "HTTP/1.0")) return HTTP_VERSION_1_0;
  if(!strcmp(str, "HTTP/1.1")) return HTTP_VERSION_1_1;
  return HTTP_VERSION_UNKNOWN;
}

enum EHttpMethod http_method_from_string(const char* str) {
  if(!strcmp(str, "GET" )) return HTTP_METHOD_GET;
  if(!strcmp(str, "HEAD")) return HTTP_METHOD_HEAD;
  if(!strcmp(str, "POST")) return HTTP_METHOD_POST;
  if(!strcmp(str, "PUT" )) return HTTP_METHOD_PUT;
  return HTTP_METHOD_UNKNOWN;
}

const char* http_version_to_string(enum EHttpVersion x) {
  switch(x) {
    case HTTP_VERSION_1_0: return "HTTP/1.0";
    case HTTP_VERSION_1_1: return "HTTP/1.1";
    default:               return "?";
  }
}

const char* http_method_to_string(enum EHttpMethod x) {
  switch(x) {
    case HTTP_METHOD_GET:  return "GET";
    case HTTP_METHOD_HEAD: return "HEAD";
    case HTTP_METHOD_POST: return "POST";
    case HTTP_METHOD_PUT:  return "PUT";
    default:               return "?";
  }
}

//==============================================================================
// HttpHeader
//==============================================================================
void http_header_init(HttpHeader* header) {
  header->key = 0;
  header->value = 0;
}

void http_header_free(HttpHeader* header) {
  if(header->key) free(header->key);
  if(header->value) free(header->value);
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

void http_header_set_value(HttpHeader* header, const char* value) {
  if(header->value) {
    free(header->value);
  }
  const size_t valsize = strlen(value);
  header->value = malloc(valsize + 1);
  strcpy(header->value, value);
}

//==============================================================================
// HttpRequest
//==============================================================================
void http_request_init(HttpRequest* request) {
  request->version = HTTP_VERSION_UNKNOWN;
  request->method = HTTP_METHOD_UNKNOWN;
  request->uri = 0;
  request->num_headers = 0;
  request->header_cap = 0;
  request->headers = 0;
  request->body = 0;
}

void http_request_free(HttpRequest* request) {
  if(request->uri)     free(request->uri);
  if(request->headers) free(request->headers);
  if(request->body)    free(request->body);
  http_request_init(request);
}

HttpHeader* http_request_add_header(HttpRequest* request) {
  // Check if we need to allocate more memory
  if(request->num_headers == request->header_cap) {
    // Figure out the new size: start at 4, then double
    const size_t newsize = (request->num_headers ? request->num_headers * 2 : 4);

    // Allocate or reallocate memory
    if(request->headers) {
      request->headers = realloc(request->headers, sizeof(HttpHeader) * newsize);
    }
    else {
      request->headers = malloc(sizeof(HttpHeader) * newsize);
    }
    request->header_cap = newsize;

    // Initialize new header objects
    for(size_t i=request->num_headers; i<newsize; ++i) {
      http_header_init(&request->headers[i]);
    }
  }

  // Increment size and return new header
  request->num_headers += 1;
  return &request->headers[request->num_headers - 1];
}

// Parse the HTTP request line
// Format:  Method SP Request-URI SP HTTP-Version CRLF
// Example: GET / HTTP/1.1
bool http_request_parse_request_line(HttpRequest* request, char* line) {
  char* token = NULL;

  // Token 1: HTTP method
  token = strsep_trim(&line, " ");
  if(!token) {
    fprintf(stderr, "Error parsing HTTP header line: didn't find method (token 1)");
    return false;
  }
  request->method = http_method_from_string(token);
  if(request->method == HTTP_METHOD_UNKNOWN) {
    fprintf(stderr, "Error parsing HTTP header line: unknown method \"%s\"\n", token);
    return false;
  }

  // Token 2: URI
  token = strsep_trim(&line, " ");
  if(!token) {
    fprintf(stderr, "Error parsing HTTP header line: didn't find request URI (token 2)");
    return false;
  }
  request->uri = strdup(token);

  // Token 3: HTTP version
  token = strsep_trim(&line, " ");
  if(!token) {
    fprintf(stderr, "Error parsing HTTP header line: didn't find HTTP version (token 3)");
    return false;
  }
  request->version = http_version_from_string(token);
  if(request->method == HTTP_VERSION_UNKNOWN) {
    fprintf(stderr, "Unrecognized HTTP version \"%s\"\n", token);
    return false;
  }
  return true;
}

bool http_request_parse_header_line(HttpRequest* request, char* line) {
  char* line_start = line;
  char* token = NULL;
  HttpHeader* header = http_request_add_header(request);

  // Fetch and store key
  token = strsep_trim(&line, ":");
  if(!token) goto failure;
  http_header_set_key(header, token);

  // Fetch and store value
  token = strsep_trim(&line, ":");
  if(!token) goto failure;
  http_header_set_value(header, token);
  return true;

  // If it hits the fan, clean up
failure:
  fprintf(stderr, "Error parsing HTTP header line %s", line_start);
  request->num_headers -= 1;
  return false;
}

void http_request_parse(HttpRequest* request, char* text) {
  // Clear any existing data and reset the object
  http_request_free(request);

  // Parse the HTTP request line (the first line): HTTP method, URI, version
  char* curline = strsep(&text, "\n");
  http_request_parse_request_line(request, curline);

  // Parse headers
  while(curline) {
    // Get next line. Break if "\r\n" is reached (or in our case, "\r\0").
    curline = strsep(&text, "\n");
    if(!curline) {
      fprintf(stderr, "Unexpected end of text while parsing HTTP request\n");
      break;
    }
    if(curline[0] == '\r' && curline[1] == '\0') {
      break;
    }

    // Store header key and value
    char* curline_orig = curline;
    if(!http_request_parse_header_line(request, curline)) {
      fprintf(stderr, "Error parsing HTTP header line %s", curline_orig);
      continue;
    }
  }

  // Store the remaining text as the body
  if(text) {
    const size_t textlen = strlen(text);
    request->body = malloc(textlen + 1);
    memcpy(request->body, text, textlen + 1);
  }

  // Print the request
  http_request_print(request); //TODO
}

void http_request_print(HttpRequest* request) {
  printf("HTTP Request\n");
  printf("- Version: %s\n", http_version_to_string(request->version));
  printf("- Method:  %s\n", http_method_to_string(request->method));
  printf("- URI:     %s\n", (request->uri ? request->uri : "<none>"));
  printf("- Headers (%zu):\n", request->num_headers);
  for(size_t i=0; i<request->num_headers; ++i) {
    printf("  %s: %s\n", request->headers[i].key, request->headers[i].value);
  }
  printf("- Body:     %s\n", (request->body ? request->body : "<none>"));
}
