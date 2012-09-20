//==============================================================================
// Unit-testing framework based on an extended version of minunit.h
//==============================================================================
#include <stdio.h>

// Counts. Initialize these to 0 in your main.c file.
extern int mu_tests_passed;
extern int mu_tests_failed;
extern int mu_tests_not_impl;

// Initialize the test counters. Call this at the top of your main.c file.
#define mu_init() \
  int mu_tests_passed = 0; \
  int mu_tests_failed = 0; \
  int mu_tests_not_impl = 0

// Assert something is true. If not, return the error message.
#define mu_assert(msg, test) \
  do { \
    if(!(test)) { \
      printf("- %s\n", msg); \
      ++mu_tests_failed; \
      return; \
    } \
  } while(0)

// Fail with the given error message. Causes the current function to return.
#define mu_fail(msg) \
  do { \
    printf("- %s\n", msg); \
    ++mu_tests_failed; \
    return; \
  } while(0)

// Run a test. Return from the calling function if the test fails.
#define mu_run_test(test, name) \
  do { \
    if(!mu_tests_failed) { \
      printf("test: %s\n", name); \
      test(); \
      if(mu_tests_failed) return; \
      ++mu_tests_passed; \
    } \
  } while(0)

// Run a test suite. Skip the suite if a previous test failed.
#define mu_run_suite(suite, name) \
  do { \
    if(!mu_tests_failed) { \
      printf("\nsuite: %s\n", name); \
      suite(); \
    } \
  } while(0)

// Print a message and increment not-implemented counter
#define mu_not_implemented() \
  do { \
    ++mu_tests_not_impl; \
    printf("- test not implemented\n"); \
  } while(0)

// Print a summary of tests. Takes the error message returned from the tests.
#define mu_print_summary() \
  do { \
    printf("\nSummary: %i passed, %i failed, %i not implemented\n", \
      mu_tests_passed, mu_tests_failed, mu_tests_not_impl); \
    printf(mu_tests_failed ? "FAILURE\n" : "SUCCESS\n"); \
  } while(0)

#define mu_return_status() \
  (mu_tests_failed ? 1 : 0)
