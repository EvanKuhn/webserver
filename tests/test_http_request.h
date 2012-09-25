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
  HttpHeader* header = NULL;
  HttpRequest request;
  http_request_init(&request);
  for(size_t i=1; i<=5; ++i) {
    header = http_request_add_header(&request);
    nu_assert("no header object returned", header);
    nu_assert("header not initialized", header->key == 0 && header->value == 0);
    nu_assert("request num_headers not set properly", request.num_headers == i);
    nu_assert("request header_cap not set properly", request.header_cap >= i);
  }
  http_request_free(&request);
}

void test__http_request_pop_header() {
  HttpHeader* header = NULL;
  HttpRequest request;
  http_request_init(&request);
  // Add two headers
  header = http_request_add_header(&request);
  header = http_request_add_header(&request);
  // Start popping
  nu_assert("expected num_headers to be 2", request.num_headers == 2);
  http_request_pop_header(&request);
  nu_assert("expected num_headers to be 2", request.num_headers == 1);
  http_request_pop_header(&request);
  nu_assert("expected num_headers to be 2", request.num_headers == 0);
  http_request_pop_header(&request);
  nu_assert("expected num_headers to be 2", request.num_headers == 0);
}

void test__http_request_free() {
  // Create a request
  HttpRequest request;
  http_request_init(&request);

  // Set the fields
  request.version = HTTP_VERSION_1_1;
  request.method = HTTP_METHOD_GET;
  request.uri = strdup("http://foo.com");
  http_request_add_header(&request);
  request.body = strdup("this is the request body");

  // Free the request and check all fields
  http_request_free(&request);
  nu_check("failed to initialize version", request.version == HTTP_VERSION_UNKNOWN);
  nu_check("failed to initialize method", request.method == HTTP_METHOD_UNKNOWN);
  nu_check("failed to initialize uri", request.uri == 0);
  nu_check("failed to initialize num_headers", request.num_headers == 0);
  nu_check("failed to initialize header_cap", request.header_cap == 0);
  nu_check("failed to initialize headers", request.headers == 0);
  nu_check("failed to initialize body", request.body == 0);
}

//==============================================================================
// Test-suite functions
//==============================================================================
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