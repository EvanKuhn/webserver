#include "webserver.h"
#include "http_request.h"
#include "http_response.h"
#include "sockets.h"
#include "logging.h"
#include "utils.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

//==============================================================================
// Constants and utilities
//==============================================================================
// Max number of incoming connections that may be queued by the server's socket
static const int MAX_PENDING_CONNS = 100;

// Path to the files to serve   //TODO - make this configurable!
//static const char* FILE_STORAGE_ROOT = "/Users/evan/dev/webserver/sites";

// Check status. On error, print mesage and return from calling function.
#define return_on_error(status, errmsg) \
if(!status.ok) { \
  log_err("%s (errno: %i)", errmsg, status.errnum); \
  return; \
}

//==============================================================================
// Signal handling
//==============================================================================
// Objects for the signal handler to manipulate
static bool keep_running = true;
static ClientSocket* curr_client_socket = NULL;
static ServerSocket* curr_server_socket = NULL;

// Upon receipt of a signal, close sockets and have the webserver stop running
void handle_signal(int sig) {
  keep_running = false;
  log_all("Received signal %i. Shutting down.", sig);
  if(curr_client_socket) {
    client_socket_close(curr_client_socket);
  }
  if(curr_server_socket) {
    server_socket_close(curr_server_socket);
  }
}

//==============================================================================
// Webserver request-handling and response functions
//==============================================================================
// This function routes the request to the proper function below
void webserver_process_request(HttpRequest* request, ClientSocket* client,
                               WebServerConfig* config);

// Handle different HTTP methods, or a bad request
void webserver_process_get    (HttpRequest* request, ClientSocket* client);
void webserver_process_head   (HttpRequest* request, ClientSocket* client);
void webserver_process_post   (HttpRequest* request, ClientSocket* client);
void webserver_process_put    (HttpRequest* request, ClientSocket* client);
void webserver_process_delete (HttpRequest* request, ClientSocket* client);
void webserver_process_error  (HttpRequest* request, ClientSocket* client);

// Echo the request data back to the client. Useful for development/debugging.
void webserver_echo_request   (HttpRequest* request, ClientSocket* client);

// Send an HTTP response
// - Use NULL to indicate no body
// - If content_type is NULL, use "text/plain"
void webserver_send_response(ClientSocket* client, enum EHttpStatus status,
                             const char* body, const char* content_type);

//==============================================================================
// Webserver
//==============================================================================
void webserver_start(WebServerConfig* config) {
  const int port = config->port;
  log_all("Initializing server on port %i", port);

  // Set up signal handler
  signal(SIGINT,  handle_signal);
  signal(SIGKILL, handle_signal);
  signal(SIGTERM, handle_signal);

  // Make sure we can open the log files
  echo_log_to_console(true);
  if(!open_log_files().ok) return;

  // Create and initialize the socket
  Status status;
  ServerSocket server;

  // Set the current server socket so the signal handler can close the socket
  curr_server_socket = &server;

  // Initialize the server socket and start listening for incoming connections
  status = server_socket_init(&server);
  return_on_error(status, "Error initializing server socket");

  status = server_socket_bind(&server, port);
  return_on_error(status, "Error binding socket to port");

  status = server_socket_listen(&server, MAX_PENDING_CONNS);
  return_on_error(status, "Error having socket listen for incoming connections");

  log_std("Server now listening for incoming connections on port %i", port);

  // Accept incoming connections and service their requests
  while(keep_running) {
    // Create the client socket save it so the signal handler can close it
    ClientSocket client;
    client_socket_init(&client);
    curr_client_socket = &client;

    // Accept the next connection. On failure, try again.
    // - If we're shutting down, skip printing the error
    status = server_socket_accept(&server, &client);
    if(!status.ok) {
      if(keep_running) log_err("Error accepting incoming connection (errno: %i)", status.errnum);
      continue;
    }

    // Where is the connection coming from?
    const char* ip = client_socket_get_ip(&client);
    const int port = client_socket_get_port(&client);

    // Read the incoming data
    status = client_socket_recv(&client);
    if(!status.ok) {
      log_err("%s:%i | Error reading data from client (errno: %i)", ip, port, status.errnum);
      client_socket_close(&client);
      continue;
    }

    // Check if data received. If not, log an error.
    if(!client.data) {
      log_err("%s:%i | Got no data from client", ip, port);
      client_socket_close(&client);
      continue;
    }

    // Print the entire request if verbose mode enabled
    if(config->verbose) {
      printf("------------ received ------------\n");
      printf("%s\n", client.data);
      printf("----------------------------------\n");
    }

    // Parse the request
    HttpRequest request;
    http_request_init(&request);
    bool status = http_request_parse(&request, client.data);

    // If parsing succeeded, log a message and process the response
    if(status) {
      const char* method = http_method_to_string(request.method);
      const char* version = http_version_to_string(request.version);
      log_std("%s:%i | %s %s %s", ip, port, method, request.uri, version);
      webserver_process_request(&request, &client, config);
    }
    // If parsing failed, log the error and respond with a 400 / Bad Request
    else {
      log_err("%s:%i | %s", ip, port, request.error);
      webserver_send_response(&client, HTTP_STATUS_BAD_REQUEST, request.error, 0);
    }

    // Clean up: request-handling resources
    curr_client_socket = NULL;
    http_request_free(&request);
    client_socket_close(&client);
  }

  // Clean up: webserver resources
  curr_server_socket = NULL;
  server_socket_close(&server);
  close_log_files();
}

