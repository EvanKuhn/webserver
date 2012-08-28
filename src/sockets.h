#ifndef SOCKETS_H
#define SOCKETS_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <string.h>

//==============================================================================
// ClientSocket
//==============================================================================
struct ClientSocket {
  int                fd;       // File descriptor
  struct sockaddr_in addr;     // Address
  char*              data;     // Data last read
  size_t             data_len; // Length of data last read
};
typedef struct ClientSocket ClientSocket;

// Initialize the socket
void client_socket_init(ClientSocket* s);

// Send data over the client socket
bool client_socket_send(ClientSocket* s, void* buf, size_t bufsize);

// Receive data from the socket.
// - Data will be placed in ClientSocket::data.
// - Data array size will be written to ClientSocket::data_len.
// - Any existing data array will be deleted.
bool client_socket_recv(ClientSocket* s);

// Close the socket and clean up any allocated resource, like data, within the
// ClientSocket struct.
bool client_socket_close(ClientSocket* s);

//==============================================================================
// ServerSocket
//==============================================================================
struct ServerSocket {
  int                fd;       // File descriptor
  struct sockaddr_in addr;     // Address
  bool               blocking; // Causes server_socket_accept() to block
};
typedef struct ServerSocket ServerSocket;

// Initialize the socket
bool server_socket_init(ServerSocket* s);

// Bind the socket to a port
bool server_socket_bind(ServerSocket* s, int port);

// Listen for incoming connections. Sets the max number of pending connections.
bool server_socket_listen(ServerSocket* s, int max_pending);

// Accept an incoming connection and initialize the ClientSocket.
bool server_socket_accept(ServerSocket* s, ClientSocket* c);

// Close a server socket
bool server_socket_close(ServerSocket* s);

#endif // SOCKETS_H