#include "minunit.h"

//==============================================================================
// Example test functions
//==============================================================================
void test_foo() {
  printf("test: foo\n");
  int foo = 1;
  mu_assert("foo isn't 1", foo == 1);
}

void test_bar() {
  printf("test: bar\n");
  int bar = 2;
  mu_assert("bar isn't 2", bar == 2);
}

void test_bat() {
  printf("test: bar\n");
  mu_not_implemented();
}

//==============================================================================
// Example test suite
//==============================================================================
void test_suite() {
  mu_run_test(test_foo);
  mu_run_test(test_bar);
  mu_run_test(test_bat);
}

void test_suite2() {
  mu_run_test(test_foo);
  mu_run_test(test_bar);
  mu_run_test(test_bat);
}

//==============================================================================
// Example main.c
//==============================================================================
mu_init();

int main(int argc, char **argv) {
  // Run test suites
  mu_run_suite(test_suite);
  mu_run_suite(test_suite2);
  // add more test suites here...

  // Print results
  mu_print_summary();

  // Exit with success or failure
  return mu_return_status();
}

