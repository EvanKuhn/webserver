//==============================================================================
// HttpRequest struct and related functions for parsing an HTTP request and
// examining its contents
//
// Evan Kuhn 2012-09-09
//==============================================================================
#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

enum EHttpRequestType {
  HTTP_GET,
  HTTP_HEAD,
  HTTP_POST,
  HTTP_PUT
};

typedef struct HttpRequest {
  enum EHttpRequestType type;
} HttpRequest;

// Parse the request and populated the struct's fields.
// - If reusing an HttpRequest object, first free it using http_request_free.
//   Otherwise a memory leak will occur.
void http_request_parse(HttpRequest* request, char* text);

// Free resources allocated during parsing
void http_request_free(HttpRequest* request);

#endif // HTTP_REQUEST_H
