//==============================================================================
// Socket helper functions
//
// Evan Kuhn 2012-09-09
//==============================================================================
#ifndef SOCKETS_H
#define SOCKETS_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <string.h>
#include "status.h"

//==============================================================================
// ClientSocket
//==============================================================================
typedef struct ClientSocket ClientSocket;

// Initialize the socket
ClientSocket* client_socket_new();

// Connect to a server listening on the given IP address and port
Status client_socket_connect(ClientSocket* s, const char* ip, int port);

// Send data over the client socket
Status client_socket_send(ClientSocket* s, void* buf, size_t bufsize);

// Receive data from the socket. Overwrites previously-received data.
Status client_socket_recv(ClientSocket* s);

// Get the last set of data received
char* client_socket_get_data(ClientSocket* s);

// Get the IP address that the socket is connected to
const char* client_socket_get_ip(ClientSocket* s);

// Get the port that the socket is connected to
uint16_t client_socket_get_port(ClientSocket* s);

// Close the socket and clean up any allocated resource.
// - Will always clean up data, even if it fails to close the socket.
Status client_socket_close(ClientSocket* s);

// Close the client socket and free the object
void client_socket_free(ClientSocket* s);

//==============================================================================
// ServerSocket
//==============================================================================
typedef struct ServerSocket ServerSocket;

// Create a new server socket
ServerSocket* server_socket_new();

// Initialize the socket
Status server_socket_init(ServerSocket* s);

// Enable or disable blocking IO for the socket.
// - Blocking IO enabled by default.
Status server_socket_set_blocking(ServerSocket* s, bool blocking);

// Bind the socket to a port
Status server_socket_bind(ServerSocket* s, int port);

// Listen for incoming connections. Sets the max number of pending connections.
Status server_socket_listen(ServerSocket* s, int max_pending);

// Accept an incoming connection and initialize the ClientSocket.
// - If socket is non-blocking and this function returns false, check if
//   errno equals EWOULDBLOCK. If so, it's not an error, but rather there are
//   no pending connections.
Status server_socket_accept(ServerSocket* s, ClientSocket* c);

// Accept a connection on a server socket by polling. The ServerSocket must be
// set to non-blocking for this to work properly. Returns true on success and
// false on error or timeout.
Status server_socket_accept_poll(ServerSocket* server,
                                 ClientSocket* client,
                                 int wait_time_ms,
                                 int timeout_ms);

// Close a server socket
// - If successful, sets file descriptor to -1
Status server_socket_close(ServerSocket* s);

// Close the server socket and free the object
void server_socket_free(ServerSocket* s);

#endif // SOCKETS_H