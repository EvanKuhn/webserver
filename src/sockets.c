#include "sockets.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//==============================================================================
// Constants
//==============================================================================
const int CLIENT_SOCKET_RECV_BUFFER_SIZE = 1024;

//==============================================================================
// ClientSocket
//==============================================================================
void client_socket_init(ClientSocket* s) {
  s->fd = -1;
  memset(&s->addr, 0, sizeof(s->addr));
  s->data = 0;
  s->data_len = 0;
}

bool client_socket_connect(ClientSocket* s, const char* ip, int port) {
  // Make sure the socket isn't already open
  if(s->fd != -1) return false;

  // Create the socket
  s->fd = socket(AF_INET, SOCK_STREAM, 0);

  // Set up the address and connect to it
  memset(&s->addr, 0, sizeof(s->addr));
  s->addr.sin_family = AF_INET;
  s->addr.sin_addr.s_addr = inet_addr(ip);
  s->addr.sin_port = htons(port);
  const int result = connect(s->fd, (struct sockaddr*)&s->addr, sizeof(s->addr));
  const bool status = (result != -1);

  // If we failed, reinitialize the socket
  if(!status) client_socket_init(s);
  return status;
}

// Send data over the client socket
bool client_socket_send(ClientSocket* s, void* buf, size_t bufsize) {
  int result = send(s->fd, buf, bufsize, 0);
  return (result != -1);
}

// Receive data from the socket.
// - Data will be placed in ClientSocket::data.
// - Data array size will be written to ClientSocket::data_len.
// - Any existing data array will be deleted.
bool client_socket_recv(ClientSocket* s) {
  if(!s->data) {
    s->data_len = CLIENT_SOCKET_RECV_BUFFER_SIZE;
    s->data = malloc(CLIENT_SOCKET_RECV_BUFFER_SIZE);
  }
  memset(s->data, 0, s->data_len);

  char*  write_to_addr = s->data;
  size_t buffer_space = s->data_len;
  size_t bytes_received = 0;

  while(1) {
    // Receive data
    ssize_t bytes = recv(s->fd, write_to_addr, buffer_space, 0);

    // If -1, there was an error
    if(bytes == -1) return false;

    // If our buffer is big enough to fit all the data, we're done
    if(bytes < buffer_space) return true;

    // Otherwise we need to reallocate a bigger buffer and keep receiving
    bytes_received += bytes;
    s->data_len *= 2;
    s->data = realloc(s->data, s->data_len);
    write_to_addr = s->data + bytes_received;
    buffer_space = s->data_len - bytes_received;
  }
}

bool client_socket_close(ClientSocket* s) {
  const int result = close(s->fd);
  const bool status = (result != -1);
  if(s->data && s->data_len > 0) {
    free(s->data);
    s->data = 0;
    s->data_len = 0;
  }
  if(status) {
    s->fd = -1;
  }
  return status;
}

//==============================================================================
// ServerSocket
//==============================================================================
bool server_socket_init(ServerSocket* s) {
  // Start from invalid/zero values
  s->fd = -1;
  memset(&s->addr, 0, sizeof(s->addr));
  s->blocking = true;

  // Create the socket
  s->fd = socket(AF_INET, SOCK_STREAM, 0);
  if(s->fd == -1) return false;

  // Initialize the socket's fields
  s->addr.sin_family = AF_INET;
  s->addr.sin_addr.s_addr = INADDR_ANY;

  // Enable blocking IO
  server_socket_set_blocking(s, s->blocking);
  return true;
}

bool server_socket_set_blocking(ServerSocket* s, bool blocking) {
  s->blocking = blocking;
  int flags = fcntl(s->fd, F_GETFL, 0);
  if(flags == -1) return false;
  flags = (s->blocking ? flags & (~O_NONBLOCK) : flags | O_NONBLOCK);
  return fcntl(s->fd, F_SETFL, flags) != -1;
}

bool server_socket_bind(ServerSocket* s, int port) {
  s->addr.sin_port = htons(port);
  const int result = bind(s->fd, (struct sockaddr*)&s->addr, sizeof(s->addr));
  return (result != -1);
}

bool server_socket_listen(ServerSocket* s, int max_pending) {
  if(s->addr.sin_port == 0) return false; // Make sure socket is bound to port
  int result = listen(s->fd, max_pending);
  return (result != -1);
}

bool server_socket_accept(ServerSocket* s, ClientSocket* c) {
  socklen_t client_len = sizeof(c->addr);
  c->fd = accept(s->fd, (struct sockaddr*)&c->addr, &client_len);
  return (c->fd != -1);
}

bool server_socket_accept_poll(ServerSocket* server,
                               ClientSocket* client,
                               int wait_time_ms,
                               int timeout_ms)
{
  for(int waited=0; waited<timeout_ms; waited+=wait_time_ms) {
    const bool result = server_socket_accept(server, client);
    if(result) {
      return true;
    }
    if(errno != EWOULDBLOCK) {
      return false;
    }
    usleep(wait_time_ms * 1000);
  }
  errno = EWOULDBLOCK;  // HACK? usleep sets errno to ETIMEDOUT
  return false;
}

bool server_socket_close(ServerSocket* s) {
  const int result = close(s->fd);
  const bool status = (result != -1);
  if(status) s->fd = -1;
  return status;
}
