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

//==============================================================================
// An HTTP header contains a key and value
//==============================================================================
typedef struct HttpHeader {
  char* key;
  char* val;
} HttpHeader;

// Initialize or free the struct's fields
void http_header_init(HttpHeader* header);
void http_header_free(HttpHeader* header);

// Set the key or value
void http_header_set_key(HttpHeader* header, char* key);
void http_header_set_val(HttpHeader* header, char* val);

//==============================================================================
// Struct containing all info from an HTTP request
//==============================================================================
typedef struct HttpRequest {
  enum EHttpVersion version;      // HTTP version
  enum EHttpMethod  method;       // HTTP method
  char*             uri;          // URI of resource
  size_t            num_headers;  // Number of headers
  HttpHeader*       headers;      // Array of headers
  char*             body;         // Request body
} HttpRequest;

// Initialize or free the struct's fields
void http_request_init(HttpRequest* request);
void http_request_free(HttpRequest* request);

// Parse the request and populated the struct's fields
void http_request_parse(HttpRequest* request, char* text);

#endif // HTTP_REQUEST_H
