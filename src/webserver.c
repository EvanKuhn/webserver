#include "webserver.h"
#include "http_request.h"
#include "http_response.h"
#include "sockets.h"
#include "utils.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//==============================================================================
// Constants and utilities
//==============================================================================
// Max number of incoming connections that may be queued by the server's socket
const int MAX_PENDING_CONNS = 100;

// Path to the files to serve   //TODO - make this configurable!
const char* FILE_STORAGE_ROOT = "/Users/evan/dev/webserver/sites";

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
// Webserver request-handling functions
//==============================================================================
// This function routes the request to the proper function below
void webserver_process_request(HttpRequest* request, ClientSocket* client);
void webserver_process_get    (HttpRequest* request, ClientSocket* client);
void webserver_process_head   (HttpRequest* request, ClientSocket* client);
void webserver_process_post   (HttpRequest* request, ClientSocket* client);
void webserver_process_put    (HttpRequest* request, ClientSocket* client);
void webserver_process_delete (HttpRequest* request, ClientSocket* client);
void webserver_process_error  (HttpRequest* request, ClientSocket* client);

//==============================================================================
// Webserver
//==============================================================================
void webserver_start(WebServerConfig* config) {
  printf("Initializing webserver\n");
  const int port = config->port;
  //const bool verbose = config->verbose;

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

    // Parse the request
    HttpRequest request;
    http_request_init(&request);
    http_request_parse(&request, client.data);
    http_request_print(&request); //TODO

    // Process the request and send a response
    webserver_process_request(&request, &client);

    // Clean up
    http_request_free(&request);
    client_socket_close(&client);

    break; // TODO - for now, just handle one request
  }

  // Close the server socket
  server_socket_close(&server);
}

//==============================================================================
// Webserver request-handling functions
//   TODO - reuse shared memory location for writing responses
//   TODO - move to a different file?
//==============================================================================
void webserver_process_request(HttpRequest* request, ClientSocket* client) {
  switch(request->method) {
    case HTTP_METHOD_GET:  webserver_process_get  (request, client); break;
    case HTTP_METHOD_HEAD: webserver_process_head (request, client); break;
    case HTTP_METHOD_POST: webserver_process_post (request, client); break;
    case HTTP_METHOD_PUT:  webserver_process_put  (request, client); break;
    default:               webserver_process_error(request, client); break;
  }
}

void webserver_process_get(HttpRequest* request, ClientSocket* client) {
  // Look up resource and return it
  // - If found, return 200 / OK
  // - If not, return 404 / Not Found

  // Append request URI to FILE_STORAGE_ROOT
  // If file exists, return it with 200 / OK
  // Else file is missing, so return 404 / Not Found

  const char* body =
    "<html>"
    "<head></head>"
    "<body><p>Hello World!</p></body>"
    "</html>\n";

  char bodylen[20];
  bzero(bodylen, 20);
  snprintf(bodylen, 20, "%zu", strlen(body));

  HttpResponse* res = http_response_new();
  http_response_set_status(res, HTTP_VERSION_1_0, HTTP_STATUS_OK);
  http_response_add_header(res, "Content-Length", bodylen);
  http_response_add_header(res, "Content-Type", "text/html");
  http_response_add_header(res, "Server", "webserver");
  http_response_set_body(res, body);

  client_socket_send(client, http_response_string(res), http_response_length(res));
  http_response_free(res);
}

void webserver_process_head(HttpRequest* request, ClientSocket* client) {
  // Look up resource and return meta-info via headers
  // - Should be identical to meta-info returned from GET; just w/o a body
  HttpResponse* res = http_response_new();
  http_response_set_status(res, HTTP_VERSION_1_0, HTTP_STATUS_OK);
  http_response_add_header(res, "Server", "webserver");
  client_socket_send(client, http_response_string(res), http_response_length(res));
  http_response_free(res);
}

void webserver_process_post(HttpRequest* request, ClientSocket* client) {
  // Respond with 501 / Not Implemented
  // Get content length
  // - If missing, send 411 / Length Required
  // - If different than message body, send 400 / Bad Request
  HttpResponse* res = http_response_new();
  http_response_set_status(res, HTTP_VERSION_1_0, HTTP_STATUS_NOT_IMPLEMENTED);
  http_response_add_header(res, "Server", "webserver");
  client_socket_send(client, http_response_string(res), http_response_length(res));
  http_response_free(res);
}

void webserver_process_put(HttpRequest* request, ClientSocket* client) {
  // Respond with 501 / Not Implemented
  HttpResponse* res = http_response_new();
  http_response_set_status(res, HTTP_VERSION_1_0, HTTP_STATUS_NOT_IMPLEMENTED);
  http_response_add_header(res, "Server", "webserver");
  client_socket_send(client, http_response_string(res), http_response_length(res));
  http_response_free(res);
}

void webserver_process_delete(HttpRequest* request, ClientSocket* client) {
  // Respond with 404 / Not Found
  HttpResponse* res = http_response_new();
  http_response_set_status(res, HTTP_VERSION_1_0, HTTP_STATUS_NOT_FOUND);
  http_response_add_header(res, "Server", "webserver");
  client_socket_send(client, http_response_string(res), http_response_length(res));
  http_response_free(res);
}

void webserver_process_error(HttpRequest* request, ClientSocket* client) {
  // Respond with 501 / Not Implemented
  HttpResponse* res = http_response_new();
  http_response_set_status(res, HTTP_VERSION_1_0, HTTP_STATUS_NOT_IMPLEMENTED);
  http_response_add_header(res, "Server", "webserver");
  client_socket_send(client, http_response_string(res), http_response_length(res));
  http_response_free(res);
}
