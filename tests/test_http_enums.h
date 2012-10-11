//==============================================================================
// HTTP enums tests
//
// Evan Kuhn 2012-09-09
//==============================================================================
#ifndef TEST_HTTP_ENUMS_H
#define TEST_HTTP_ENUMS_H

#include "nu_unit.h"
#include "http_enums.h"
#include <string.h>

//==============================================================================
// HTTP Versions
//==============================================================================
void test__http_version_to_string() {
  const char* str = NULL;
  str = http_version_to_string(HTTP_VERSION_1_0);
  nu_check("failed to convert HTTP_VERSION_1_0 to string", strcmp(str, "HTTP/1.0")==0);
  str = http_version_to_string(HTTP_VERSION_1_1);
  nu_check("failed to convert HTTP_VERSION_1_1 to string", strcmp(str, "HTTP/1.1")==0);
  str = http_version_to_string(HTTP_VERSION_UNKNOWN);
  nu_check("failed to convert HTTP_VERSION_UNKNOWN to string", strcmp(str, "?")==0);
  str = http_version_to_string(88);
  nu_check("failed to convert invalid enum to string", strcmp(str, "?")==0);
}

void test__http_version_from_string() {
  enum EHttpVersion val = HTTP_VERSION_UNKNOWN;
  val = http_version_from_string("HTTP/1.0");
  nu_check("failed to recognize HTTP/1.0", val == HTTP_VERSION_1_0);
  val = http_version_from_string("HTTP/1.1");
  nu_check("failed to recognize HTTP/1.1", val == HTTP_VERSION_1_1);
  val = http_version_from_string("1.2.3");
  nu_check("failed to return UNKOWN for invalid version", val == HTTP_VERSION_UNKNOWN);
}

//==============================================================================
// HTTP Request Methods
//==============================================================================
void test__http_method_from_string() {
  enum EHttpMethod val = HTTP_METHOD_UNKNOWN;
  val = http_method_from_string("GET");
  nu_check("failed to recognize GET", val == HTTP_METHOD_GET);
  val = http_method_from_string("HEAD");
  nu_check("failed to recognize HEAD", val == HTTP_METHOD_HEAD);
  val = http_method_from_string("POST");
  nu_check("failed to recognize POST", val == HTTP_METHOD_POST);
  val = http_method_from_string("PUT");
  nu_check("failed to recognize PUT", val == HTTP_METHOD_PUT);
  val = http_method_from_string("DELETE");
  nu_check("failed to recognize DELETE", val == HTTP_METHOD_DELETE);
  val = http_method_from_string("SHOUT");
  nu_check("failed to return UNKNOWN for invalid method", val == HTTP_METHOD_UNKNOWN);
}

void test__http_method_to_string() {
  const char* str = NULL;
  str = http_method_to_string(HTTP_METHOD_GET);
  nu_check("failed to convert HTTP_METHOD_GET to string", strcmp(str, "GET")==0);
  str = http_method_to_string(HTTP_METHOD_HEAD);
  nu_check("failed to convert HTTP_METHOD_HEAD to string", strcmp(str, "HEAD")==0);
  str = http_method_to_string(HTTP_METHOD_POST);
  nu_check("failed to convert HTTP_METHOD_POST to string", strcmp(str, "POST")==0);
  str = http_method_to_string(HTTP_METHOD_PUT);
  nu_check("failed to convert HTTP_METHOD_PUT to string", strcmp(str, "PUT")==0);
  str = http_method_to_string(HTTP_METHOD_DELETE);
  nu_check("failed to convert HTTP_METHOD_DELETE to string", strcmp(str, "DELETE")==0);
  str = http_method_to_string(HTTP_METHOD_UNKNOWN);
  nu_check("failed to convert HTTP_METHOD_UNKNOWN to string", strcmp(str, "?")==0);
  str = http_method_to_string(1234);
  nu_check("failed to convert invalid enum to string", strcmp(str, "?")==0);
}

//==============================================================================
// HTTP Status Codes
//==============================================================================
void test__http_status_to_string() {
  const char* str = NULL;
  str = http_status_to_string(HTTP_STATUS_OK);
  nu_check("failed to convert HTTP_STATUS_OK to string", !strcmp(str, "OK"));
  str = http_status_to_string(HTTP_STATUS_BAD_REQUEST);
  nu_check("failed to convert HTTP_STATUS_BAD_REQUEST to string", !strcmp(str, "Bad Request"));
  str = http_status_to_string(HTTP_STATUS_NOT_FOUND);
  nu_check("failed to convert HTTP_STATUS_NOT_FOUND to string", !strcmp(str, "Not Found"));
  str = http_status_to_string(HTTP_STATUS_NOT_IMPLEMENTED);
  nu_check("failed to convert HTTP_STATUS_NOT_IMPLEMENTED to string", !strcmp(str, "Not Implemented"));
  str = http_status_to_string(HTTP_STATUS_UNKNOWN);
  nu_check("failed to convert HTTP_STATUS_UNKNOWN to string", !strcmp(str, "?"));
  str = http_status_to_string(HTTP_STATUS_UNKNOWN + 3);
  nu_check("failed to convert invalid enum to string", !strcmp(str, "?"));
}

void test__http_status_from_string() {
  enum EHttpStatus val = HTTP_STATUS_UNKNOWN;
  val = http_status_from_string("OK");
  nu_check("failed to recognize OK", val == HTTP_STATUS_OK);
  val = http_status_from_string("Bad Request");
  nu_check("failed to recognize Bad Request", val == HTTP_STATUS_BAD_REQUEST);
  val = http_status_from_string("Not Found");
  nu_check("failed to recognize Not Found", val == HTTP_STATUS_NOT_FOUND);
  val = http_status_from_string("Not Implemented");
  nu_check("failed to recognize Not Implemented", val == HTTP_STATUS_NOT_IMPLEMENTED);
  val = http_status_from_string("AMAZING");
  nu_check("failed to return UNKNOWN for invalid status", val == HTTP_STATUS_UNKNOWN);
}

//==============================================================================
// Test-suite functions
//==============================================================================
void test_suite__http_enums() {
  nu_run_test(test__http_version_to_string,   "http_version_to_string()");
  nu_run_test(test__http_version_from_string, "http_version_from_string()");
  nu_run_test(test__http_method_to_string,    "http_method_to_string()");
  nu_run_test(test__http_method_from_string,  "http_method_from_string()");
  nu_run_test(test__http_status_to_string,    "http_status_to_string()");
  nu_run_test(test__http_status_from_string,  "http_status_from_string()");
}

#endif // TEST_HTTP_ENUMS_H