#include <stdio.h>
#include <string.h>      # strlen
#include <unistd.h>      # close
#include <sys/socket.h>
#include <arpa/inet.h>   # inet_addr

int SERVER_PORT = 80;

int main(int argc, char** argv) {
  int socket_desc = -1;
  struct sockaddr_in server;

  // Create socket
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);
  if(socket_desc == -1) {
    puts("Could not create socket");
    return 1;
  }

  // Set up the address to connect to (host, port, type)
  server.sin_addr.s_addr = inet_addr("127.0.0.1");
  server.sin_port = htons(SERVER_PORT);
  server.sin_family = AF_INET;

  // Connect to remote server
  if(connect(socket_desc, (struct sockaddr*)&server, sizeof(server)) < 0) {
    puts("Connect error");
    return 1;
  }

  puts("Connected");

  // Send some data
  const char* message = "GET /~evan/hi.php HTTP/1.0\r\n\r\n";
  if(send(socket_desc, message, strlen(message), 0) < 0) {
    puts("Send failed");
    goto return_failure;
  }
  puts("Data Sent");

  // Receive reply from server
  char server_reply[2000];
  if(recv(socket_desc, server_reply, 2000, 0) < 0) {
    puts("Recv failed");
    goto return_failure;
  }
  puts("Reply received:");
  puts("----------------------------------");
  puts(server_reply);
  puts("----------------------------------");

  // Close the socket and exit
  if(socket_desc != -1) close(socket_desc);
  return 0;
return_failure:
  if(socket_desc != -1) close(socket_desc);
  return 1;
}
