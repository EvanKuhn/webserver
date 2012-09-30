#include "webserver_config.h"

void webserver_config_init(WebServerConfig* conf) {
  conf->port = 80;
  conf->verbose = false;
}

// TODO - we need a 3-step process to get configuration data:
//
//     1) Start with defaults
//     2) Apply data from config file(s)
//     3) Apply data from the command line
//
// So we could have:
//
//     void webserver_config_apply_defaults();
//     void webserver_config_read_from_file();
//     void webserver_config_copy_from_cmdline();
//
