//==============================================================================
// Entry point for webserver program. Parses command-line inputs, prints usage
// info, and runs the webserver.
//
// Evan Kuhn 2012-09-09
//==============================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "program_options.h"
#include "webserver.h"

const char* usage() {
  return
  "\n"
  "OPTIONS:\n"
  "  -p <port>    Set the port to listen on\n"
  "  -h           Show this help message\n"
  "\n"
  ;
}

int main(int argc, char** argv) {
  // Get options
  ProgramOptions options;
  if(!parse_options(argc, argv, &options)) {
    fprintf(stderr, "%s", usage());
    exit(1);
  }
  if(options.help) {
    printf("%s", usage());
    exit(0);
  }

  // Print options
  printf("Options\n");
  printf(" - port = %i\n", options.port);

  // Start server
  start_server(options.port);
}
