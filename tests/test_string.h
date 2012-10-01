//==============================================================================
// String tests
//
// Evan Kuhn 2012-09-29
//==============================================================================
#ifndef TEST_STRNG_H
#define TEST_STRNG_H

#include "nu_unit.h"
#include "std_string.h"
#include <string.h>

//==============================================================================
// String tests
//==============================================================================
void test__string_new() {
  string* s = string_new();
  nu_check("string size should be 0", string_size(s) == 0);
  nu_check("string capacity should be 0", string_capacity(s) == 0);
  nu_check("string data should not be null", string_cstr(s) != NULL);
  nu_check("string should be empty", !strcmp(string_cstr(s), ""));
  string_free(s);
}

void test__string_set() {
  string* s = string_new();

  string_set(s, "abc");
  nu_check("string size should be 3", string_size(s) == 3);
  nu_check("string capacity should be >= 3", string_capacity(s) >= 3);
  nu_check("string should be 'abc'", !strcmp(string_cstr(s), "abc"));

  string_set(s, "0123456789");
  nu_check("string size should be 10", string_size(s) == 10);
  nu_check("string capacity should be >= 10", string_capacity(s) >= 10);
  nu_check("string should be '0123456789'", !strcmp(string_cstr(s), "0123456789"));

  string_set(s, "x");
  nu_check("string size should be 1", string_size(s) == 1);
  nu_check("string capacity should be >= 10", string_capacity(s) >= 10);
  nu_check("string should be 'x'", !strcmp(string_cstr(s), "x"));

  string_free(s);
}

void test__string_clear() {
  string* s = string_new();
  // Set to a non-empty string
  string_set(s, "abc");
  nu_check("string should be 'abc'", !strcmp(string_cstr(s), "abc"));
  // Then clear the string and make sure its empty
  string_clear(s);
  nu_check("string size should be 0", string_size(s) == 0);
  nu_check("string capacity should not be 0", string_capacity(s) >= 0);
  nu_check("string should be empty", !strcmp(string_cstr(s), ""));
  string_free(s);
}

void test__string_release() {
  string* s = string_new();
  // Set to a non-empty string
  string_set(s, "abc");
  nu_check("string should be 'abc'", !strcmp(string_cstr(s), "abc"));
  // Then release the string and make sure its empty
  string_release(s);
  nu_check("string size should be 0", string_size(s) == 0);
  nu_check("string capacity should be 0", string_capacity(s) == 0);
  nu_check("string should be empty", !strcmp(string_cstr(s), ""));
  string_free(s);
}

void test__string_reserve() {
  string* s = string_new();
  // Reserve 10
  string_reserve(s, 10);
  nu_check("string size should be 0", string_size(s) == 0);
  nu_check("string capacity should be >= 10", string_capacity(s) >= 10);
  nu_check("string should be empty", !strcmp(string_cstr(s), ""));
  // Set string of length 5
  string_set(s, "12345");
  nu_check("string size should be 5", string_size(s) == 5);
  nu_check("string capacity should be >= 10", string_capacity(s) >= 10);
  nu_check("string should be '12345'", !strcmp(string_cstr(s), "12345"));
  // Reserve 20
  string_reserve(s, 20);
  nu_check("string size should be 5", string_size(s) == 5);
  nu_check("string capacity should be >= 20", string_capacity(s) >= 20);
  nu_check("string should be '12345'", !strcmp(string_cstr(s), "12345"));
  // Try to reserve 1 ... fail
  string_reserve(s, 1);
  nu_check("string size should be 5", string_size(s) == 5);
  nu_check("string capacity should be >= 20", string_capacity(s) >= 20);
  nu_check("string should be '12345'", !strcmp(string_cstr(s), "12345"));
  // Reserve 5 ... should work
  string_reserve(s, 5);
  nu_check("string size should be 5", string_size(s) == 5);
  nu_check("string capacity should be 5", string_capacity(s) == 5);
  nu_check("string should be '12345'", !strcmp(string_cstr(s), "12345"));
  string_free(s);
}

void test__string_resize() {
  string* s = string_new();
  // Resize, adding spaces
  string_resize(s, 5);
  nu_check("string size should be 5", string_size(s) == 5);
  nu_check("string capacity should be >= 5", string_capacity(s) >= 5);
  nu_check("string should be '     '", !strcmp(string_cstr(s), "     "));
  // Set to something else
  string_set(s, "12345");
  nu_check("string size should be 5", string_size(s) == 5);
  nu_check("string capacity should be >= 5", string_capacity(s) >= 5);
  nu_check("string should be '12345'", !strcmp(string_cstr(s), "12345"));
  // Resize larger. Should add spaces.
  string_resize(s, 7);
  nu_check("string size should be 7", string_size(s) == 7);
  nu_check("string capacity should be >= 7", string_capacity(s) >= 7);
  nu_check("string should be '12345  '", !strcmp(string_cstr(s), "12345  "));
  // Resize smaller. Should not reduce capacity.
  string_resize(s, 2);
  nu_check("string size should be 2", string_size(s) == 2);
  nu_check("string capacity should be >= 7", string_capacity(s) >= 7);
  nu_check("string should be '12'", !strcmp(string_cstr(s), "12"));
  string_free(s);
}

