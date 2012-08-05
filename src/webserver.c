#include <stdio.h>
#include <string.h>      # strlen
#include <unistd.h>      # close
#include <sys/socket.h>
#include <arpa/inet.h>   # inet_addr
#include "program_options.h"


int main(int argc, char** argv) {
  // Get options
  ProgramOptions options;
  parse_options(argc, argv, &options);

  // Print options
  printf("Options\n");
  printf(" - port = %i\n", options.port);
}
