//==============================================================================
// HttpRequest tests
//
// Evan Kuhn 2012-09-09
//==============================================================================
#ifndef TEST_HTTP_REQUEST_H
#define TEST_HTTP_REQUEST_H

#include "minunit.h"
#include "http_request.h"

//==============================================================================
// HttpHeader
//==============================================================================
static char* test__http_header_init() {
  puts("test: http_header_init()");
  HttpHeader h;
  http_header_init(&h);
  mu_assert("failed to initialize key to NULL", h.key == NULL);
  mu_assert("failed to initialize val to NULL", h.val == NULL);
  return 0;
}

//==============================================================================
// HttpRequest
//==============================================================================

//==============================================================================
// Test-all function
//==============================================================================
char* test_http_request() {
  char* msg = NULL;

  printf("\n");
  mu_run_test(test__http_header_init);

  return msg;
}

#endif // TEST_HTTP_REQUEST_H