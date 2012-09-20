#include "minunit.h"

//==============================================================================
// Example test functions
//==============================================================================
const char* test_foo() {
  printf("test: foo\n");
  int foo = 1;
  mu_assert("foo isn't 1", foo == 1);
  return 0;
}

const char* test_bar() {
  printf("test: bar\n");
  int bar = 2;
  mu_assert("bar isn't 2", bar == 2);
  return 0;
}

const char* test_bat() {
  printf("test: bar\n");
  mu_not_implemented();
  return 0;
}

//==============================================================================
// Example test suite
//==============================================================================
const char* test_suite() {
  mu_run_test(test_foo);
  mu_run_test(test_bar);
  mu_run_test(test_bat);
  return 0;
}

//==============================================================================
// Example main.c
//==============================================================================
mu_init();

int main(int argc, char **argv) {
  // Run test suites
  const char* msg = NULL;
  if(!msg) msg = test_suite();
  // add more test suites here...

  // Print results
  mu_print_summary(msg);

  // Exit with success or failure
  return (msg != 0);
}

