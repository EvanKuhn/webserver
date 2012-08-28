#include "sockets.h"
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

//==============================================================================
// ClientSocket
//==============================================================================
void client_socket_init(ClientSocket* s) {
  s->fd = -1;
  memset(&s->addr, 0, sizeof(s->addr));
  s->data = 0;
  s->data_len = 0;
}

// Send data over the client socket
bool client_socket_send(ClientSocket* s, void* buf, size_t bufsize) {
  return true; //TODO
}

// Receive data from the socket.
// - Data will be placed in ClientSocket::data.
// - Data array size will be written to ClientSocket::data_len.
// - Any existing data array will be deleted.
bool client_socket_recv(ClientSocket* s) {
  return true; //TODO
}

// Close the socket and clean up any allocated resource, like data, within the
// ClientSocket struct.
bool client_socket_close(ClientSocket* s) {
  const int result = close(s->fd);
  if(s->data && s->data_len > 0) {
    free(s->data, s->data_len); // TODO
  }
  client_socket_init(s); // Reset data members
  return (result != -1);
}

//==============================================================================
// ServerSocket
//==============================================================================
bool server_socket_init(ServerSocket* s) {
  // Start from invalid/zero values
  s->fd = -1;
  memset(&s->addr, 0, sizeof(s->addr));
  s->blocking = true; // TODO - how do we let the user set this?

  // Create the socket
  s->fd = socket(AF_INET, SOCK_STREAM, 0);
  if(s->fd == -1) return false;

  // Initialize the socket's fields
  s->addr.sin_family = AF_INET;
  s->addr.sin_addr.s_addr = INADDR_ANY;

  // Set blocking
  int flags = fcntl(s->fd, F_GETFL, 0);
  if(flags == -1) return false;
  flags = (s->blocking ? flags & (~O_NONBLOCK) : flags | O_NONBLOCK);
  fcntl(s->fd, F_SETFL, flags);

  return true;
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
  c->fd = accept(s->fd, &c->addr, sizeof(c->addr));
  return (c->fd != -1);
}

bool server_socket_close(ServerSocket* s) {
  const int result = close(s->fd);
  return (result != -1);
}