#include "core/string.h"
#include "minunit.h"
#include <stdio.h>
#include <string.h>

int tests_run = 0;

static char* test_new_creates_empty_string() {
  string* str = string_new();
  mu_assert("string_new() didn't create empty string", string_size(str) == 0);
  return 0;
}

static char* test_string_set() {
  string* str = string_new();
  string_set(str, "");
  mu_assert("string_set() fails to set empty string", strcmp(string_cstr(str), "") == 0);
  mu_assert("string_set() fails to set empty string size", string_size(str) == 0);
  string_set(str, "hello there");
  mu_assert("string_set() fails to set string", strcmp(string_cstr(str), "hello there") == 0);
  mu_assert("string_set() fails to set string size", string_size(str) == 11);
  string_set(str, "hi");
  mu_assert("string_set() fails to set shorter string", strcmp(string_cstr(str), "hi") == 0);
  mu_assert("string_set() fails to set shorter string_size", string_size(str) == 2);
  return 0;
}

static char* test_string_clear() {
  string* str = string_new();
  string_clear(str);
  mu_assert("string_clear() fails to set size on empty string", string_size(str) == 0);
  string_set(str, "asdf");
  string_clear(str);
  mu_assert("string_clear() fails to set size on non-empty string", string_size(str) == 0);
  mu_assert("string_clear() sets capacity to 0 on non-empty string", string_capacity(str) > 0);
  return 0;
}

static char* test_string_release() {
  string* str = string_new();
  string_release(str);
  mu_assert("string_clear() fails to set size on empty string", string_size(str) == 0);
  mu_assert("string_clear() fails to set capacity on empty string", string_capacity(str) == 0);
  string_set(str, "asdf");
  string_release(str);
  mu_assert("string_clear() fails to set size on string", string_size(str) == 0);
  mu_assert("string_clear() fails to set capacity on string", string_capacity(str) == 0);
  return 0;
}

static char* test_string_resize() {
  string* str = string_new();
  string_resize(str, 4);
  mu_assert("string_resize() fails to set size on empty string", string_size(str) == 4);
  mu_assert("string_resize() fails to set capacity on empty string", string_capacity(str) >= 4);
  mu_assert("string_resize() fails to set data for empty string", string_equal_cstr(str, "    "));
  string_resize(str, 10);
  mu_assert("string_resize() fails to grow size of string", string_size(str) == 10);
  mu_assert("string_resize() fails to grow capacity of string", string_capacity(str) >= 10);
  mu_assert("string_resize() fails to set data for string", string_equal_cstr(str, "          "));
  string_resize(str, 1);
  mu_assert("string_resize() fails to shrink size of string", string_size(str) == 1);
  mu_assert("string_resize() shouldn't shrink capacity of string", string_capacity(str) >= 10);
  mu_assert("string_resize() fails to shrink data for string", string_equal_cstr(str, " "));
  return 0;
}

static char* test_string_cstr() {
  string* str = string_new();
  mu_assert("string_cstr() fails for empty string", strcmp(string_cstr(str), "") == 0);
  string_set(str, "hello there");
  mu_assert("string_cstr() fails for string", strcmp(string_cstr(str), "hello there") == 0);
  string_set(str, "hi");
  mu_assert("string_cstr() fails for string", strcmp(string_cstr(str), "hi") == 0);
  return 0;
}

static char* test_string_size() {
  string* str = string_new();
  mu_assert("string_size() fails for new string", string_size(str) == 0);
  string_set(str, "i like hamburgers");
  mu_assert("string_size() fails for non-empty string", string_size(str) == 17);
  string_set(str, "abc");
  mu_assert("string_size() fails for shorter string", string_size(str) == 3);
  string_set(str, "");
  mu_assert("string_size() fails for empty string", string_size(str) == 0);
  return 0;
}

static char* test_string_capacity() {
  string* str = string_new();
  string_set(str, "asdf");
  mu_assert("string capacity is too small", string_capacity(str) >= 4);
  string_set(str, "i like hamburgers");
  mu_assert("string capacity didn't grow", string_capacity(str) >= 17);
  string_set(str, "");
  mu_assert("string capacity shouldn't shrink capacity", string_capacity(str) >= 17);
  return 0;
}

static char* test_string_empty() {
  string* str = string_new();
  mu_assert("string_empty() fails for new string", string_empty(str));
  string_set(str, "asdf");
  mu_assert("string_empty() fails for non-empty string", !string_empty(str));
  string_set(str, "");
  mu_assert("string_empty() fails for empty string", string_empty(str));
  return 0;
}

static char* test_string_equal() {
  string* a = string_new();
  string* b = string_new();
  mu_assert("string_equal() fails for new strings", string_equal(a, b));

  string_set(a, "asdf");
  string_set(b, "asdf");
  mu_assert("string_equal() fails for equal strings", string_equal(a, b));

  string_set(a, "foo");
  string_set(b, "bar");
  mu_assert("string_equal() fails for unequal strings", !string_equal(a, b));

  string_set(a, "");
  string_set(b, "");
  mu_assert("string_equal() fails for empty strings", string_equal(a, b));

  return 0;
}

