#ifndef TEST_SOCKETS_H
#define TEST_SOCKETS_H

#include "minunit.h"
#include "sockets.h"
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>

//==============================================================================
// ClientSocket
//==============================================================================
static char* client_socket_init_test() {
  puts("client_socket_init_test");
  ClientSocket s;
  client_socket_init(&s);
  mu_assert("failed to initialize the file descriptor", s.fd == -1);
  mu_assert("failed to initialize the data pointer", s.data == 0);
  mu_assert("failed to initialize the data length", s.data_len == 0);
  return 0;
}

static char* client_socket_connect_test() {
  puts("client_socket_connect_test");

  // Use a different port in this test, because we're going to spawn a subprocess
  // that'll stick around for a while and use that port.
  const int port = 8001;
  bool result = false;

  ClientSocket s;
  client_socket_init(&s);

  result = client_socket_connect(&s, "localhost", port);
  mu_assert("should fail if no server is listening", !result);
  mu_assert("should reset file descriptor on failure", s.fd == -1);

  // Test accepting a connection by forking a new process and starting a server
  // socket listening from the child process
  pid_t pid = fork();
  if(pid < 0) {
    perror("Unable to fork during testing");
    mu_assert("should fork successfully during testing", false);
  }
  if(pid == 0) {
    // Child process: open a server socket
    ServerSocket server;
    server_socket_init(&server);
    server_socket_bind(&server, port);
    server_socket_listen(&server, 5);
    sleep(2); // Wait for client to connect
    exit(0);
  }
  else {
    // Parent process: open connection to server
    usleep(250 * 1000); // Wait a bit for server socket to listen
    result = client_socket_connect(&s, "127.0.0.1", port);
    mu_assert("failed to connect to server socket", result);
  }

  return 0;
}

static char* client_socket_send_test() {
  puts("client_socket_send_test");
  printf("- not implemented\n");
  //TODO
  // Sending on a closed connection should fail
  // Fork and send to a subprocess, make sure if succeeds
  return 0;
}

static char* client_socket_recv_test() {
  puts("client_socket_recv_test");
  printf("- not implemented\n");
  //TODO
  // Receiving on a closed connection should fail
  // Fork and recv from a subprocess, make sure if succeeds

  return 0;
}

static char* client_socket_close_test() {
  puts("client_socket_close_test");
  printf("- not implemented\n");
  //TODO
  // Test with uninitialized socket
  // Test with initialized socket
  // Make sure file descriptor is set to -1
  // Check that data is deleted
  return 0;
}

//==============================================================================
// ServerSocket
//==============================================================================
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
  mu_assert("failed with bound socket", result);

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
  mu_assert("shouldn't set client connection file descriptor", c.fd == -1);

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
  mu_assert("didn't accept a pending connection", result);
  mu_assert("didn't set the client's file descriptor", c.fd != -1);

  server_socket_close(&s);
  return 0;
}

static char* server_socket_close_test() {
  puts("server_socket_close_test");

  bool result = false;
  ServerSocket s;

  result = server_socket_close(&s);
  mu_assert("should succeed with uninitialized socket", result);
  mu_assert("failed to reset file descriptor to -1 (a)", s.fd == -1);

  server_socket_init(&s);
  result = server_socket_close(&s);
  mu_assert("should succeed with initialized socket", result);
  mu_assert("failed to reset file descriptor to -1 (b)", s.fd == -1);

  server_socket_init(&s);
  server_socket_bind(&s, 8000);
  server_socket_listen(&s, 1);
  result = server_socket_close(&s);
  mu_assert("should succeed with listening socket", result);
  mu_assert("failed to reset file descriptor to -1 (c)", s.fd == -1);

  return 0;
}

//==============================================================================
// Test-all function
//==============================================================================
char* test_sockets() {
  char* msg = NULL;

  mu_run_test(client_socket_init_test);
  mu_run_test(client_socket_connect_test);
  mu_run_test(client_socket_send_test);
  mu_run_test(client_socket_recv_test);
  mu_run_test(client_socket_close_test);

  mu_run_test(server_socket_init_test);
  mu_run_test(server_socket_bind_test);
  mu_run_test(server_socket_listen_test);
  mu_run_test(server_socket_accept_test);
  mu_run_test(server_socket_close_test);

  return msg;
}

#endif // TEST_SOCKETS_H