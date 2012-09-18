//==============================================================================
// HttpRequest tests
//
// Evan Kuhn 2012-09-09
//==============================================================================
#ifndef TEST_HTTP_REQUEST_H
#define TEST_HTTP_REQUEST_H

#include "minunit.h"
#include "http_request.h"
#include <stdbool.h>

//==============================================================================
// HTTP enum types
//==============================================================================
static char* test__http_version_from_string() {
  puts("test: http_version_from_string()");
  enum EHttpVersion val = HTTP_VERSION_UNKNOWN;
  val = http_version_from_string("HTTP/1.0");
  mu_assert("failed to recognize HTTP/1.0", val == HTTP_VERSION_1_0);
  val = http_version_from_string("HTTP/1.1");
  mu_assert("failed to recognize HTTP/1.1", val == HTTP_VERSION_1_1);
  val = http_version_from_string("1.2.3");
  mu_assert("failed to return UNKOWN for invalid version", val == HTTP_VERSION_UNKNOWN);
  return 0;
}

static char* test__http_method_from_string() {
  puts("test: http_method_from_string()");
  enum EHttpMethod val = HTTP_METHOD_UNKNOWN;
  val = http_method_from_string("GET");
  mu_assert("failed to recognize GET", val == HTTP_METHOD_GET);
  val = http_method_from_string("HEAD");
  mu_assert("failed to recognize HEAD", val == HTTP_METHOD_HEAD);
  val = http_method_from_string("POST");
  mu_assert("failed to recognize POST", val == HTTP_METHOD_POST);
  val = http_method_from_string("PUT");
  mu_assert("failed to recognize PUT", val == HTTP_METHOD_PUT);
  val = http_method_from_string("SHOUT");
  mu_assert("failed to return UNKNOWN for invalid method", val == HTTP_METHOD_UNKNOWN);
  return 0;
}

//TODO - more tests!
//const char* http_version_to_string(enum EHttpVersion x);
//const char* http_method_to_string(enum EHttpMethod x);


//==============================================================================
// HttpHeader
//==============================================================================
static char* test__http_header_init() {
  puts("test: http_header_init()");
  HttpHeader h;
  http_header_init(&h);
  mu_assert("failed to initialize key to NULL", h.key == NULL);
  mu_assert("failed to initialize value to NULL", h.value == NULL);
  return 0;
}

static char* test__http_header_set_key() {
  puts("test: http_header_set_key()");
  HttpHeader h;
  http_header_init(&h);
  http_header_set_key(&h, "foo");
  mu_assert("failed to set key", h.key);
  mu_assert("failed to set key to \"foo\"", (strcmp(h.key, "foo") == 0));
  return 0;
}

static char* test__http_header_set_value() {
  puts("test: http_header_set_value()");
  HttpHeader h;
  http_header_init(&h);
  http_header_set_value(&h, "bar");
  mu_assert("failed to set value", h.value);
  mu_assert("failed to set value to \"bar\"", (strcmp(h.value, "bar") == 0));
  return 0;
}

static char* test__http_header_free() {
  puts("test: http_header_free()");
  HttpHeader h;
  http_header_init(&h);
  http_header_set_key(&h, "foo");
  http_header_set_value(&h, "bar");
  http_header_free(&h);
  mu_assert("failed to set key to NULL", h.key == NULL);
  mu_assert("failed to set value to NULL", h.value == NULL);
  return 0;
}

//==============================================================================
// HttpRequest
//==============================================================================
static char* test__http_request_init() {
  puts("test: http_request_init()");
  HttpRequest request;
  http_request_init(&request);
  mu_assert("failed to initialize version", request.version == HTTP_VERSION_UNKNOWN);
  mu_assert("failed to initialize method", request.method == HTTP_METHOD_UNKNOWN);
  mu_assert("failed to initialize uri", request.uri == 0);
  mu_assert("failed to initialize num_headers", request.num_headers == 0);
  mu_assert("failed to initialize header_cap", request.header_cap == 0);
  mu_assert("failed to initialize headers", request.headers == 0);
  mu_assert("failed to initialize body", request.body == 0);
  return 0;
}

static char* test__http_request_parse() {
  puts("test: http_request_parse()");
  HttpRequest request;
  http_request_init(&request);
  //TODO
  return 0;
}

static char* test__http_request_add_header() {
  puts("test: http_request_add_header()");
  HttpRequest request;
  http_request_init(&request);
  //TODO
  return 0;
}

static char* test__http_request_free() {
  puts("test: http_request_free()");
  HttpRequest request;
  http_request_init(&request);
  //TODO
  return 0;
}

//==============================================================================
// Test-all function
//==============================================================================
char* test_http_request() {
  char* msg = NULL;

  printf("\n");
  mu_run_test(test__http_version_from_string);
  mu_run_test(test__http_method_from_string);

  printf("\n");
  mu_run_test(test__http_header_init);
  mu_run_test(test__http_header_set_key);
  mu_run_test(test__http_header_set_value);
  mu_run_test(test__http_header_free);

  printf("\n");
  mu_run_test(test__http_request_init);
  mu_run_test(test__http_request_parse);
  mu_run_test(test__http_request_add_header);
  mu_run_test(test__http_request_free);

  return msg;
}

#endif // TEST_HTTP_REQUEST_H