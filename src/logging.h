//==============================================================================
// Logging functions. Log messages are written to either a 'standard' or 'error'
// log file.
//
// Evan Kuhn, 2012-10-08
//==============================================================================
#ifndef LOGGING_H
#define LOGGING_H

#include "status.h"

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

// Log to the 'standard' or 'error' log file
void stdlog(const char* format, ...);
void errlog(const char* format, ...);

#endif // LOGGING_H