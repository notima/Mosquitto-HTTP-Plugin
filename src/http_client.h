#ifndef HTTP_CLIENT_H

#define ERR_NO_SOCK -1
#define ERR_CONNECTION -2
#define ERR_HOSTNAME -3

int post(char* endpoint, char* content);

extern char* host;

#define HTTP_CLIENT_H
#endif