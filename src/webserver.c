#include "webserver.h"
#include "http_request.h"
#include "sockets.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

//==============================================================================
// Constants and utilities
//==============================================================================
// Max number of incoming connections that may be queued by the server's socket
const int MAX_PENDING_CONNS = 100;

// Check status. On error, print message and return from calling function.
#define return_on_error(status, errmsg) \
if(!status.ok) { \
  fprintf(stderr, "%s (errno: %i)\n", errmsg, status.errnum); \
  return; \
}

// Check status. On error, print message and continue calling loop.
#define continue_on_error(status, errmsg) \
if(!status.ok) { \
  fprintf(stderr, "%s (errno: %i)\n", errmsg, status.errnum); \
  continue; \
}

//==============================================================================
// Webserver functions
//==============================================================================
void start_server(int port) {
  printf("Starting server on port %i\n", port);
  printf("Initializing\n");

  // Create and initialize the socket
  ServerSocket server;
  Status status;

  status = server_socket_init(&server);
  return_on_error(status, "Error initializing server socket");

  status = server_socket_bind(&server, port);
  return_on_error(status, "Error binding socket to port");

  status = server_socket_listen(&server, MAX_PENDING_CONNS);
  return_on_error(status, "Error having socket listen for incoming connections");

  printf("Server now listening for incoming connections on port %i\n", port);

  // Accept incoming connections and service their requests
  while(1) {
    ClientSocket client;
    client_socket_init(&client);

    // Accept the next connection. On failure, try again.
    status = server_socket_accept(&server, &client);
    continue_on_error(status, "Error accepting incoming connection");

    // Where is the connection coming from?
    printf("Accepted client connection from %s:%i\n",
           client_socket_get_ip(&client),
           client_socket_get_port(&client));

    // Read the incoming data
    status = client_socket_recv(&client);
    continue_on_error(status, "Error reading data from client");

    // Check if data received. If not, log an error.
    if(!client.data) {
      fprintf(stderr, "Got no data from client\n");
    }

    // TODO - debugging output
    printf("------------ received ------------\n");
    printf("%s\n", client.data);
    printf("----------------------------------\n");

    // Parse request, write a response
    HttpRequest request;
    http_request_init(&request);
    http_request_parse(&request, client.data);

    // Do stuff based on request
    // TODO - the meat!!!

    // Clean up
    http_request_free(&request);
    client_socket_close(&client);

    break; // TODO - for now, just handle one request
  }

  // Close the server socket
  server_socket_close(&server);
}
