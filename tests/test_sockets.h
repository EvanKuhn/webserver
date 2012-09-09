//==============================================================================
// Socket tests
//
// Evan Kuhn 2012-09-09
//==============================================================================
#ifndef TEST_SOCKETS_H
#define TEST_SOCKETS_H

#include "minunit.h"
#include "sockets.h"
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

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
    printf("- socket tests starting with port %i\n", port);
  }
  return ++port;
}

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
  const int port = get_next_port();
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
    mu_assert("failed to connect to server socket", result);
  }

  client_socket_close(&s);
  return 0;
}

static char* client_socket_send_test() {
  puts("client_socket_send_test");
  const int port = get_next_port();
  bool result = false;

  ClientSocket s;
  client_socket_init(&s);

  result = client_socket_send(&s, "asdf", 4);
  mu_assert("sending over an unconnected socket shouldn't succeed", !result);

  // Test sending by forking a server process and sending to it
  pid_t pid = fork();
  if(pid < 0) {
    perror("Unable to fork during testing");
    mu_assert("should fork successfully during testing", false);
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
    mu_assert("failed to connect to server socket", result);
    result = client_socket_send(&s, "asdf", 4);
    mu_assert("failed to send data to the server", result);
  }

  client_socket_close(&s);
  return 0;
}

// Helper function used by recv_test functions
static char* client_socket_recv_test_helper(char* test_message,
                                            size_t test_message_len)
{
  const int port = get_next_port();
  bool result = false;

  // Create socket and start testing
  ClientSocket s;
  client_socket_init(&s);

  result = client_socket_recv(&s);
  mu_assert("receiving on an unconnected socket shouldn't succeed", !result);

  pid_t pid = fork();
  if(pid < 0) {
    perror("Unable to fork during testing");
    mu_assert("should fork successfully during testing", false);
  }
  else if(pid == 0) {
    // Child process: open a server, accept a connection, and send something
    ClientSocket client;
    client_socket_init(&client);
    ServerSocket server;
    if(!server_socket_init(&server)) {
      printf("CHILD ERROR: server_socket_init failed, errno = %i\n", errno);
      exit(0);
    }
    if(!server_socket_bind(&server, port)) {
      printf("CHILD ERROR: server_socket_bind to port %i failed, errno = %i\n", port, errno);
      exit(0);
    }
    if(!server_socket_set_blocking(&server, false)) {
      printf("CHILD ERROR: server_socket_set_blocking failed, errno = %i\n", errno);
      exit(0);
    }
    if(!server_socket_listen(&server, 5)) {
      printf("CHILD ERROR: server_socket_listen failed, errno = %i\n", errno);
      exit(0);
    }
    if(server_socket_accept_poll(&server, &client, 250, 5000)) {
      client_socket_send(&client, test_message, test_message_len);
    }
    client_socket_close(&client);
    server_socket_close(&server);
    exit(0);
  }
  else {
    // Parent process: accept connection and read data
    usleep(100 * 1000);
    result = client_socket_connect(&s, LOCALHOST, port);
    mu_assert("couldn't connect to server", result);

    result = client_socket_recv(&s);
    mu_assert("didn't receive data from server", result);

    result = (s.data_len >= test_message_len);
    mu_assert("didn't receive all the data from the server", result);

    result = (strncmp(test_message, s.data, s.data_len) == 0);
    //if(!result) printf("- received: %s\n", s.data);
    mu_assert("didn't receive expected string from server", result);
  }

  client_socket_close(&s);
  return 0;
}

static char* client_socket_recv_test__short_msg() {
  puts("client_socket_recv_test (short message)");

  // Create a test message
  char* msg = "hello world!";
  size_t msglen = strlen(msg);

  // Run the test
  return client_socket_recv_test_helper(msg, msglen);
}

