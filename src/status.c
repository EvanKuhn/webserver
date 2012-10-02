#include "status.h"
#include <errno.h>

Status make_status(bool ok, int errnum) {
  Status s = { ok, errnum };
  return s;
}

Status get_status(bool ok) {
  Status s = { ok, (ok ? 0 : errno) };
  return s;
}
