#include "http_request.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

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
  if(request->headers) {
    for(size_t i=0; i<request->num_headers; ++i) {
      http_header_free(&request->headers[i]);
    }
    free(request->headers);
  }
  if(request->uri)  free(request->uri);
  if(request->body) free(request->body);
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

void http_request_pop_header(HttpRequest* request) {
  if(request->num_headers > 0) {
    http_header_free(&request->headers[request->num_headers-1]);
    request->num_headers -= 1;
  }
}

// Parse the HTTP request line
// Format:  Method SP Request-URI SP HTTP-Version CRLF
// Example: GET / HTTP/1.1
bool http_request_parse_request_line(HttpRequest* request, char* line) {
  char* token = NULL;

  // Token 1: HTTP method
  token = trim(strsep(&line, " "));
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
  token = trim(strsep(&line, " "));
  if(!token) {
    fprintf(stderr, "Error parsing HTTP header line: didn't find request URI (token 2)");
    return false;
  }
  request->uri = strdup(token);

  // Token 3: HTTP version
  token = trim(strsep(&line, " "));
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
  HttpHeader* header = http_request_add_header(request);

  // See if we can tokenize the line. If not, bail.
  const char* key = trim(strsep(&line, ":"));
  if(!key) {
    fprintf(stderr, "Error parsing HTTP header line %s", line_start);
    http_request_pop_header(request);
    return false;
  }

  // The value is the remainder of the line, trimmed
  const char* value = trim(line);

  // Store key and value
  http_header_set_key(header, key);
  http_header_set_value(header, value);
  return true;
}

void http_request_parse(HttpRequest* request, const char* text) {
  // Clear any existing data and reset the object
  http_request_free(request);

  // Copy the text
  char* textcopy = strdup(text);
  char* textcopy_start = textcopy;

  // Parse the HTTP request line (the first line): HTTP method, URI, version
  char* curline = strsep(&textcopy, "\n");
  http_request_parse_request_line(request, curline);

  // Parse headers
  while(textcopy) {
    // Get next line. Break if "\r\n" is reached (or in our case, "\r\0").
    curline = strsep(&textcopy, "\n");
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
  if(textcopy && textcopy[0]) {
    const size_t textlen = strlen(textcopy);
    request->body = malloc(textlen + 1);
    memcpy(request->body, textcopy, textlen + 1);
    request->body[textlen] = 0;
  }

  // Clean up our copy of the text
  free(textcopy_start);
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
