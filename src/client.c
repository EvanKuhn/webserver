#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char** argv) {
  // Create a socket
  int client_socket = socket(AF_INET, SOCK_STREAM, 0);
  if(client_socket == -1) {
    perror("Couldn't create socket");
    return 1;
  }

  // Set up the server address
  struct sockaddr_in server;
  server.sin_addr.s_addr = inet_addr("127.0.0.1");
  server.sin_port = htons(7000);
  server.sin_family = AF_INET;

  // Connect to the server
  if(connect(client_socket, (struct sockaddr*)&server, sizeof(server)) < 0) {
    perror("Unable to connect to server");
    return 1;
  }

  // Send data
  const char* data = "Hey there!";
  printf("Sending: %s\n", data);
  if(send(client_socket, data, strlen(data), 0) < 0) {
    perror("Unable to send data");
    return 1;
  }

  // Receive reply from server
  char reply[2048] = {0};
  if(recv(client_socket, reply, 2048, 0) < 0) {
    perror("Error getting server reply");
    return 1;
  }

  // Print reply
  printf("Received: %s\n", reply);

  return 0;
}
