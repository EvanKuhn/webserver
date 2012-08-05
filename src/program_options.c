#include "program_options.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

bool parse_options(int argc, char** argv, ProgramOptions* options) {
  // Tell getopt() not to print error messages
  opterr = 0;

  // Reset the option-parsing index (so tests can use getopt more than once)
  optind = 1;

  // Set defaults
  options->port = 80;
  options->help = false;

  // Parse command-line inputs
  char c = 0;
  while((c = getopt(argc, argv, "hp:")) != -1) {
    switch(c) {
    case 'h':
      options->help = true;
      break;
    case 'p':
      options->port = atoi(optarg);
      break;
    case '?':
      if(optopt == 'p')
        fprintf(stderr, "ERROR: Option -p requires an argument\n");
      else if(isprint(optopt))
        fprintf(stderr, "ERROR: Unknown option '-%c'\n", optopt);
      else
        fprintf(stderr, "ERROR: Unknown option character '\\x%x'\n", optopt);
      return false;
    }
  }
  return true;
}
