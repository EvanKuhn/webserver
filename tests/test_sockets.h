#ifndef TEST_SOCKETS_H
#define TEST_SOCKETS_H

#include "minunit.h"
#include "sockets.h"
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>

static char* server_socket_init_test() {
  puts("server_socket_init_test");
  ServerSocket s;
  bool result = server_socket_init(&s);
  mu_assert("returned false", result);
  mu_assert("failed to set the file descriptor", s.fd != 0);
  mu_assert("failed to set the socket address family",
            s.addr.sin_family == AF_INET);
  mu_assert("failed to set the in-address",
            s.addr.sin_addr.s_addr == INADDR_ANY);
  mu_assert("failed to set port to 0",
            s.addr.sin_port == 0);
  server_socket_close(&s);
  return 0;
}

static char* server_socket_bind_test() {
  puts("server_socket_bind_test");
  ServerSocket s;
  server_socket_init(&s);
  bool result = server_socket_bind(&s, 8000);
  mu_assert("returned false", result);
  mu_assert("failed to set the port", s.addr.sin_port == htons(8000));
  server_socket_close(&s);
  return 0;
}

static char* server_socket_listen_test() {
  puts("server_socket_listen_test");
  ServerSocket s;
  server_socket_init(&s);

  bool result = server_socket_listen(&s, 10);
  mu_assert("should fail with unbound socket", !result);

  server_socket_bind(&s, 8000);
  result = server_socket_listen(&s, 10);
  mu_assert("should succeed with bound socket", result);

  server_socket_close(&s);
  return 0;
}

static char* server_socket_accept_test() {
  puts("server_socket_accept_test");
  ClientSocket c;
  ServerSocket s;
  client_socket_init(&c);
  server_socket_init(&s);
  server_socket_set_blocking(&s, false);

  bool result = server_socket_accept(&s, &c);
  mu_assert("should fail with unbound socket", !result);

  server_socket_bind(&s, 8000);
  result = server_socket_accept(&s, &c);
  mu_assert("should fail with non-listening socket", !result);

  server_socket_listen(&s, 1);
  result = server_socket_accept(&s, &c);
  result = (!result && errno == EWOULDBLOCK);
  mu_assert("should fail with no pending connections", result);
  mu_assert("won't set client connection file descriptor", c.fd == -1);

  // Test accepting a connection by forking a new process and initiating a
  // client connection from that process.
  pid_t pid = fork();
  if(pid < 0) {
    perror("Unable to fork during testing");
    mu_assert("should fork successfully during testing", false);
  }
  if(pid == 0) {
    // Child process. Open a connection to port 8000.
    ClientSocket pending;
    client_socket_init(&pending);
    client_socket_connect(&pending, "127.0.0.1", 8000);
    exit(0);
  }

  // Wait a bit for the child process, then accept
  usleep(250 * 1000);
  server_socket_listen(&s, 1);
  result = server_socket_accept(&s, &c);
  mu_assert("should accept a pending connection", result);
  mu_assert("should set the client's file descriptor", c.fd != -1);

  server_socket_close(&s);
  return 0;
}

// TODO - test client sockets

char* test_sockets() {
  char* msg = NULL;
  mu_run_test(server_socket_init_test);
  mu_run_test(server_socket_bind_test);
  mu_run_test(server_socket_listen_test);
  mu_run_test(server_socket_accept_test);
  return msg;
}

#endif // TEST_SOCKETS_H