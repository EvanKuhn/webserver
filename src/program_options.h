//==============================================================================
// Struct containing webserver command-line options, plus a function for parsing
// the command-line args and populating the struct.
//
// Evan Kuhn 2012-09-09
//==============================================================================
#ifndef PROGRAM_OPTIONS_H
#define PROGRAM_OPTIONS_H

#include <stdbool.h>

// ProgramOptions struct
typedef struct {
  int port;
  bool help;
} ProgramOptions;

// Parse program options from the command-line inputs
bool parse_options(int argc, char** argv, ProgramOptions* options);

// Flag to tell parse_options not to print error messages. For testing.
extern bool silence_parse_options;

#endif // PROGRAM_OPTIONS_H