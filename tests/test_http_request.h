//==============================================================================
// HttpRequest tests
//
// Evan Kuhn 2012-09-09
//==============================================================================
#ifndef TEST_HTTP_REQUEST_H
#define TEST_HTTP_REQUEST_H

#include "nu_unit.h"
#include "http_request.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

//==============================================================================
// HTTP enum tests
//==============================================================================
void test__http_version_from_string() {
  enum EHttpVersion val = HTTP_VERSION_UNKNOWN;
  val = http_version_from_string("HTTP/1.0");
  nu_check("failed to recognize HTTP/1.0", val == HTTP_VERSION_1_0);
  val = http_version_from_string("HTTP/1.1");
  nu_check("failed to recognize HTTP/1.1", val == HTTP_VERSION_1_1);
  val = http_version_from_string("1.2.3");
  nu_check("failed to return UNKOWN for invalid version", val == HTTP_VERSION_UNKNOWN);
}

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
  val = http_method_from_string("SHOUT");
  nu_check("failed to return UNKNOWN for invalid method", val == HTTP_METHOD_UNKNOWN);
}

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
  str = http_method_to_string(HTTP_METHOD_UNKNOWN);
  nu_check("failed to convert HTTP_METHOD_UNKNOWN to string", strcmp(str, "?")==0);
  str = http_method_to_string(1234);
  nu_check("failed to convert invalid enum to string", strcmp(str, "?")==0);
}

//==============================================================================
// HttpHeader tests
//==============================================================================
void test__http_header_init() {
  HttpHeader h;
  http_header_init(&h);
  nu_check("failed to initialize key to NULL", h.key == NULL);
  nu_check("failed to initialize value to NULL", h.value == NULL);
}

void test__http_header_set_key() {
  HttpHeader h;
  http_header_init(&h);
  http_header_set_key(&h, "foo");
  nu_check("failed to set key", h.key);
  nu_check("failed to set key to \"foo\"", (strcmp(h.key, "foo") == 0));
}

void test__http_header_set_value() {
  HttpHeader h;
  http_header_init(&h);
  http_header_set_value(&h, "bar");
  nu_check("failed to set value", h.value);
  nu_check("failed to set value to \"bar\"", (strcmp(h.value, "bar") == 0));
}

void test__http_header_free() {
  HttpHeader h;
  http_header_init(&h);
  http_header_set_key(&h, "foo");
  http_header_set_value(&h, "bar");
  http_header_free(&h);
  nu_check("failed to set key to NULL", h.key == NULL);
  nu_check("failed to set value to NULL", h.value == NULL);
}

//==============================================================================
// HttpRequest tests
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

void test__http_request_init() {
  HttpRequest request;
  http_request_init(&request);
  nu_check("failed to initialize version", request.version == HTTP_VERSION_UNKNOWN);
  nu_check("failed to initialize method", request.method == HTTP_METHOD_UNKNOWN);
  nu_check("failed to initialize uri", request.uri == 0);
  nu_check("failed to initialize num_headers", request.num_headers == 0);
  nu_check("failed to initialize header_cap", request.header_cap == 0);
  nu_check("failed to initialize headers", request.headers == 0);
  nu_check("failed to initialize body", request.body == 0);
}

void test__http_request_parse() {
  HttpRequest request;
  http_request_init(&request);
  char* request_str = strdup(HTTP_REQUEST_STRING);
  http_request_parse(&request, request_str);
  free(request_str);
  nu_check("failed to set version", request.version == HTTP_VERSION_1_1);
  nu_check("failed to set method", request.method == HTTP_METHOD_POST);
  nu_check("failed to set URI", !strcmp(request.uri, "/"));
  nu_check("failed to set num_headers", request.num_headers == 5);
  nu_check("failed to set header[0].key",   !strcmp(request.headers[0].key,   "User-Agent"));
  nu_check("failed to set header[0].value", !strcmp(request.headers[0].value, "curl/7.24.0"));
  nu_check("failed to set header[1].key",   !strcmp(request.headers[1].key,   "Host"));
  nu_check("failed to set header[1].value", !strcmp(request.headers[1].value, "localhost:4445"));
  nu_check("failed to set header[2].key",   !strcmp(request.headers[2].key,   "Accept"));
  nu_check("failed to set header[2].value", !strcmp(request.headers[2].value, "*/*"));
  nu_check("failed to set header[3].key",   !strcmp(request.headers[3].key,   "Content-Length"));
  nu_check("failed to set header[3].value", !strcmp(request.headers[3].value, "4"));
  nu_check("failed to set header[4].key",   !strcmp(request.headers[4].key,   "Content-Type"));
  nu_check("failed to set header[4].value", !strcmp(request.headers[4].value, "application/x-www-form-urlencoded"));
  nu_check("failed to set body", !strcmp(request.body, "this is the body\nit has multiple lines"));
  http_request_free(&request);
}

void test__http_request_add_header() {
  HttpRequest request;
  http_request_init(&request);
  nu_not_implemented(); //TODO
}

void test__http_request_pop_header() {
  HttpRequest request;
  http_request_init(&request);
  nu_not_implemented(); //TODO
}

void test__http_request_free() {
  HttpRequest request;
  http_request_init(&request);
  nu_not_implemented(); //TODO
}

//==============================================================================
// Test-all function
//==============================================================================
void test_suite__http_enums() {
  nu_run_test(test__http_version_from_string, "http_version_from_string()");
  nu_run_test(test__http_method_from_string,  "http_method_from_string()");
  nu_run_test(test__http_version_to_string,   "http_version_to_string()");
  nu_run_test(test__http_method_to_string,    "http_method_to_string()");
}

void test_suite__http_header() {
  nu_run_test(test__http_header_init,      "http_header_init()");
  nu_run_test(test__http_header_set_key,   "http_header_set_key()");
  nu_run_test(test__http_header_set_value, "http_header_set_value()");
  nu_run_test(test__http_header_free,      "http_header_free()");
}

void test_suite__http_request() {
  nu_run_test(test__http_request_init,       "http_request_init()");
  nu_run_test(test__http_request_parse,      "http_request_parse()");
  nu_run_test(test__http_request_add_header, "http_request_add_header()");
  nu_run_test(test__http_request_pop_header, "http_request_pop_header()");
  nu_run_test(test__http_request_free,       "http_request_free()");
}

#endif // TEST_HTTP_REQUEST_H