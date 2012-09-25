#include "webserver.h"
#include "http_request.h"
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

// TODO - we probably want to have a single memory location for writing responses,
// so that we don't need to allocate such memory with each request.

void webserver_process_get(HttpRequest* request, ClientSocket* client) {
  printf("Processing GET\n");
  // Look up resource and return it
  // - If found, return 200 / OK
  // - If not, return 404 / Not Found

  // Append request URI to FILE_STORAGE_ROOT
  // If file exists, return it with 200 / OK
  // Else file is missing, so return 404 / Not Found


  StringBuffer body;
  string_buffer_init(&body, 1024);
  string_buffer_append(&body, "<html>");
  string_buffer_append(&body, "<head></head>");
  string_buffer_append(&body, "<body>");
  string_buffer_append(&body, "<p>Hello World!</p>");
  string_buffer_append(&body, "</body>");
  string_buffer_append(&body, "</html>");

  char bodylen[20];
  bzero(bodylen, 20);
  snprintf(bodylen, 20, "%zu", body.size);

  StringBuffer res;
  string_buffer_init(&res, 1024);
  string_buffer_append(&res, "HTTP/1.1 200 OK\n");
  //string_buffer_append(&res, "Server: webserver\r\f");
  string_buffer_append(&res, "Content-Length: ");
  string_buffer_append(&res, bodylen);
  string_buffer_append(&res, "\n");
  string_buffer_append(&res, "Content-Type: text/html\n");
  string_buffer_append(&res, "\n");
  string_buffer_append(&res, body.chars);

  printf("%s\n", res.chars);
  printf("chars length = %zu\n", strlen(res.chars));
  printf("res.size = %zu\n", res.size);

  client_socket_send(client, res.chars, res.size);
  sleep(1);
  string_buffer_free(&res);
  string_buffer_free(&body);
}

void webserver_process_head(HttpRequest* request, ClientSocket* client) {
  printf("Processing HEAD\n");
  // Look up resource and return meta-info via headers
  // - Should be identical to meta-info returned from GET; just w/o a body

  StringBuffer buf;
  string_buffer_init(&buf, 128);
  string_buffer_append(&buf, "HTTP/1.0 200 OK\r\f");
  string_buffer_append(&buf, "Server: webserver\r\f");
  string_buffer_append(&buf, "\r\f");
  client_socket_send(client, buf.chars, buf.size);
  string_buffer_free(&buf);

  //Example from "curl http://www.sinatrarb.com":
  /*
  < HTTP/1.1 200 OK
  < Server: nginx
  < Date: Mon, 24 Sep 2012 02:10:53 GMT
  < Content-Type: text/html
  < Content-Length: 2525
  < Last-Modified: Tue, 18 Sep 2012 02:39:45 GMT
  < Connection: keep-alive
  < Expires: Tue, 25 Sep 2012 02:10:53 GMT
  < Cache-Control: max-age=86400
  < Accept-Ranges: bytes
  */
}

void webserver_process_post(HttpRequest* request, ClientSocket* client) {
  printf("Processing POST\n");

  // Respond with 501 / Not Implemented
  // Get content length
  // - If missing, send 411 / Length Required
  // - If different than message body, send 400 / Bad Request
  StringBuffer buf;
  string_buffer_init(&buf, 128);
  string_buffer_append(&buf, "HTTP/1.0 501 / Not Implemented\r\f\r\f");
  client_socket_send(client, buf.chars, buf.size);
  string_buffer_free(&buf);
}

void webserver_process_put(HttpRequest* request, ClientSocket* client) {
  printf("Processing PUT\n");

  // Respond with 501 / Not Implemented
  StringBuffer buf;
  string_buffer_init(&buf, 128);
  string_buffer_append(&buf, "HTTP/1.0 501 / Not Implemented\r\f\r\f");
  client_socket_send(client, buf.chars, buf.size);
  string_buffer_free(&buf);
}

void webserver_process_delete(HttpRequest* request, ClientSocket* client) {
  printf("Processing DELETE\n");

  // Respond with 404 / Not Found
  StringBuffer buf;
  string_buffer_init(&buf, 128);
  string_buffer_append(&buf, "HTTP/1.0 404 / Not Found\r\f\r\f");
  client_socket_send(client, buf.chars, buf.size);
  string_buffer_free(&buf);
}

void webserver_process_bad(HttpRequest* request, ClientSocket* client) {
  printf("Processing bad request\n");

  // Respond with 501 / Not Implemented
  StringBuffer buf;
  string_buffer_init(&buf, 128);
  string_buffer_append(&buf, "HTTP/1.0 501 / Not Implemented\r\f\r\f");
  client_socket_send(client, buf.chars, buf.size);
  string_buffer_free(&buf);
}

// This function serves as a router, calling the request-handler function for
// the given HTTP request method.
void webserver_process_request(HttpRequest* request, ClientSocket* client) {
  switch(request->method) {
    case HTTP_METHOD_GET:  webserver_process_get (request, client); break;
    case HTTP_METHOD_HEAD: webserver_process_head(request, client); break;
    case HTTP_METHOD_POST: webserver_process_post(request, client); break;
    case HTTP_METHOD_PUT:  webserver_process_put (request, client); break;
    default:               webserver_process_bad (request, client); break;
  }
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
