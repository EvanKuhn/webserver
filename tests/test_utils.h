//==============================================================================
// Utils tests
//
// Evan Kuhn 2012-09-18
//==============================================================================
#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include "nu_unit.h"
#include "utils.h"
#include <sys/types.h>

//==============================================================================
// Tests
//==============================================================================
void test__safe_cstr() {
  nu_check("didn't return the input string", !strcmp(safe_cstr("asdf"), "asdf"));
  nu_check("didn't return empty string when given NULL", !strcmp(safe_cstr(NULL), ""));
}

void test__trim() {
  char buf[6];
  memcpy(buf, "  foo", 6);
  nu_check("didn't trim leading spaces", !strcmp(trim(buf), "foo"));
  memcpy(buf, "foo  ", 6);
  nu_check("didn't trim trailing spaces", !strcmp(trim(buf), "foo"));
  memcpy(buf, " foo ", 6);
  nu_check("didn't trim surrounding spaces", !strcmp(trim(buf), "foo"));
}

//==============================================================================
// Test suites
//==============================================================================
void test_suite__utils() {
  nu_run_test(test__safe_cstr, "safe_cstr()");
  nu_run_test(test__trim,      "trim()");
}

#endif // TEST_UTILS_H