#ifndef MOSQ_HTTP__CONFIG_H
#define MOSQ_HTTP__CONFIG_H

#define CONFIG_UNKNOWN 1
#define CONFIG_INVALID 2

extern const char* config_server_host;
extern int config_server_port;
extern const char* config_connect_uri;
extern const char* config_acl_uri;
// How long an /acl answer is remembered, in seconds. 0 turns the cache off.
extern int config_acl_cache_seconds;
// How many /acl answers are remembered at most.
extern int config_acl_cache_size;
// How many denied access checks that needed an /acl request a username may cause in a
// minute before its clients are disconnected. 0 turns the limit off.
extern int config_acl_max_denials_per_minute;

/**
 * Set one of the supported config options by name
 */
int set_config(char* key, char* value);

#endif