#include "minunit.h"

//==============================================================================
// Example test functions
//==============================================================================
void test_foo() {
  int foo = 1;
  mu_assert("foo isn't 1", foo == 1);
}

void test_bar() {
  int bar = 2;
  mu_assert("bar isn't 2", bar == 2);
}

void test_bat() {
  mu_not_implemented();
}

//==============================================================================
// Example test suite
//==============================================================================
void test_suite1() {
  mu_run_test(test_foo, "foo");
  mu_run_test(test_bar, "bar");
  mu_run_test(test_bat, "bat");
}

void test_suite2() {
  mu_run_test(test_foo, "foo");
  mu_run_test(test_bar, "bar");
}

//==============================================================================
// Example main.c
//==============================================================================
mu_init();

int main(int argc, char **argv) {
  // Run test suites
  mu_run_suite(test_suite1, "suite 1");
  mu_run_suite(test_suite2, "suite 2");
  // add more test suites here...

  // Print results and return
  mu_print_summary();
  return mu_return_status();
}

