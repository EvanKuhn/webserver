//==============================================================================
// Logging functions. Log messages are written to either a 'standard' or 'error'
// log file.
//
// - All non-error messages are written to the standard log.
// - All error messages are written to the error log.
// - Webserver start and stop messages are written to both logs.
//
// Evan Kuhn, 2012-10-08
//==============================================================================
#ifndef LOGGING_H
#define LOGGING_H

#include "status.h"
#include <stdio.h>

// Tell the logging system to echo log messages to the console.
// - Messages will still be written to the log files.
// - Can be called at any time.
// - On by default.
void echo_log_to_console(bool echo);

// Open log files.
// - Prints an error message and return a status if unable to open the files.
// - It's not necessary to call this explicitly before logging, but it's a good
//   idea, just to make sure the process can open the files.
Status open_log_files();

// Close log files. Ignores errors.
void close_log_files();

// Logging functions
// - If echo-to-console is enabled, these will also log to stdout or stderr
void log_std(const char* format, ...);  // Log to stdout and 'standard' file
void log_err(const char* format, ...);  // Log to stderr and 'error' file
void log_all(const char* format, ...);  // Log to stdout and both files

#endif // LOGGING_H