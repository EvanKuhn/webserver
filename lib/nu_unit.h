//==============================================================================
// nu_unit: C unit-testing framework based on MinUnit
//==============================================================================
#include <stdio.h>

// Counts. Initialize these to 0 in your main.c file.
extern int nu_tests_passed;
extern int nu_tests_failed;
extern int nu_tests_not_impl;

// Initialize the test counters. Call this at the top of your main.c file.
#define nu_init() \
  int nu_tests_passed = 0; \
  int nu_tests_failed = 0; \
  int nu_tests_not_impl = 0

// Assert something is true. If not, return the error message.
#define nu_assert(msg, test) \
  do { \
    if(!(test)) { \
      printf("- %s:%i assert failed: %s\n", __FILE__, __LINE__, msg); \
      ++nu_tests_failed; \
      return; \
    } \
  } while(0)

// Fail with the given error message. Causes the current function to return.
#define nu_fail(msg) \
  do { \
    printf("- %s\n", msg); \
    ++nu_tests_failed; \
    return; \
  } while(0)

// Run a test. Return from the calling function if the test fails.
#define nu_run_test(test, name) \
  do { \
    if(!nu_tests_failed) { \
      printf("test: %s\n", name); \
      test(); \
      if(nu_tests_failed) return; \
      ++nu_tests_passed; \
    } \
  } while(0)

// Run a test suite. Skip the suite if a previous test failed.
#define nu_run_suite(suite, name) \
  do { \
    if(!nu_tests_failed) { \
      printf("\nsuite: %s\n", name); \
      suite(); \
    } \
  } while(0)

// Print a message and increment not-implemented counter
#define nu_not_implemented() \
  do { \
    ++nu_tests_not_impl; \
    printf("- test not implemented\n"); \
  } while(0)

// Print a summary of tests. Takes the error message returned from the tests.
#define nu_print_summary() \
  do { \
    printf("\nSummary: %i passed, %i failed, %i not implemented\n", \
      nu_tests_passed, nu_tests_failed, nu_tests_not_impl); \
    printf(nu_tests_failed ? "FAILURE\n" : "SUCCESS\n"); \
  } while(0)

#define nu_return_status() \
  (nu_tests_failed ? 1 : 0)
