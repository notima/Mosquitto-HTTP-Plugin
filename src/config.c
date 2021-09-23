#include "config.h"
#include "string.h"

#define CONFIG_HOST "http_host"
#define CONFIG_PORT "http_port"
#define CONFIG_CONNECT_URI "http_connect_uri"

const char* config_server_host = "localhost";
int config_server_port = 80;
const char* config_connect_uri = "/auth";

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
    return CONFIG_UNKNOWN;
}