#ifndef TEST_PROGRAM_OPTIONS_H
#define TEST_PROGRAM_OPTIONS_H

#include "minunit.h"
#include "program_options.h"

static char* program_options_sets_defaults() {
  puts("program_options_sets_defaults");
  ProgramOptions options;
  int argc = 1;
  char* argv[1] = { "webserver" };
  bool status = parse_options(argc, argv, &options);
  mu_assert("should succeed given no args", status);
  mu_assert("didn't set default port", options.port == 80);
  return 0;
}

static char* program_options_parses_port() {
  puts("program_options_parses_port");
  ProgramOptions options;
  int argc = 3;
  char* argv[3] = { "webserver", "-p", "1234" };
  bool status = parse_options(argc, argv, &options);
  mu_assert("should succeed given a port", status);
  mu_assert("didn't parse port", options.port == 1234);
  return 0;
}

static char* program_options_requires_port_arg() {
  puts("program_options_requires_port_arg");
  ProgramOptions options;
  int argc = 2;
  char* argv[2] = { "webserver", "-p" };
  bool status = parse_options(argc, argv, &options);
  mu_assert("should fail if not given port arg", status == false);
  return 0;
}

static char* program_options_supports_help() {
  puts("program_options_supports_help");
  ProgramOptions options;
  int argc = 2;
  char* argv[2] = { "webserver", "-h" };
  bool status = parse_options(argc, argv, &options);
  mu_assert("should accept -h", status);
  mu_assert("should set help flag to true", options.help);
  return 0;
}

char* test_program_options() {
  silence_parse_options = true;
  char* msg = NULL;
  printf("\n");
  mu_run_test(program_options_sets_defaults);
  mu_run_test(program_options_parses_port);
  mu_run_test(program_options_requires_port_arg);
  mu_run_test(program_options_supports_help);
  return msg;
}

#endif // TEST_PROGRAM_OPTIONS_H