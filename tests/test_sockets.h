//==============================================================================
// Socket tests
//
// Evan Kuhn 2012-09-09
//==============================================================================
#ifndef TEST_SOCKETS_H
#define TEST_SOCKETS_H

#include "nu_unit.h"
#include "sockets.h"
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// Localhost ip address
const char* LOCALHOST = "127.0.0.1";

// We'll reserve port 8000 for tests that don't fork. For those that do, use
// 8001 and greater. This function will return such a port number and increment
// the number returned each time it is called.
int get_next_port() {
  static int port = 0;
  if(port == 0) {
    srand(time(NULL));
    port = 8000 + (rand() % 30000);
  }
  return ++port;
}

//==============================================================================
// ClientSocket tests
//==============================================================================
void test__client_socket_init() {
  ClientSocket s;
  client_socket_init(&s);
  nu_check("failed to initialize the file descriptor", s.fd == -1);
  nu_check("failed to initialize the data pointer", s.data == 0);
  nu_check("failed to initialize the data length", s.data_len == 0);
}

void test__client_socket_connect() {
  // Use a different port in this test, because we're going to spawn a subprocess
  // that'll stick around for a while and use that port.
  const int port = get_next_port();
  Status result = make_status(false, 0);

  ClientSocket s;
  client_socket_init(&s);

  result = client_socket_connect(&s, "localhost", port);
  nu_assert("should fail if no server is listening", !result.ok);
  nu_check("should reset file descriptor on failure", s.fd == -1);

  // Test accepting a connection by forking a new process and starting a server
  // socket listening from the child process
  pid_t pid = fork();
  if(pid < 0) {
    perror("Unable to fork during testing");
    nu_assert("should fork successfully during testing", false);
  }
  else if(pid == 0) {
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
    usleep(100 * 1000); // Wait a bit for server socket to listen
    result = client_socket_connect(&s, LOCALHOST, port);
    nu_assert("failed to connect to server socket", result.ok);
  }

  client_socket_close(&s);
}

void test__client_socket_send() {
  const int port = get_next_port();
  Status result = make_status(false, 0);

  ClientSocket s;
  client_socket_init(&s);

  result = client_socket_send(&s, "asdf", 4);
  nu_assert("sending over an unconnected socket shouldn't succeed", !result.ok);

  // Test sending by forking a server process and sending to it
  pid_t pid = fork();
  if(pid < 0) {
    perror("Unable to fork during testing");
    nu_assert("should fork successfully during testing", false);
  }
  else if(pid == 0) {
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
    usleep(100 * 1000); // Wait a bit for server socket to listen
    result = client_socket_connect(&s, LOCALHOST, port);
    nu_assert("failed to connect to server socket", result.ok);
    result = client_socket_send(&s, "asdf", 4);
    nu_assert("failed to send data to the server", result.ok);
  }

  client_socket_close(&s);
}

// Helper function used by recv_test functions
void client_socket_recv_test_helper(const char* msg, size_t msg_len) {
  const int port = get_next_port();
  Status result = make_status(false, 0);

  // Create socket and start testing
  ClientSocket s;
  client_socket_init(&s);

  result = client_socket_recv(&s);
  nu_assert("receiving on an unconnected socket shouldn't succeed", !result.ok);

  pid_t pid = fork();
  if(pid < 0) {
    perror("Unable to fork during testing");
    nu_assert("should fork successfully during testing", false);
  }
  else if(pid == 0) {
    // Child process: open a server, accept a connection, and send something
    Status status = { false, 0 };
    ServerSocket server;
    ClientSocket client;
    client_socket_init(&client);

    status = server_socket_init(&server);
    if(!status.ok) {
      fprintf(stderr, "CHILD ERROR: server_socket_init failed, errno = %i\n", status.errnum);
      exit(0);
    }

    status = server_socket_bind(&server, port);
    if(!status.ok) {
      fprintf(stderr, "CHILD ERROR: server_socket_bind to port %i failed, errno = %i\n", port, status.errnum);
      exit(0);
    }

    status = server_socket_set_blocking(&server, false);
    if(!status.ok) {
      fprintf(stderr, "CHILD ERROR: server_socket_set_blocking failed, errno = %i\n", status.errnum);
      exit(0);
    }

    status = server_socket_listen(&server, 5);
    if(!status.ok) {
      fprintf(stderr, "CHILD ERROR: server_socket_listen failed, errno = %i\n", status.errnum);
      exit(0);
    }

    if(server_socket_accept_poll(&server, &client, 250, 5000).ok) {
      client_socket_send(&client, msg, msg_len);
    }
    client_socket_close(&client);
    server_socket_close(&server);
    exit(0);
  }
  else {
    // Parent process: accept connection and read data
    usleep(100 * 1000);
    result = client_socket_connect(&s, LOCALHOST, port);
    nu_assert("couldn't connect to server", result.ok);

    result = client_socket_recv(&s);
    nu_assert("didn't receive data from server", result.ok);

    result.ok = (s.data_len >= msg_len);
    nu_assert("didn't receive all the data from the server", result.ok);

    result.ok = (strncmp(msg, s.data, s.data_len) == 0);
    nu_assert("didn't receive expected string from server", result.ok);
  }

  client_socket_close(&s);
}

