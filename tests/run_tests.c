//==============================================================================
// Entry point for unit tests
//
// Evan Kuhn 2012-09-09
//==============================================================================
#include "nu_unit.h"
#include "test_http_request.h"
#include "test_program_options.h"
#include "test_sockets.h"
#include "test_utils.h"

nu_init();

int main(int argc, char **argv) {
  nu_output_level_suites();

  // Run all test suites
  nu_run_suite(test_suite__http_enums,      "HTTP Enums");
  nu_run_suite(test_suite__http_header,     "HttpHeader");
  nu_run_suite(test_suite__http_request,    "HttpRequest");
  nu_run_suite(test_suite__program_options, "ProgramOptions");
  nu_run_suite(test_suite__client_socket,   "ClientSocket");
  nu_run_suite(test_suite__server_socket,   "ServerSocket");
  nu_run_suite(test_suite__utils,           "Utils");

  // Print results and return
  nu_print_summary();
  nu_exit();
}
