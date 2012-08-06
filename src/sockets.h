#ifndef SOCKETS_H
#define SOCKETS_H

#include <stdbool.h>

//==============================================================================
// ServerSocket
//==============================================================================
typdef struct {
  int port;
} ServerSocket;

// Initialize the socket
bool server_socket_init(ServerSocket* s);

// Bind the socket to a port
bool server_socket_bind(ServerSocket* s, int port);

// Listen for an incoming connection
bool server_socket_listen(ServerSocket* s);

// Accept an incoming connection and initialize the ClientSocket.
bool server_socket_accept(ServerSocket* s, ClientSocket* c);

//==============================================================================
// ClientSocket
//==============================================================================
typdef struct {
  char* data;
  size_t data_len;
} ClientSocket;

// Send data over the client socket
bool server_socket_send(ClientSocket* s, void* buf, size_t bufsize);

// Receive data from the socket.
// - Data will be placed in ClientSocket::data.
// - Data array size will be written to ClientSocket::data_len.
// - Any existing data array will be deleted.
bool server_socket_recv(ClientSocket* s);

// Close the socket and clean up any allocated resource, like data, within the
// ClientSocket struct.
bool server_socket_close(ClientSocket* s);

#endif // SOCKETS_H