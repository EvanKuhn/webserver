#include <stdio.h>
#include <string.h>
#include "minunit.h"
#include "test_program_options.h"
#include "test_sockets.h"

int tests_run = 0;

int main(int argc, char **argv) {
  // Run all tests
  char* msg = NULL;
  if(!msg) msg = test_program_options();
  if(!msg) msg = test_sockets();

  // Print results
  if(msg) printf("- %s\n\nTEST FAILED\n", msg);
  else    printf("\nALL TESTS PASSED\n");
  printf("Tests run: %d\n", tests_run);

  // Exit with success or failure
  return msg != 0;
}
