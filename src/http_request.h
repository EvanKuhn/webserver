//==============================================================================
// HttpRequest struct and related functions for parsing an HTTP request and
// examining its contents
//
// Evan Kuhn 2012-09-09
//==============================================================================
#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <sys/types.h>
#include "http_enums.h"

//==============================================================================
// An HTTP header contains a key and value
//==============================================================================
typedef struct HttpHeader {
  char* key;
  char* value;
} HttpHeader;

// Initialize or free the struct's fields
void http_header_init(HttpHeader* header);
void http_header_free(HttpHeader* header);

// Set the key or value
void http_header_set_key  (HttpHeader* header, const char* key);
void http_header_set_value(HttpHeader* header, const char* value);

//==============================================================================
// Struct containing all info from an HTTP request
//==============================================================================
typedef struct HttpRequest {
  enum EHttpVersion version;      // HTTP version
  enum EHttpMethod  method;       // HTTP method
  char*             uri;          // URI of resource
  size_t            num_headers;  // Number of headers
  size_t            header_cap;   // Header array capacity
  HttpHeader*       headers;      // Array of headers
  char*             body;         // Request body
} HttpRequest;

// Initialize or free the struct's fields
// - 'free' will free the uri, headers, and body, then reinitialize all fields
void http_request_init(HttpRequest* request);
void http_request_free(HttpRequest* request);

// Parse the request and populated the struct's fields
// - Will modify the input string
void http_request_parse(HttpRequest* request, char* text);

// Add or remove a header. Acts on the end of the headers array.
// - You probably won't need to use these. They're mainly for internal use.
HttpHeader* http_request_add_header(HttpRequest* request);
void        http_request_pop_header(HttpRequest* request);

// Print an HttpRequest to stdout. For debugging.
void http_request_print(HttpRequest* request);

#endif // HTTP_REQUEST_H
