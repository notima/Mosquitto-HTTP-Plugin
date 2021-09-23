#define CONFIG_UNKNOWN 1

extern const char* config_server_host;
int config_server_port;
extern const char* config_connect_uri;

int set_config(char* key, char* value);