// Not testing cstr(), size(), or capacity() since they're using in other tests

void test__string_empty() {
  string* s = string_new();
  nu_check("string should be empty", string_empty(s));
  string_set(s, "hello");
  nu_check("string should not be empty", !string_empty(s));
  string_clear(s);
  nu_check("string should be empty", string_empty(s));
  string_free(s);
}

void test__string_equal() {
  string* a = string_new();
  string* b = string_new();
  nu_check("empty strings should be equal", string_equal(a,b));
  string_set(a, "foo");
  nu_check("different strings should not equal", !string_equal(a,b));
  string_set(b, "FOO");
  nu_check("case should matter", !string_equal(a,b));
  string_set(b, "foo1");
  nu_check("entire strings must be compared", !string_equal(a,b));
  string_set(b, "foo");
  nu_check("equal strings should be equal", string_equal(a,b));
  string_reserve(a, 10);
  nu_check("capacity shouldn't matter", string_equal(a,b));
  nu_check("a string should be equal to itself", string_equal(a,a));
  string_free(a);
  string_free(b);
}

void test__string_equal_cstr() {
  string* s = string_new();
  nu_check("empty string should equal empty cstr", string_equal_cstr(s, ""));
  nu_check("empty string should not equal non-empty cstr", !string_equal_cstr(s, "x"));
  string_set(s, "asdf");
  nu_check("equal strings should match", string_equal_cstr(s, "asdf"));
  nu_check("case should matter", !string_equal_cstr(s, "Asdf"));
  nu_check("entire string must match", !string_equal_cstr(s, "asd"));
  nu_check("entire string must match", !string_equal_cstr(s, "asdff"));
  string_free(s);
}

void test__string_append_char() {
  string* s = string_new();
  string_append_char(s, 'x');
  nu_check("char 'x' should be appended", string_equal_cstr(s, "x"));
  nu_check("size should be 1", string_size(s) == 1);
  string_append_char(s, 'y');
  nu_check("char 'y' should be appended", string_equal_cstr(s, "xy"));
  nu_check("size should be 2", string_size(s) == 2);
  string_append_char(s, '\0');
  nu_check("null terminator should not change string", string_equal_cstr(s, "xy"));
  nu_check("null terminator should not change size", string_size(s) == 2);
  string_free(s);
}

void test__string_append_cstr() {
  string* s = string_new();
  string_append_cstr(s, "abc");
  nu_check("string 'abc' should be appended", string_equal_cstr(s, "abc"));
  nu_check("size should be 3", string_size(s) == 3);
  string_append_cstr(s, "123");
  nu_check("string '123' should be appended", string_equal_cstr(s, "abc123"));
  nu_check("size should be 6", string_size(s) == 6);
  string_append_cstr(s, "x\0y");
  nu_check("only 'x' should be appended", string_equal_cstr(s, "abc123x"));
  nu_check("size should be 7", string_size(s) == 7);
  string_free(s);
}

// TODO - finish tests below

void test__string_append_cstrn() {
  string* s = string_new();
  // Should work when n == string length
  // Should work when n < string length
  // Should work when n > string length
  string_free(s);
}

void test__string_append_str() {
  string* s = string_new();
  string_free(s);
}

void test__string_trim() {
  string* s = string_new();
  string_free(s);
}

void test__string_ltrim() {
  string* s = string_new();
  string_free(s);
}

void test__string_rtrim() {
  string* s = string_new();
  string_free(s);
}

void test__string_upcase() {
  string* s = string_new();
  string_free(s);
}

void test__string_downcase() {
  string* s = string_new();
  string_free(s);
}

//==============================================================================
// Test suites
//==============================================================================
void test_suite__string() {
  nu_run_test(test__string_new, "string_new()");
  nu_run_test(test__string_set, "string_set()");
  nu_run_test(test__string_clear, "string_clear()");
  nu_run_test(test__string_release, "string_release()");
  nu_run_test(test__string_reserve, "string_reserve()");
  nu_run_test(test__string_resize, "string_resize()");
  nu_run_test(test__string_empty, "string_empty()");
  nu_run_test(test__string_equal, "string_equal()");
  nu_run_test(test__string_equal_cstr, "string_equal_cstr()");
  nu_run_test(test__string_append_char, "string_append_char()");
  nu_run_test(test__string_append_cstr, "string_append_cstr()");


  nu_run_test(test__string_append_cstrn, "string_append_cstrn()");
  nu_run_test(test__string_append_str, "string_append_str()");

  nu_run_test(test__string_trim, "string_trim()");
  nu_run_test(test__string_ltrim, "string_ltrim()");
  nu_run_test(test__string_rtrim, "string_rtrim()");

  nu_run_test(test__string_upcase, "string_upcase()");
  nu_run_test(test__string_downcase, "string_downcase()");
}

#endif // TEST_STRNG_H