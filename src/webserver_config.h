//==============================================================================
// Webserver configuration data
//
// Evan Kuhn 2012-09-29
//==============================================================================
#ifndef WEBSERVER_CONFIG_H
#define WEBSERVER_CONFIG_H

#include <stdbool.h>

typedef struct WebServerConfig {
  int port;
  bool verbose;
} WebServerConfig;

// Initialize the config object by setting defaults
void webserver_config_init(WebServerConfig* conf);

#endif // WEBSERVER_CONFIG_H
