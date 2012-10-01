//==============================================================================
// The HttpResponse struct is used for builing a response string. To use it,
// you must call these functions in the following order:
//
//   1) Set the status line:      http_response_set_status()
//   2) Add headers:              http_response_add_header()
//   3) Optionally, add a body:   http_response_set_body()
//   4) Get the response string:  http_response_string()
//
// If you set a body, make sure you set the Content-Length header! This struct
// is fairly simple and won't set it for you.
//
// Evan Kuhn 2012-09-26
//==============================================================================
#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include "http_enums.h"
#include <string.h>

typedef struct HttpResponse HttpResponse;

// Allocate and initialize a new HttpResponse object.
// - Caller must free it via http_response_free
HttpResponse* http_response_new();

// Free an HttpResponse object and its internal resources
void http_response_free(HttpResponse* res);

// Clear the response of all data (response line, headers, body)
void http_response_clear(HttpResponse* res);

// Set the status line, which contains an HTTP version and status
void http_response_set_status(HttpResponse* res, enum EHttpVersion ver, enum EHttpStatus status);

// Add headers, which contains a key and value
void http_response_add_header(HttpResponse* res, const char* key, const char* value);

// Set the HTTP body. Not required for all responses.
// - Make sure you set the Content-Length header if setting a body!
void http_response_set_body(HttpResponse* res, const char* body);

// Get the string constructed after adding response line, headers and body
const char* http_response_string(const HttpResponse* res);

// Get the length of the http_response_string
size_t http_response_length(const HttpResponse* res);

#endif // HTTP_RESPONSE_H