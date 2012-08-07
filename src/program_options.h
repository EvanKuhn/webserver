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