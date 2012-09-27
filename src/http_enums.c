#include "http_enums.h"
#include <string.h>

//==============================================================================
// HTTP Versions
//==============================================================================
const char* http_version_to_string(enum EHttpVersion x) {
  switch(x) {
    case HTTP_VERSION_1_0: return "HTTP/1.0";
    case HTTP_VERSION_1_1: return "HTTP/1.1";
    default:               return "?";
  }
}

enum EHttpVersion http_version_from_string(const char* str) {
  if(!strcmp(str, "HTTP/1.0")) return HTTP_VERSION_1_0;
  if(!strcmp(str, "HTTP/1.1")) return HTTP_VERSION_1_1;
  return HTTP_VERSION_UNKNOWN;
}

//==============================================================================
// HTTP Request Methods
//==============================================================================
const char* http_method_to_string(enum EHttpMethod x) {
  switch(x) {
    case HTTP_METHOD_GET:    return "GET";
    case HTTP_METHOD_HEAD:   return "HEAD";
    case HTTP_METHOD_POST:   return "POST";
    case HTTP_METHOD_PUT:    return "PUT";
    case HTTP_METHOD_DELETE: return "DELETE";
    default:                 return "?";
  }
}

enum EHttpMethod http_method_from_string(const char* str) {
  if(!strcmp(str, "GET"   )) return HTTP_METHOD_GET;
  if(!strcmp(str, "HEAD"  )) return HTTP_METHOD_HEAD;
  if(!strcmp(str, "POST"  )) return HTTP_METHOD_POST;
  if(!strcmp(str, "PUT"   )) return HTTP_METHOD_PUT;
  if(!strcmp(str, "DELETE")) return HTTP_METHOD_DELETE;
  return HTTP_METHOD_UNKNOWN;
}

//==============================================================================
// HTTP Status Codes
//==============================================================================
const char* http_status_to_string(enum EHttpStatus x) {
  switch(x) {
    case HTTP_STATUS_OK:              return "OK";
    case HTTP_STATUS_NOT_FOUND:       return "Not Found";
    case HTTP_STATUS_NOT_IMPLEMENTED: return "Not Implemented";
    default:                          return "?";
  }
}

enum EHttpStatus http_status_from_string(const char* str) {
  if(!strcmp(str, "OK"))              return HTTP_STATUS_OK;
  if(!strcmp(str, "Not Found"))       return HTTP_STATUS_NOT_FOUND;
  if(!strcmp(str, "Not Implemented")) return HTTP_STATUS_NOT_IMPLEMENTED;
  return HTTP_STATUS_UNKNOWN;
}
