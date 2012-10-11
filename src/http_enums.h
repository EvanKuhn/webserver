//==============================================================================
// HTTP enums and string-conversion functions.
//
// NOTE:
//   - xxx_to_string() will return "?" on error
//   - xxx_from_string() will return the proper UNKNOWN enum value on error
//
// Evan Kuhn 2012-09-25
//==============================================================================
#ifndef HTTP_ENUMS_H
#define HTTP_ENUMS_H

//==============================================================================
// HTTP Versions
//==============================================================================
enum EHttpVersion {
  HTTP_VERSION_UNKNOWN,
  HTTP_VERSION_1_0,
  HTTP_VERSION_1_1
};

// String conversion
const char*       http_version_to_string  (enum EHttpVersion x);
enum EHttpVersion http_version_from_string(const char* s);

//==============================================================================
// HTTP Request Methods
//==============================================================================
enum EHttpMethod {
  HTTP_METHOD_UNKNOWN,
  HTTP_METHOD_GET,
  HTTP_METHOD_HEAD,
  HTTP_METHOD_POST,
  HTTP_METHOD_PUT,
  HTTP_METHOD_DELETE
};

// String conversion
const char*      http_method_to_string  (enum EHttpMethod x);
enum EHttpMethod http_method_from_string(const char* s);

//==============================================================================
// HTTP Status Codes
//==============================================================================
enum EHttpStatus {
  HTTP_STATUS_OK = 200,
  HTTP_STATUS_BAD_REQUEST = 400,
  HTTP_STATUS_NOT_FOUND = 404,
  HTTP_STATUS_NOT_IMPLEMENTED = 501,
  //TODO - etc
  HTTP_STATUS_UNKNOWN
};

// String conversion
const char*      http_status_to_string  (enum EHttpStatus x);
enum EHttpStatus http_status_from_string(const char* s);

#endif // HTTP_ENUMS_H