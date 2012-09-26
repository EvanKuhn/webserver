//==============================================================================
// HttpRequest tests
//
// Evan Kuhn 2012-09-26
//==============================================================================
#ifndef TEST_HTTP_RESPONSE_H
#define TEST_HTTP_RESPONSE_H

#include "nu_unit.h"
#include "http_response.h"

void test__http_response_new() {
  HttpResponse* res = http_response_new();
  nu_check("returned null pointer", res);
  nu_check("request string should be empty", !strcmp(http_response_string(res),""));
}

void test__http_response_set_status() {
  HttpResponse* res = http_response_new();
  http_response_set_status(res, HTTP_VERSION_1_1, HTTP_STATUS_OK);
  const char* text = http_response_string(res);
  const size_t len = http_response_length(res);
  nu_check("response string incorrect", !strcmp(text, "HTTP/1.1 200 OK\r\n"));
  nu_check("response length incorrect", len == strlen(text));
}

void test__http_response_add_header() {
  HttpResponse* res = http_response_new();
  http_response_set_status(res, HTTP_VERSION_1_1, HTTP_STATUS_OK);
  http_response_add_header(res, "Server", "webserver");
  http_response_add_header(res, "Accept", "*/*");
  const char* text = http_response_string(res);
  const size_t len = http_response_length(res);
  const char* expect =
    "HTTP/1.1 200 OK\r\n"
    "Server: webserver\r\n"
    "Accept: */*\r\n";
  nu_check("response string incorrect", !strcmp(text, expect));
  nu_check("response length incorrect", len == strlen(text));
}

// Set the HTTP body. Not required for all responses.
// - Make sure you set the Content-Length header if setting a body!
void test__http_response_set_body() {
  HttpResponse* res = http_response_new();
  http_response_set_status(res, HTTP_VERSION_1_1, HTTP_STATUS_OK);
  http_response_add_header(res, "Server", "webserver");
  http_response_add_header(res, "Accept", "*/*");
  http_response_set_body(res, "I like programming!");
  const char* text = http_response_string(res);
  const size_t len = http_response_length(res);
  const char* expect =
    "HTTP/1.1 200 OK\r\n"
    "Server: webserver\r\n"
    "Accept: */*\r\n"
    "\r\n"
    "I like programming!";
  nu_check("response string incorrect", !strcmp(text, expect));
  nu_check("response length incorrect", len == strlen(text));
}

void test__http_response_clear() {
  HttpResponse* res = http_response_new();
  http_response_set_status(res, HTTP_VERSION_1_1, HTTP_STATUS_OK);
  const char* text = http_response_string(res);
  size_t len = http_response_length(res);
  nu_assert("response string should not be empty", strlen(text) > 0);
  http_response_clear(res);
  text = http_response_string(res);
  len = http_response_length(res);
  nu_assert("response string should not be null", text);
  nu_assert("response string should be empty", strlen(text) == 0);
  nu_check("response length incorrect", len == strlen(text));
}

//==============================================================================
// Test-suite functions
//==============================================================================
void test_suite__http_response() {
  nu_run_test(test__http_response_new,        "http_response_new()");
  nu_run_test(test__http_response_set_status, "http_response_set_status()");
  nu_run_test(test__http_response_add_header, "http_response_add_header()");
  nu_run_test(test__http_response_set_body,   "http_response_set_body()");
  nu_run_test(test__http_response_clear,      "http_response_clear()");
}

#endif // TEST_HTTP_RESPONSE_H