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
  errlog("%s (errno: %i)", errmsg, status.errnum); \
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
  stdlog("Received signal %i. Shutting down.\n", sig);
  errlog("Received signal %i. Shutting down.\n", sig);
  if(curr_client_socket) {
    client_socket_close(curr_client_socket);
  }
  if(curr_server_socket) {
    server_socket_close(curr_server_socket);
  }
}

//==============================================================================
// Webserver request-handling functions
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

//==============================================================================
// Webserver
//==============================================================================
void webserver_start(WebServerConfig* config) {
  const int port = config->port;
  stdlog("Initializing server on port %i", port);
  errlog("Initializing server on port %i", port);

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

  stdlog("Server now listening for incoming connections on port %i", port);

  // Accept incoming connections and service their requests
  while(keep_running) {
    // Create the client socket save it so the signal handler can close it
    ClientSocket client;
    client_socket_init(&client);
    curr_client_socket = &client;

    // Accept the next connection. On failure, try again.
    status = server_socket_accept(&server, &client);
    if(!status.ok) {
      errlog("Error accepting incoming connection (errno: %i)", status.errnum);
      continue;
    }

    // Where is the connection coming from?
    const char* ip = client_socket_get_ip(&client);
    const int port = client_socket_get_port(&client);

    // Read the incoming data
    status = client_socket_recv(&client);
    if(!status.ok) {
      errlog("%s:%i | Error reading data from client (errno: %i)", ip, port, status.errnum);
      continue;
    }

    // Check if data received. If not, log an error.
    if(!client.data) {
      errlog("%s:%i | Got no data from client", ip, port);
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
    http_request_parse(&request, client.data);

    // Log what we got
    const char* method = http_method_to_string(request.method);
    const char* version = http_version_to_string(request.version);
    stdlog("%s:%i | %s %s %s", ip, port, method, request.uri,version);

    // Process the request and send a response
    webserver_process_request(&request, &client, config);

    // Clean up
    curr_client_socket = NULL;
    http_request_free(&request);
    client_socket_close(&client);

    //break; // TODO - for now, just handle one request
  }

  // Clean up
  curr_server_socket = NULL;
  server_socket_close(&server);
  close_log_files();
}

//==============================================================================
// Webserver request-handling functions
//   TODO - reuse shared memory location for writing responses
//   TODO - move to a different file?
//==============================================================================
void webserver_process_request(HttpRequest* request, ClientSocket* client,
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

void webserver_echo_request(HttpRequest* request, ClientSocket* client) {
  // Build the response body
  char bodylen[20];
  bzero(bodylen, 20);
  snprintf(bodylen, 20, "%zu", strlen(client->data));

  // Build the response object
  HttpResponse* res = http_response_new();
  http_response_set_status(res, HTTP_VERSION_1_0, HTTP_STATUS_OK);
  http_response_add_header(res, "Server", "webserver");
  http_response_add_header(res, "Content-Length", bodylen);
  http_response_add_header(res, "Content-Type", "text/plain");
  http_response_set_body(res, client->data);

  // Send the response and clean up
  client_socket_send(client, http_response_string(res), http_response_length(res));
  http_response_free(res);
}