static char* client_socket_recv_test__long_msg() {
  puts("client_socket_recv_test (long message)");

  // Create some test data
  const size_t msglen = 2047;
  char* msg = malloc(msglen + 1);
  memset(msg, 'x', msglen);
  msg[msglen] = 0;

  // Run the test, free the message, and return
  char* result = client_socket_recv_test_helper(msg, msglen);
  free(msg);
  return result;
}

static char* client_socket_close_test() {
  puts("client_socket_close_test");
  const int port = get_next_port();
  bool result = false;

  // Set up a server socket to accept connections
  ServerSocket server;
  result = server_socket_init(&server);
  mu_assert("failed to initialize server socket for test", result);

  result = server_socket_bind(&server, port);
  mu_assert("failed to bind server socket for test", result);

  result = server_socket_listen(&server, 5);
  mu_assert("failed to set server socket to listen", result);

  // Now we can start testing
  ClientSocket s;
  client_socket_init(&s);

  result = client_socket_close(&s);
  mu_assert("should fail with unconnected socket", !result);

  // Set up some fake data
  s.data = malloc(5);
  memcpy(s.data, "asdf", 5);
  s.data_len = 5;

  result = client_socket_connect(&s, LOCALHOST, port);
  mu_assert("socket file descriptor should not equal -1", s.fd != -1);

  result = client_socket_close(&s);
  mu_assert("should succeed with connected socket", result);
  mu_assert("should set socket file descriptor to -1", s.fd == -1);

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
  bool result = false;
  const int port = get_next_port();

  ClientSocket c;
  ServerSocket s;
  client_socket_init(&c);
  server_socket_init(&s);
  server_socket_set_blocking(&s, false);

  result = server_socket_accept(&s, &c);
  mu_assert("should fail with unbound socket", !result);

  server_socket_bind(&s, port);
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
    mu_assert("didn't accept a pending connection", result);
    mu_assert("didn't set the client's file descriptor", c.fd != -1);
  }

  server_socket_close(&s);
  return 0;
}

static char* server_socket_accept_poll_test() {
  puts("server_socket_accept_poll_test");
  bool result = false;
  const int port = get_next_port();
  const int interval = 100;
  const int timeout = 1000;

  ClientSocket c;
  ServerSocket s;
  client_socket_init(&c);
  server_socket_init(&s);
  server_socket_set_blocking(&s, false);

  result = server_socket_accept_poll(&s, &c, interval, timeout);
  mu_assert("should fail with unbound socket", !result);

  server_socket_bind(&s, port);
  result = server_socket_accept_poll(&s, &c, interval, timeout);
  mu_assert("should fail with non-listening socket", !result);

  server_socket_listen(&s, 1);
  result = server_socket_accept_poll(&s, &c, interval, timeout);
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
  else if(pid == 0) {
    // Child process: wait a bit, then open a connection to the server
    usleep(100 * 1000);
    ClientSocket pending;
    client_socket_init(&pending);
    client_socket_connect(&pending, LOCALHOST, port);
    exit(0);
  }
  else {
    // Wait a bit for the child process, then accept
    result = server_socket_accept_poll(&s, &c, interval, timeout);
    mu_assert("didn't accept a pending connection", result);
    mu_assert("didn't set the client's file descriptor", c.fd != -1);
  }

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

  printf("\n");
  mu_run_test(client_socket_init_test);
  mu_run_test(client_socket_connect_test);
  mu_run_test(client_socket_send_test);
  mu_run_test(client_socket_recv_test__short_msg);
  mu_run_test(client_socket_recv_test__long_msg);
  mu_run_test(client_socket_close_test);

  printf("\n");
  mu_run_test(server_socket_init_test);
  mu_run_test(server_socket_bind_test);
  mu_run_test(server_socket_listen_test);
  mu_run_test(server_socket_accept_test);
  mu_run_test(server_socket_accept_poll_test);
  mu_run_test(server_socket_close_test);

  return msg;
}

#endif // TEST_SOCKETS_H