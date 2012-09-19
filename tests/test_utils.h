//==============================================================================
// Utils tests
//
// Evan Kuhn 2012-09-18
//==============================================================================
#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include "minunit.h"
#include "utils.h"

//==============================================================================
// Utility function tests
//==============================================================================
const char* test__safe_cstr() {
  puts("test: test__safe_cstr()");
  mu_assert("didn't return the input string", !strcmp(safe_cstr("asdf"), "asdf"));
  mu_assert("didn't return empty string when given NULL", !strcmp(safe_cstr(NULL), ""));
  return 0;
}

const char* test__trim() {
  puts("test: test__trim()");
  char buf[6];
  memcpy(buf, "  foo", 6);
  mu_assert("didn't trim leading spaces", !strcmp(trim(buf), "foo"));
  memcpy(buf, "foo  ", 6);
  mu_assert("didn't trim trailing spaces", !strcmp(trim(buf), "foo"));
  memcpy(buf, " foo ", 6);
  mu_assert("didn't trim surrounding spaces", !strcmp(trim(buf), "foo"));
  return 0;
}

const char* test__strsep_trim() {
  puts("test: test__strsep_trim()");
  mu_not_implemented(); //TODO
  return 0;
}

//==============================================================================
// Test-all function
//==============================================================================
const char* test_utils() {
  printf("\n");
  mu_run_test(test__safe_cstr);
  mu_run_test(test__trim);
  mu_run_test(test__strsep_trim);
  return 0;
}

#endif // TEST_UTILS_H