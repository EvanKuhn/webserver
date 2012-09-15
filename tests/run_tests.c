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

int tests_run = 0;

int main(int argc, char **argv) {
  // Run all tests
  char* msg = NULL;
  if(!msg) msg = test_http_request();
  if(!msg) msg = test_program_options();
  if(!msg) msg = test_sockets();

  // Print results
  if(msg) printf("- %s\n\nTEST FAILED\n", msg);
  else    printf("\nALL TESTS PASSED\n");
  printf("Tests run: %d\n", tests_run);

  //TODO - at end of tests, check for and kill all child processes

  // Exit with success or failure
  return msg != 0;
}
