#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>

int SERVER_PORT = 80;

int main(int argc, char** argv) {
  printf("Initializing\n");
  int result = 0;
  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));

  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if(server_socket == -1) {
    perror("Error creating socket");
    return 1;
  }

  // Initialize the socket description
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(SERVER_PORT);

  // bind the socket to the port specified above
  result = bind(server_socket,
                (struct sockaddr*)&server_addr,
                sizeof(server_addr));
  if(result == -1) {
    perror("Unable to bind socket");
    return 1;
  }

  // Listen for incoming connections
  result = listen(server_socket, 5);
  if(result == -1) {
    perror("Socket unable to listen");
    return 1;
  }

  // Accept incoming connections
  printf("Listening on port %i\n", ntohs(server_addr.sin_port));
  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  int client_socket = accept(server_socket,
                             (struct sockaddr*)&client_addr,
                             &client_len);
  if(client_socket == -1) {
    perror("Unable to accept client socket\n");
    return 1;
  }
  printf("Got client socket on port %i", ntohs(client_addr.sin_port));

  // Read data from the socket
  char data[2048] = {0};
  if(recv(client_socket, data, 2048, 0) < 0) {
    perror("Error getting data from socket");
    return 1;
  }
  printf("Got data: %s\n", data);

  // Echo the data back
  if(send(client_socket, data, strlen(data), 0) < 0) {
    perror("Error sending data back to client");
    return 1;
  }
  printf("Echoed data back to client\n");

  printf("bye\n");
  return 0;
}
