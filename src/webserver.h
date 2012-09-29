//==============================================================================
// Webserver interface
//
// Evan Kuhn 2012-09-09
//==============================================================================
#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "webserver_config.h"

// Start the webserver listening on a given port
void webserver_start(WebServerConfig* config);

#endif // WEBSERVER_H