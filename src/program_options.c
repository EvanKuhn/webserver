#include "program_options.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

bool silence_program_options_parse = false;

const char* program_options_usage() {
  return
  "\n"
  "OPTIONS:\n"
  "  -p <port>    Set the port to listen on\n"
  "  -v           Enable verbose output\n"
  "  -e           Echo the request, for debugging\n"
  "  -h           Show this help message\n"
  "\n"
  ;
}

void program_options_print(ProgramOptions* options) {
  printf("Options\n");
  printf(" - port:    %i\n", options->config.port);
  printf(" - verbose: %s\n", options->config.verbose ? "yes" : "no");
  printf(" - echo:    %s\n", options->config.echo ? "yes" : "no");
}

bool program_options_parse(ProgramOptions* options, int argc, char** argv) {
  // Tell getopt() not to print error messages
  opterr = 0;

  // Reset the option-parsing index (so tests can use getopt more than once)
  optind = 1;

  // Set defaults
  options->help = false;
  webserver_config_init(&options->config);

  // Parse command-line inputs
  char c = 0;
  while((c = getopt(argc, argv, "p:veh")) != -1) {
    switch(c) {
    case 'p':
      options->config.port = atoi(optarg);
      break;
    case 'v':
      options->config.verbose = true;
      break;
    case 'e':
      options->config.echo = true;
      break;
    case 'h':
      options->help = true;
      break;
    case '?':
      if(!silence_program_options_parse) {
        if(optopt == 'p') {
          fprintf(stderr, "ERROR: Option -p requires an argument\n");
        }
        else if(isprint(optopt)) {
          fprintf(stderr, "ERROR: Unknown option '-%c'\n", optopt);
        }
        else {
          fprintf(stderr, "ERROR: Unknown option character '\\x%x'\n", optopt);
        }
      }
      return false;
    }
  }
  return true;
}
