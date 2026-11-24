#ifndef HTTP_CLIENT_H

#define ERR_NO_SOCK -1
#define ERR_CONNECTION -2
#define ERR_HOSTNAME -3
#define ERR_NO_MEMORY -4
#define ERR_BAD_RESPONSE -5

/**
 * Send a POST request to the endpoint with content as body
 * config_server_host & config_server_port will be used to connect to the server.
 */
int post(const char* endpoint, const char* content);

#define HTTP_CLIENT_H
#endif