static char* test_string_equal_cstr() {
  string* a = string_new();
  mu_assert("string_equal() fails for new strings", string_equal_cstr(a, ""));
  string_set(a, "foo");
  mu_assert("string_equal() fails for equal strings", string_equal_cstr(a, "foo"));
  mu_assert("string_equal() fails for unequal strings", !string_equal_cstr(a, "bar"));
  string_set(a, "");
  mu_assert("string_equal() fails for empty strings", string_equal_cstr(a, ""));
  return 0;
}

static char* test_string_append_char() {
  string* str = string_new();
  string_append_char(str, 'a');
  mu_assert("string_append() fails for new string", string_equal_cstr(str, "a"));
  string_append_char(str, 'b');
  mu_assert("string_append() fails", string_equal_cstr(str, "ab"));
  return 0;
}

static char* test_string_append_cstr() {
  string* str = string_new();
  string_append_cstr(str, "hello");
  mu_assert("string_append_cstr() fails for new string", string_equal_cstr(str, "hello"));
  string_append_cstr(str, " world");
  mu_assert("string_append_cstr() fails", string_equal_cstr(str, "hello world"));
  return 0;
}

static char* test_string_append_cstrn() {
  string* str = string_new();
  string_append_cstrn(str, "hello", 5);
  mu_assert("string_append_cstrn() fails for new string", string_equal_cstr(str, "hello"));
  string_append_cstrn(str, " world", 6);
  mu_assert("string_append_cstrn() fails", string_equal_cstr(str, "hello world"));
  string_append_cstrn(str, "!!!!!", 2);
  mu_assert("string_append_cstrn() fails when cstr is longer", string_equal_cstr(str, "hello world!!"));
  return 0;
}

static char* test_string_append_str() {
  string* str = string_new();
  string* suffix = string_new();
  string_set(suffix, "hello");
  string_append_str(str, suffix);
  mu_assert("string_append_str() fails for new string", string_equal_cstr(str, "hello"));
  string_append_str(str, suffix);
  mu_assert("string_append_str() fails", string_equal_cstr(str, "hellohello"));
  return 0;
}

static char* test_string_trim() {
  string* str = string_new();
  string_clear(str); //TODO
  return 0;
}

static char* test_string_ltrim() {
  string* str = string_new();
  string_clear(str); //TODO
  return 0;
}

static char* test_string_rtrim() {
  string* str = string_new();
  string_clear(str); //TODO
  return 0;
}

static char* test_string_upcase() {
  string* str = string_new();
  string_upcase(str);
  mu_assert("string_upcase() fails for new string", string_equal_cstr(str, ""));
  string_set(str, "fooBAR");
  string_upcase(str);
  mu_assert("string_upcase() fails for letters", string_equal_cstr(str, "FOOBAR"));
  string_set(str, "!@#$%^&*()1234567890");
  string_upcase(str);
  mu_assert("string_upcase() fails for non-letters", string_equal_cstr(str, "!@#$%^&*()1234567890"));
  return 0;
}

static char* test_string_downcase() {
  string* str = string_new();
  string_downcase(str);
  mu_assert("string_downcase() fails for new string", string_equal_cstr(str, ""));
  string_set(str, "fooBAR");
  string_downcase(str);
  mu_assert("string_downcase() fails for letters", string_equal_cstr(str, "foobar"));
  string_set(str, "!@#$%^&*()1234567890");
  string_downcase(str);
  mu_assert("string_downcase() fails for non-letters", string_equal_cstr(str, "!@#$%^&*()1234567890"));
  return 0;
}

static char* run_all_tests() {
  mu_run_test(test_new_creates_empty_string);
  mu_run_test(test_string_set);
  mu_run_test(test_string_clear);
  mu_run_test(test_string_release);
  mu_run_test(test_string_resize);
  mu_run_test(test_string_cstr);
  mu_run_test(test_string_size);
  mu_run_test(test_string_capacity);
  mu_run_test(test_string_empty);
  mu_run_test(test_string_equal);
  mu_run_test(test_string_equal_cstr);
  mu_run_test(test_string_append_char);
  mu_run_test(test_string_append_cstr);
  mu_run_test(test_string_append_cstrn);
  mu_run_test(test_string_append_str);
  mu_run_test(test_string_trim);
  mu_run_test(test_string_ltrim);
  mu_run_test(test_string_rtrim);
  mu_run_test(test_string_upcase);
  mu_run_test(test_string_downcase);
  return 0;
}

int main(int argc, char **argv) {
  char *result = run_all_tests();
  if(result != 0) {
    printf("%s\n", result);
  }
  else {
    printf("ALL TESTS PASSED\n");
  }
  printf("Tests run: %d\n", tests_run);

  return result != 0;
}
