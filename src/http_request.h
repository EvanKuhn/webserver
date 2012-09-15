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
  HTTP_VERSION_1_0,
  HTTP_VERSION_1_1,
  HTTP_VERSION_UNKNOWN
};

// HTTP request types
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

// Initialize or free the struct's fields
void http_header_init(HttpHeader* header);
void http_header_free(HttpHeader* header);

// Set the key or value. Will free existing data.
void http_header_set_key(HttpHeader* header, char* key);
void http_header_set_val(HttpHeader* header, char* val);

//==============================================================================
// Struct containing all info from an HTTP request
//==============================================================================
typedef struct HttpRequest HttpRequest;

// Create a new HttpRequest object
HttpRequest* http_request_new();

// Delete the HttpRequest object and all internally-allocated resources
void http_request_free(HttpRequest* request);

// Parse the request and populated the struct's fields
// - Will clear the request's data if populated by a previous call to parse
void http_request_parse(HttpRequest* request, char* text);

// Get the HTTP request version
enum EHttpVersion http_request_get_version(HttpRequest* request);

// Get the HTTP request type
enum EHttpRequestType http_request_get_type(HttpRequest* request);

// Get the number of HTTP request headers specified
size_t http_request_num_headers(HttpRequest* request);

// Get the array of HTTP request headers. Returns NULL if array empty.
HttpHeader* http_request_get_headers(HttpRequest* request);

// Get the HTTP request body, or NULL if no body given
char* http_request_get_body(HttpRequest* request);

#endif // HTTP_REQUEST_H
