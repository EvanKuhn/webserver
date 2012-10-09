//==============================================================================
// Utils tests
//
// Evan Kuhn 2012-09-18
//==============================================================================
#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include "nu_unit.h"
#include "utils.h"
#include <ctype.h>
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

void test__timestamp() {
  // Expected format: "20121008-15:32:46 UTC"
  timebuf_t tb;
  const char* str = timestamp(tb);
  nu_check("should return pointer to input struct", str == tb);
  nu_check("timestamp should have length 21", strlen(tb) == 21);
  bool digits_ok = true;
  const int digit_indexes[14] = {0,1,2,3,4,5,6,7,9,10,12,13,15,16};
  for(int i=0; i<14 && digits_ok; ++i) {
    digits_ok = isdigit(str[digit_indexes[i]]);
  }
  nu_check("should have digits in expected locations", digits_ok);
}

//==============================================================================
// Test suites
//==============================================================================
void test_suite__utils() {
  nu_run_test(test__safe_cstr, "safe_cstr()");
  nu_run_test(test__trim,      "trim()");
  nu_run_test(test__timestamp, "timestamp()");
}

#endif // TEST_UTILS_H