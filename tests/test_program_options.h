#ifndef TEST_PROGRAM_OPTIONS_H
#define TEST_PROGRAM_OPTIONS_H

#include "minunit.h"
#include "program_options.h"

static char* test_something() {
  ProgramOptions options;
  int argc = 1;
  char* argv[1];
  argv[0] = "my_program";
  parse_options(argc, argv, &options);
  mu_assert("parse_options() didn't set default port", options.port == 80);
  return 0;
}

char* test_program_options() {
  char* msg = NULL;
  mu_run_test(test_something);
  return msg;
}

#endif // TEST_PROGRAM_OPTIONS_H