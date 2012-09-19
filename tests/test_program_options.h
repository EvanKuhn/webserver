//==============================================================================
// ProgramOptions tests
//
// Evan Kuhn 2012-09-09
//==============================================================================
#ifndef TEST_PROGRAM_OPTIONS_H
#define TEST_PROGRAM_OPTIONS_H

#include "minunit.h"
#include "program_options.h"

static char* test__program_options_parse__sets_defaults() {
  puts("test: program_options_parse() sets defaults");
  ProgramOptions options;
  int argc = 1;
  char* argv[1] = { "webserver" };
  bool status = program_options_parse(&options, argc, argv);
  mu_assert("should succeed given no args", status);
  mu_assert("didn't set default port", options.port == 80);
  return 0;
}

static char* test__program_options_parse__parses_port() {
  puts("test: program_options_parse() parses port");
  ProgramOptions options;
  int argc = 3;
  char* argv[3] = { "webserver", "-p", "1234" };
  bool status = program_options_parse(&options, argc, argv);
  mu_assert("should succeed given a port", status);
  mu_assert("didn't parse port", options.port == 1234);
  return 0;
}

static char* test__program_options_parse__requires_port_arg() {
  puts("test: program_options_parse() requires port arg");
  ProgramOptions options;
  int argc = 2;
  char* argv[2] = { "webserver", "-p" };
  bool status = program_options_parse(&options, argc, argv);
  mu_assert("should fail if not given port arg", status == false);
  return 0;
}

static char* test__program_options_parse__supports_help() {
  puts("test: program_options_parse() supports help");
  ProgramOptions options;
  int argc = 2;
  char* argv[2] = { "webserver", "-h" };
  bool status = program_options_parse(&options, argc, argv);
  mu_assert("should accept -h", status);
  mu_assert("should set help flag to true", options.help);
  return 0;
}

// Run all tests
char* test_program_options() {
  silence_program_options_parse = true;
  printf("\n");
  mu_run_test(test__program_options_parse__sets_defaults);
  mu_run_test(test__program_options_parse__parses_port);
  mu_run_test(test__program_options_parse__requires_port_arg);
  mu_run_test(test__program_options_parse__supports_help);
  return 0;
}

#endif // TEST_PROGRAM_OPTIONS_H