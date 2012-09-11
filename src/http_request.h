//==============================================================================
// HttpRequest struct and related functions for parsing an HTTP request and
// examining its contents
//
// Evan Kuhn 2012-09-09
//==============================================================================
#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <sys/types.h>

// Enum of HTTP request types
enum EHttpRequestType {
  HTTP_GET,
  HTTP_HEAD,
  HTTP_POST,
  HTTP_PUT,
  HTTP_REQUEST_TYPE_UNKNOWN
};

//==============================================================================
// An HTTP header contains a key and value
//==============================================================================
typedef struct HttpHeader {
  char* key;
  char* val;
} HttpHeader;

// Initialize struct's fields
void http_header_init(HttpHeader* header);

// Set the key or value
void http_header_set_key(HttpHeader* header, char* key);
void http_header_set_val(HttpHeader* header, char* val);

// Free allocated data
void http_header_free(HttpHeader* header);

//==============================================================================
// Struct containing all info from an HTTP request
//==============================================================================
typedef struct HttpRequest {
  enum EHttpRequestType type;  // Request type
  HttpHeader* headers;         // Array of headers
  size_t num_headers;          // Number of headers
  char* body;                  // Request body
} HttpRequest;

// Initialize struct's fields
void http_request_init(HttpRequest* request);

// Parse the request and populated the struct's fields.
// - You MUST call http_request_free() after http_request_parse() to free
//   allocated resources referenced within the HttpRequest object.
// - If reusing an HttpRequest object, first free it using http_request_free().
//   Otherwise a memory leak will occur.
void http_request_parse(HttpRequest* request, char* text);

// Free resources allocated during parsing
void http_request_free(HttpRequest* request);

#endif // HTTP_REQUEST_H
