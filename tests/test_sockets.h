#ifndef TEST_SOCKETS_H
#define TEST_SOCKETS_H

#include "minunit.h"
#include "program_options.h"

static char* client_socket_xxx() {
  puts("client_socket_xxx");
  // TODO
  mu_assert("client socket does something", true);
  return 0;
}

char* test_sockets() {
  char* msg = NULL;
  mu_run_test(client_socket_xxx);
  return msg;
}

#endif // TEST_SOCKETS_H