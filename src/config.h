#ifndef MOSQ_HTTP__CONFIG_H
#define MOSQ_HTTP__CONFIG_H

#define CONFIG_UNKNOWN 1

extern const char* config_server_host;
extern int config_server_port;
extern const char* config_connect_uri;
extern const char* config_acl_uri;

/**
 * Set one of the supported config options by name
 */
int set_config(char* key, char* value);

#endif