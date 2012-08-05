#include "program_options.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

bool parse_options(int argc, char** argv, ProgramOptions* options) {
  // Set defaults
  options->port = 80;

  // Parse command-line inputs
  char c = 0;
  while((c = getopt(argc, argv, "p:")) != -1) {
    switch(c) {
    case 'p':
      options->port = atoi(optarg);
      break;
    case '?':
      if(isprint(optopt)) {
        fprintf(stderr, "Unknown option `-%c'.\n", optopt);
      }
      else {
        fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
      }
      return false;
    }
  }
  return true;
}
