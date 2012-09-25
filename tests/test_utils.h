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

void test__string_buffer_init() {
  StringBuffer buf;
  const size_t capacity = 16;
  string_buffer_init(&buf, capacity);
  nu_check("didn't initialize the char buffer", buf.chars);
  nu_check("didn't initialize the buffer size", buf.size == 0);
  nu_check("didn't initialize the buffer capacity", buf.cap == capacity);
  for(size_t i=0; i<capacity; ++i) {
    if(buf.chars[i] != 0) nu_abort("didn't zero-out the char buffer");
  }
}

void test__string_buffer_free() {
  StringBuffer buf;
  const size_t capacity = 16;
  string_buffer_init(&buf, capacity);
  buf.size = 2;
  string_buffer_free(&buf);
  nu_check("didn't reset the char buffer", !buf.chars);
  nu_check("didn't reset the buffer size", buf.size == 0);
  nu_check("didn't reset the buffer capacity", buf.cap == 0);
}

void test__string_buffer_append() {
  StringBuffer buf;
  const size_t capacity = 16;
  string_buffer_init(&buf, capacity);

  const char* phrase = "hello there!";
  const size_t phrase_len = strlen(phrase);

  // Append once
  string_buffer_append(&buf, phrase);
  nu_check("didn't set the char buffer", !strcmp(buf.chars, phrase));
  nu_check("didn't set the buffer size", buf.size == phrase_len);
  nu_check("didn't set the buffer capacity", buf.cap == capacity);

  // Then append again, so the buffer must be resized
  string_buffer_append(&buf, phrase);
  nu_check("didn't update the char buffer", !strcmp(buf.chars, "hello there!hello there!"));
  nu_check("didn't update the buffer size", buf.size == 2*phrase_len);
  nu_check("didn't update the buffer capacity", buf.cap > capacity);
  string_buffer_free(&buf);
}

//==============================================================================
// Test suites
//==============================================================================
void test_suite__utils() {
  nu_run_test(test__safe_cstr, "safe_cstr()");
  nu_run_test(test__trim,      "trim()");
}

void test_suite__string_buffer() {
  nu_run_test(test__string_buffer_init,   "string_buffer_init()");
  nu_run_test(test__string_buffer_free,   "string_buffer_free()");
  nu_run_test(test__string_buffer_append, "string_buffer_append()");
}

#endif // TEST_UTILS_H