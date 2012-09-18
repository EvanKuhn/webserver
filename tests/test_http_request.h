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
#include <stdlib.h>
#include <string.h>

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

static char* test__http_version_to_string() {
  puts("test: http_version_to_string()");
  const char* str = NULL;
  str = http_version_to_string(HTTP_VERSION_1_0);
  mu_assert("failed to convert HTTP_VERSION_1_0 to string", strcmp(str, "HTTP/1.0")==0);
  str = http_version_to_string(HTTP_VERSION_1_1);
  mu_assert("failed to convert HTTP_VERSION_1_1 to string", strcmp(str, "HTTP/1.1")==0);
  str = http_version_to_string(HTTP_VERSION_UNKNOWN);
  mu_assert("failed to convert HTTP_VERSION_UNKNOWN to string", strcmp(str, "?")==0);
  str = http_version_to_string(88);
  mu_assert("failed to convert invalid enum to string", strcmp(str, "?")==0);
  return 0;
}

static char* test__http_method_to_string() {
  puts("test: http_method_to_string()");
  const char* str = NULL;
  str = http_method_to_string(HTTP_METHOD_GET);
  mu_assert("failed to convert HTTP_METHOD_GET to string", strcmp(str, "GET")==0);
  str = http_method_to_string(HTTP_METHOD_HEAD);
  mu_assert("failed to convert HTTP_METHOD_HEAD to string", strcmp(str, "HEAD")==0);
  str = http_method_to_string(HTTP_METHOD_POST);
  mu_assert("failed to convert HTTP_METHOD_POST to string", strcmp(str, "POST")==0);
  str = http_method_to_string(HTTP_METHOD_PUT);
  mu_assert("failed to convert HTTP_METHOD_PUT to string", strcmp(str, "PUT")==0);
  str = http_method_to_string(HTTP_METHOD_UNKNOWN);
  mu_assert("failed to convert HTTP_METHOD_UNKNOWN to string", strcmp(str, "?")==0);
  str = http_method_to_string(1234);
  mu_assert("failed to convert invalid enum to string", strcmp(str, "?")==0);
  return 0;
}

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
// Sample HTTP request string, for testing. Contains extra spaces to see if our
// code can handle them (by trimming them).
const char* HTTP_REQUEST_STRING =
  "POST / HTTP/1.1\n"
  "User-Agent:    curl/7.24.0\n"
  "Host: localhost:4445  \n"
  "Accept: */*\n"
  "Content-Length: 4\n"
  "Content-Type: application/x-www-form-urlencoded\n"
  "\r\n"
  "this is the body\n"
  "it has multiple lines"
  ;

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
  char* request_str = strdup(HTTP_REQUEST_STRING);
  http_request_parse(&request, request_str);
  free(request_str);
  mu_assert("failed to set version", request.version == HTTP_VERSION_1_1);
  mu_assert("failed to set method", request.method == HTTP_METHOD_POST);
  mu_assert("failed to set URI", !strcmp(request.uri, "/"));
  mu_assert("failed to set num_headers", request.num_headers == 5);
  mu_assert("failed to set header[0].key",   !strcmp(request.headers[0].key,   "User-Agent"));
  mu_assert("failed to set header[0].value", !strcmp(request.headers[0].value, "curl/7.24.0"));
  mu_assert("failed to set header[1].key",   !strcmp(request.headers[1].key,   "Host"));
  mu_assert("failed to set header[1].value", !strcmp(request.headers[1].value, "localhost:4445"));
  mu_assert("failed to set header[2].key",   !strcmp(request.headers[2].key,   "Accept"));
  mu_assert("failed to set header[2].value", !strcmp(request.headers[2].value, "*/*"));
  mu_assert("failed to set header[3].key",   !strcmp(request.headers[3].key,   "Content-Length"));
  mu_assert("failed to set header[3].value", !strcmp(request.headers[3].value, "4"));
  mu_assert("failed to set header[4].key",   !strcmp(request.headers[4].key,   "Content-Type"));
  mu_assert("failed to set header[4].value", !strcmp(request.headers[4].value, "application/x-www-form-urlencoded"));
  mu_assert("failed to set body", !strcmp(request.body, "this is the body\nit has multiple lines"));
  http_request_free(&request);
  return 0;
}

static char* test__http_request_add_header() {
  puts("test: http_request_add_header()");
  HttpRequest request;
  http_request_init(&request);
  //TODO
  return 0;
}

static char* test__http_request_pop_header() {
  puts("test: http_request_pop_header()");
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
  mu_run_test(test__http_version_to_string);
  mu_run_test(test__http_method_to_string);

  printf("\n");
  mu_run_test(test__http_header_init);
  mu_run_test(test__http_header_set_key);
  mu_run_test(test__http_header_set_value);
  mu_run_test(test__http_header_free);

  printf("\n");
  mu_run_test(test__http_request_init);
  mu_run_test(test__http_request_parse);
  mu_run_test(test__http_request_add_header);
  mu_run_test(test__http_request_pop_header);
  mu_run_test(test__http_request_free);

  return msg;
}

#endif // TEST_HTTP_REQUEST_H