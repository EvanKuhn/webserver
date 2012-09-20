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
#define mu_assert(message, test) \
  do { \
    if (!(test)) return message; \
  } while(0)

// Run a test and increment the tests_run counter
#define mu_run_test(test) \
  do { \
    const char *msg = test(); \
    if(msg) { ++mu_tests_failed; return msg; } \
    else    { ++mu_tests_passed; } \
  } while(0)

// Print a message and increment tests_skipped counter
#define mu_not_implemented() \
  ++mu_tests_not_impl; \
  printf("- test not implemented\n")

// Print a summary of tests. Takes the error message returned from the tests.
#define mu_print_summary(msg) \
  do { \
    if(msg) printf("- %s\n", msg); \
    printf("\nSummary: %i passed, %i failed, %i not implemented\n", \
      mu_tests_passed, mu_tests_failed, mu_tests_not_impl); \
    printf(msg ? "FAILURE\n" : "SUCCESS\n"); \
  } while(0)