void test__client_socket_recv__short() {
  const char* msg = "hello world!";
  const size_t msglen = strlen(msg);
  client_socket_recv_test_helper(msg, msglen);
}

void test__client_socket_recv__long() {
  // Create some test data
  const size_t msglen = 2047;
  char* msg = malloc(msglen + 1);
  memset(msg, 'x', msglen);
  msg[msglen] = 0;

  // Run the test, free the message, and return
  client_socket_recv_test_helper(msg, msglen);
  free(msg);
}

void test__client_socket_close() {
  const int port = get_next_port();
  Status result = make_status(false, 0);

  // Set up a server socket to accept connections
  ServerSocket server;
  result = server_socket_init(&server);
  nu_assert("failed to initialize server socket for test", result.ok);

  result = server_socket_bind(&server, port);
  nu_assert("failed to bind server socket for test", result.ok);

  result = server_socket_listen(&server, 5);
  nu_assert("failed to set server socket to listen", result.ok);

  // Now we can start testing
  ClientSocket s;
  client_socket_init(&s);

  result = client_socket_close(&s);
  nu_assert("should fail with unconnected socket", !result.ok);

  // Set up some fake data
  s.data = malloc(5);
  memcpy(s.data, "asdf", 5);
  s.data_len = 5;

  result = client_socket_connect(&s, LOCALHOST, port);
  if(!result.ok) printf("- errno: %i\n", result.errnum);
  nu_assert("connect should succeed", result.ok);
  nu_assert("socket file descriptor should not equal -1", s.fd != -1);

  result = client_socket_close(&s);
  nu_assert("should succeed with connected socket", result.ok);
  nu_assert("should set socket file descriptor to -1", s.fd == -1);
}

//==============================================================================
// ServerSocket tests
//==============================================================================
void test__server_socket_init() {
  ServerSocket s;
  Status result = server_socket_init(&s);
  nu_check("returned false", result.ok);
  nu_check("failed to set the file descriptor", s.fd != 0);
  nu_check("failed to set the socket address family", s.addr.sin_family == AF_INET);
  nu_check("failed to set the in-address", s.addr.sin_addr.s_addr == INADDR_ANY);
  nu_check("failed to set port to 0", s.addr.sin_port == 0);
  server_socket_close(&s);
}

void test__server_socket_bind() {
  const int port = get_next_port();
  ServerSocket s;
  server_socket_init(&s);
  Status result = server_socket_bind(&s, port);
  nu_check("returned false", result.ok);
  nu_check("failed to set the port", s.addr.sin_port == htons(port));
  server_socket_close(&s);
}

void test__server_socket_listen() {
  const int port = get_next_port();
  ServerSocket s;
  server_socket_init(&s);

  Status result = server_socket_listen(&s, 10);
  nu_assert("should fail with unbound socket", !result.ok);

  server_socket_bind(&s, port);
  result = server_socket_listen(&s, 10);
  nu_assert("failed with bound socket", result.ok);

  server_socket_close(&s);
}

