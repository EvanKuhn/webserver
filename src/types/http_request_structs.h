//==============================================================================
// HttpRequest struct types. FOR INTERNAL USE ONLY!
//
// Evan Kuhn 2012-09-09
//==============================================================================
#ifndef HTTP_REQUEST_STRUCTS_H
#define HTTP_REQUEST_STRUCTS_H

struct HttpRequest {
  enum EHttpVersion     version;      // HTTP version
  enum EHttpRequestType type;         // Request type
  size_t                num_headers;  // Number of headers
  HttpHeader*           headers;      // Array of headers
  char*                 body;         // Request body
};

#endif // HTTP_REQUEST_STRUCTS_H