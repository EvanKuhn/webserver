#include "webserver_config.h"

void webserver_config_init(WebServerConfig* conf) {
  conf->port = 80;
  conf->verbose = false;
}
