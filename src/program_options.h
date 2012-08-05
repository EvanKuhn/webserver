#ifndef PROGRAM_OPTIONS_H
#define PROGRAM_OPTIONS_H

#include <stdbool.h>

typedef struct {
  int port;
  bool help;
} ProgramOptions;

bool parse_options(int argc, char** argv, ProgramOptions* options);

#endif // PROGRAM_OPTIONS_H