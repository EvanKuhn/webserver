//==============================================================================
// Entry point for unit tests
//
// Evan Kuhn 2012-09-09
//==============================================================================
#include <stdio.h>
#include <string.h>
#include "minunit.h"
#include "test_http_request.h"
#include "test_program_options.h"
#include "test_sockets.h"
#include "test_utils.h"

mu_init();

int main(int argc, char **argv) {
  // Run all test suites
  mu_run_suite(test_suite__http_enums,      "HTTP Enums");
  mu_run_suite(test_suite__http_header,     "HttpHeader");
  mu_run_suite(test_suite__http_request,    "HttpRequest");
  mu_run_suite(test_suite__program_options, "ProgramOptions");
  mu_run_suite(test_suite__client_socket,  "ClientSocket");
  mu_run_suite(test_suite__server_socket,  "ServerSocket");
  mu_run_suite(test_suite__utils,           "Utils");

  // Print results and return
  mu_print_summary();
  return mu_return_status();
}

  //TODO - at end of tests, check for and kill all child processes