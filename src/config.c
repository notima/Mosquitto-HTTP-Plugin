#include "config.h"
#include <string.h>
#include <stdlib.h>
#include <limits.h>


#define CONFIG_HOST "http_host"
#define CONFIG_PORT "http_port"
#define CONFIG_CONNECT_URI "http_connect_uri"
#define CONFIG_ACL_URI "http_acl_uri"
#define CONFIG_ACL_CACHE_SECONDS "acl_cache_seconds"
#define CONFIG_ACL_CACHE_SIZE "acl_cache_size"
#define CONFIG_ACL_MAX_DENIALS "acl_max_denials_per_minute"

const char* config_server_host = "localhost";
int config_server_port = 80;
const char* config_connect_uri = "/auth";
const char* config_acl_uri = "/acl";
int config_acl_cache_seconds = 60;
int config_acl_cache_size = 10000;
int config_acl_max_denials_per_minute = 1000;

// A whole number of at least 0, or CONFIG_INVALID (leaving *result as it was).
static int parse_count(const char *value, int *result) {
    char *end;
    long parsed = strtol(value, &end, 10);
    if(end == value || *end != '\0' || parsed < 0 || parsed > INT_MAX) {
        return CONFIG_INVALID;
    }
    *result = (int)parsed;
    return 0;
}

int set_config(char* key, char* value) {
    if(strcmp(key, CONFIG_HOST) == 0){
        config_server_host = value;
        return 0;
    }
    if(strcmp(key, CONFIG_PORT) == 0){
        config_server_port = atoi(value);
        return 0;
    }
    if(strcmp(key, CONFIG_CONNECT_URI) == 0){
        config_connect_uri = value;
        return 0;
    }
    if(strcmp(key, CONFIG_ACL_URI) == 0){
        config_acl_uri = value;
        return 0;
    }
    if(strcmp(key, CONFIG_ACL_CACHE_SECONDS) == 0){
        return parse_count(value, &config_acl_cache_seconds);
    }
    if(strcmp(key, CONFIG_ACL_CACHE_SIZE) == 0){
        return parse_count(value, &config_acl_cache_size);
    }
    if(strcmp(key, CONFIG_ACL_MAX_DENIALS) == 0){
        return parse_count(value, &config_acl_max_denials_per_minute);
    }
    return CONFIG_UNKNOWN;
}