//==============================================================================
// Webserver request-handling functions
//   TODO - reuse shared memory location for writing responses
//   TODO - move to a different file?
//==============================================================================
void webserver_process_request(HttpRequest*     request,
                               ClientSocket*    client,
                               WebServerConfig* config)
{
  // If in echo mode, echo the request info back to the user
  if(config->echo) {
    webserver_echo_request(request, client);
  }
  // Otherwise, respond normally
  else {
    switch(request->method) {
      case HTTP_METHOD_GET:  webserver_process_get  (request, client); break;
      case HTTP_METHOD_HEAD: webserver_process_head (request, client); break;
      case HTTP_METHOD_POST: webserver_process_post (request, client); break;
      case HTTP_METHOD_PUT:  webserver_process_put  (request, client); break;
      default:               webserver_process_error(request, client); break;
    }
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

  webserver_send_response(client, HTTP_STATUS_OK, body, "text/html");
}

void webserver_process_head(HttpRequest* request, ClientSocket* client) {
  // Look up resource and return meta-info via headers
  // - Should be identical to meta-info returned from GET; just w/o a body
  webserver_send_response(client, HTTP_STATUS_OK, 0, 0);
}

void webserver_process_post(HttpRequest* request, ClientSocket* client) {
  // Respond with 501 / Not Implemented
  webserver_send_response(client, HTTP_STATUS_NOT_IMPLEMENTED, 0, 0);
  // NOTES:
  // Get content length
  // - If missing, send 411 / Length Required
  // - If different than message body, send 400 / Bad Request
}

void webserver_process_put(HttpRequest* request, ClientSocket* client) {
  // Respond with 501 / Not Implemented
  webserver_send_response(client, HTTP_STATUS_NOT_IMPLEMENTED, 0, 0);
}

void webserver_process_delete(HttpRequest* request, ClientSocket* client) {
  // Respond with 404 / Not Found
  webserver_send_response(client, HTTP_STATUS_NOT_FOUND, 0, 0);
}

void webserver_process_error(HttpRequest* request, ClientSocket* client) {
  // Respond with 501 / Not Implemented
  webserver_send_response(client, HTTP_STATUS_NOT_IMPLEMENTED, 0, 0);
}

void webserver_echo_request(HttpRequest* request, ClientSocket* client) {
  // Just send back the full HTTP request from the client
  webserver_send_response(client, HTTP_STATUS_OK, client->data, 0);
}

void webserver_send_response(ClientSocket*    client,
                             enum EHttpStatus status,
                             const char*      body,
                             const char*      content_type)
{
  // Build the Content-Length string
  char content_length[20] = {0};
  snprintf(content_length, 20, "%zu", (body ? strlen(body) : 0));
  if(!content_type) content_type = "text/plain";

  // Build the response object
  HttpResponse* res = http_response_new();
  http_response_set_status(res, HTTP_VERSION_1_0, status);
  http_response_add_header(res, "Server", "webserver");
  http_response_add_header(res, "Content-Length", content_length);
  if(body) http_response_add_header(res, "Content-Type", content_type);
  if(body) http_response_set_body(res, body);

  // Send the response and clean up
  client_socket_send(client, http_response_string(res), http_response_length(res));
  http_response_free(res);
}