void test__server_socket_accept() {
  Status result = make_status(false, 0);
  const int port = get_next_port();

  ClientSocket c;
  ServerSocket s;
  client_socket_init(&c);
  server_socket_init(&s);

  result = server_socket_set_blocking(&s, false);
  nu_assert("failed to set server socket non-blocking", result.ok);

  result = server_socket_accept(&s, &c);
  nu_assert("should fail with unbound socket", !result.ok);

  server_socket_bind(&s, port);
  result = server_socket_accept(&s, &c);
  nu_assert("should fail with non-listening socket", !result.ok);

  server_socket_listen(&s, 1);
  result = server_socket_accept(&s, &c);

  bool ok = (!result.ok && result.errnum == EWOULDBLOCK);
  nu_assert("should fail with no pending connections", ok);
  nu_assert("shouldn't set client connection file descriptor", c.fd == -1);

  // Test accepting a connection by forking a new process and initiating a
  // client connection from that process.
  pid_t pid = fork();
  if(pid < 0) {
    perror("Unable to fork during testing");
    nu_assert("should fork successfully during testing", false);
  }
  else if(pid == 0) {
    // Child process: open a connection to the server
    ClientSocket pending;
    client_socket_init(&pending);
    client_socket_connect(&pending, LOCALHOST, port);
    exit(0);
  }
  else {
    // Parent process: wait for child to attemp connection, then accept
    usleep(100 * 1000);
    result = server_socket_accept(&s, &c);
    nu_assert("didn't accept a pending connection", result.ok);
    nu_assert("didn't set the client's file descriptor", c.fd != -1);
  }

  server_socket_close(&s);
}

void test__server_socket_accept_poll() {
  Status result = make_status(false, 0);
  const int port = get_next_port();
  const int interval = 100;
  const int timeout = 1000;

  ClientSocket c;
  ServerSocket s;
  client_socket_init(&c);
  server_socket_init(&s);
  server_socket_set_blocking(&s, false);

  result = server_socket_accept_poll(&s, &c, interval, timeout);
  nu_assert("should fail with unbound socket", !result.ok);

  server_socket_bind(&s, port);
  result = server_socket_accept_poll(&s, &c, interval, timeout);
  nu_assert("should fail with non-listening socket", !result.ok);

  server_socket_listen(&s, 1);
  result = server_socket_accept_poll(&s, &c, interval, timeout);
  bool ok = (!result.ok && result.errnum == EWOULDBLOCK);
  nu_assert("should fail with no pending connections", ok);
  nu_assert("shouldn't set client connection file descriptor", c.fd == -1);

  // Test accepting a connection by forking a new process and initiating a
  // client connection from that process.
  pid_t pid = fork();
  if(pid < 0) {
    perror("Unable to fork during testing");
    nu_assert("should fork successfully during testing", false);
  }
  else if(pid == 0) {
    // Child process: wait a bit, then open a connection to the server
    usleep(100 * 1000);
    ClientSocket pending;
    client_socket_init(&pending);
    client_socket_connect(&pending, LOCALHOST, port);
    exit(0);
  }
  else {
    // Parent process: poll for child process
    result = server_socket_accept_poll(&s, &c, interval, timeout);
    nu_assert("didn't accept a pending connection", result.ok);
    nu_assert("didn't set the client's file descriptor", c.fd != -1);
  }

  server_socket_close(&s);
}

void test__server_socket_close() {
  Status result = make_status(false, 0);

  ServerSocket s;
  server_socket_init(&s);

  result = server_socket_close(&s);
  nu_assert("should succeed with initialized socket", result.ok);
  nu_assert("failed to reset file descriptor to -1 (b)", s.fd == -1);

  server_socket_init(&s);
  server_socket_bind(&s, 8000);
  server_socket_listen(&s, 1);
  result = server_socket_close(&s);
  nu_assert("should succeed with listening socket", result.ok);
  nu_assert("failed to reset file descriptor to -1 (c)", s.fd == -1);
}

//==============================================================================
// Test suites
//==============================================================================
void test_suite__client_socket() {
  nu_run_test(test__client_socket_init,        "client_socket_init()");
  nu_run_test(test__client_socket_connect,     "client_socket_connect()");
  nu_run_test(test__client_socket_send,        "client_socket_send()");
  nu_run_test(test__client_socket_recv__short, "client_socket_recv() w/ short message");
  nu_run_test(test__client_socket_recv__long,  "client_socket_recv() w/ long message");
  nu_run_test(test__client_socket_close,       "client_socket_close()");
}

void test_suite__server_socket() {
  nu_run_test(test__server_socket_init,        "server_socket_init()");
  nu_run_test(test__server_socket_bind,        "server_socket_bind()");
  nu_run_test(test__server_socket_listen,      "server_socket_listen()");
  nu_run_test(test__server_socket_accept,      "server_socket_accept()");
  nu_run_test(test__server_socket_accept_poll, "server_socket_accept_poll()");
  nu_run_test(test__server_socket_close,       "server_socket_close()");
}

#endif // TEST_SOCKETS_H