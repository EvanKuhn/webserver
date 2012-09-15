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
typedef struct ProgramOptions {
  int port;
  bool help;
} ProgramOptions;

// Parse program options from the command-line inputs
bool program_options_parse(ProgramOptions* options, int argc, char** argv);

// Flag to tell program_options_parse not to print error messages. For testing.
extern bool silence_program_options_parse;

#endif // PROGRAM_OPTIONS_H