//==============================================================================
// Simple status struct that contains a status flag and associated error number.
// The error number is meant to store 'errno' values, but can be used for any
// error values.
//
// Evan Kuhn, 2012-09-09
//==============================================================================
#ifndef STATUS_H
#define STATUS_H

#include <stdbool.h>

typedef struct Status {
  bool ok;     // Bool indicating success or failure
  int errnum;  // Error number indicating type of failure. Often set to errno.
} Status;

// Create a new Status object with 'ok' and 'errnum' values as specified
Status make_status(bool ok, int errnum);

// Create a new Status object
// - If 'ok' is true, set 'errnum' to 0
// - If 'ok' is false, set 'errnum' to errno
Status get_status(bool ok);

#endif // STATUS_H