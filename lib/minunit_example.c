#include "minunit.h"

//==============================================================================
// Example test functions
//==============================================================================
void test_foo() {
  int foo = 1;
  nu_assert("foo isn't 1", foo == 1);
}

void test_bar() {
  int bar = 2;
  nu_assert("bar isn't 2", bar == 2);
}

void test_bat() {
  nu_not_implemented();
}

//==============================================================================
// Example test suite
//==============================================================================
void test_suite1() {
  nu_run_test(test_foo, "foo");
  nu_run_test(test_bar, "bar");
  nu_run_test(test_bat, "bat");
}

void test_suite2() {
  nu_run_test(test_foo, "foo");
  nu_run_test(test_bar, "bar");
}

//==============================================================================
// Example main.c
//==============================================================================
nu_init();

int main(int argc, char **argv) {
  // Run test suites
  nu_run_suite(test_suite1, "suite 1");
  nu_run_suite(test_suite2, "suite 2");
  // add more test suites here...

  // Print results and return
  nu_print_summary();
  return nu_return_status();
}

