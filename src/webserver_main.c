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

int main(int argc, char** argv) {
  // Get options
  ProgramOptions options;
  if(!program_options_parse(&options, argc, argv)) {
    fprintf(stderr, "%s", program_options_usage());
    exit(1);
  }
  if(options.help) {
    printf("%s", program_options_usage());
    exit(0);
  }

  // Print options
  program_options_print(&options);

  // Start the webserver
  webserver_start(&options.config);
}
