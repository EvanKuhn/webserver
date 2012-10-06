//==============================================================================
// ProgramOptions tests
//
// Evan Kuhn 2012-09-09
//==============================================================================
#ifndef TEST_PROGRAM_OPTIONS_H
#define TEST_PROGRAM_OPTIONS_H

#include <stdlib.h>
#include "nu_unit.h"
#include "program_options.h"

// Helper function: given an array of strings, free them
void free_strings(char** strs, int num_strs) {
  for(int i=0; i<num_strs; ++i) {
    free(strs[i]);
  }
}

//==============================================================================
// ProgramOptions tests
//==============================================================================
void test__program_options_parse__sets_defaults() {
  ProgramOptions options;
  int argc = 1;
  char* argv[1] = { strdup("webserver") };
  bool status = program_options_parse(&options, argc, argv);
  free_strings(argv, 1);
  nu_check("should succeed given no args", status);
  nu_check("didn't set default port", options.config.port == 80);
  nu_check("didn't set verbose off", !options.config.verbose);
  nu_check("didn't set help off", !options.help);
}

void test__program_options_parse__parses_port() {
  ProgramOptions options;
  int argc = 3;
  char* argv[3] = { strdup("webserver"), strdup("-p"), strdup("1234") };
  bool status = program_options_parse(&options, argc, argv);
  free_strings(argv, 3);
  nu_check("should succeed given a port", status);
  nu_check("didn't parse port", options.config.port == 1234);
}

void test__program_options_parse__requires_port_arg() {
  ProgramOptions options;
  int argc = 2;
  char* argv[2] = { strdup("webserver"), strdup("-p") };
  bool status = program_options_parse(&options, argc, argv);
  free_strings(argv, 2);
  nu_check("should fail if not given port arg", status == false);
}

void test__program_options_parse__parses_verbose() {
  ProgramOptions options;
  int argc = 2;
  char* argv[2] = { strdup("webserver"), strdup("-v") };
  bool status = program_options_parse(&options, argc, argv);
  free_strings(argv, 2);
  nu_check("should succeed given verbose flag", status);
  nu_check("didn't parse verbose flag", options.config.verbose);
}

void test__program_options_parse__parses_echo() {
  ProgramOptions options;
  int argc = 2;
  char* argv[2] = { strdup("webserver"), strdup("-e") };
  bool status = program_options_parse(&options, argc, argv);
  free_strings(argv, 2);
  nu_check("should succeed given echo flag", status);
  nu_check("didn't parse echo flag", options.config.echo);
}

void test__program_options_parse__supports_help() {
  ProgramOptions options;
  int argc = 2;
  char* argv[2] = { strdup("webserver"), strdup("-h") };
  bool status = program_options_parse(&options, argc, argv);
  free_strings(argv, 2);
  nu_check("should accept -h", status);
  nu_check("should set help flag to true", options.help);
}

//==============================================================================
// Test suite
//==============================================================================
void test_suite__program_options() {
  // Suppress output from program_options_parse() before running tests
  silence_program_options_parse = true;
  nu_run_test(test__program_options_parse__sets_defaults,     "program_options_parse() sets defaults");
  nu_run_test(test__program_options_parse__parses_port,       "program_options_parse() parses port");
  nu_run_test(test__program_options_parse__requires_port_arg, "program_options_parse() requires port arg");
  nu_run_test(test__program_options_parse__parses_verbose,    "program_options_parse() parses verbose");
  nu_run_test(test__program_options_parse__parses_echo,       "program_options_parse() parses echo");
  nu_run_test(test__program_options_parse__supports_help,     "program_options_parse() supports help");
}

#endif // TEST_PROGRAM_OPTIONS_H