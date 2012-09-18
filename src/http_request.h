//==============================================================================
// HttpRequest struct and related functions for parsing an HTTP request and
// examining its contents
//
// Evan Kuhn 2012-09-09
//==============================================================================
#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <sys/types.h>

//==============================================================================
// Enums
//==============================================================================
// HTTP versions
enum EHttpVersion {
  HTTP_VERSION_UNKNOWN,
  HTTP_VERSION_1_0,
  HTTP_VERSION_1_1
};

// HTTP methods
enum EHttpMethod {
  HTTP_METHOD_UNKNOWN,
  HTTP_METHOD_GET,
  HTTP_METHOD_HEAD,
  HTTP_METHOD_POST,
  HTTP_METHOD_PUT
};

// Convert from string to enum. Returns UNKNOWN enum value on error.
enum EHttpVersion http_version_from_string(const char* s);
enum EHttpMethod http_method_from_string(const char* s);

// Convert from enum to string. Returns "?" on error.
const char* http_version_to_string(enum EHttpVersion x);
const char* http_method_to_string(enum EHttpMethod x);

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
  size_t            header_cap;  // Header array capacity
  HttpHeader*       headers;      // Array of headers
  char*             body;         // Request body
} HttpRequest;

// Initialize or free the struct's fields
void http_request_init(HttpRequest* request);
void http_request_free(HttpRequest* request);

// Parse the request and populated the struct's fields
// - Will modify the input string
void http_request_parse(HttpRequest* request, char* text);

// Make room for a new header in the request. Return the new HttpHeader object.
// - You probably won't need to use this. It's mainly for internal use.
HttpHeader* http_request_add_header(HttpRequest* request);

// Print an HttpRequest to stdout. For debugging.
void http_request_print(HttpRequest* request);

#endif // HTTP_REQUEST_H
