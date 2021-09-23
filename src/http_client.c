#include <http_client.h>
#include <mosquitto.h>
#include <config.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 512

void create_request(char* endpoint, char* content, char* request) {
    char methodLine[128];
    char hostLine[128];
    char contentTypeLine[128];
    char contentLengthLine[128];

    sprintf(methodLine, "POST %s HTTP/1.1", endpoint);
    sprintf(hostLine, "Host: %s", config_server_host);
    sprintf(contentTypeLine, "Content-Type: application/json");
    sprintf(contentLengthLine, "Content-Length: %d", strlen(content));

    sprintf(
        request,
        "%s\r\n%s\r\n%s\r\n%s\r\n\r\n%s",
        methodLine,
        hostLine,
        contentTypeLine,
        contentLengthLine,
        content
    );

    return request;
}

int open_socket() {
    int on = 1;
    int timeout = 10;
    int sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct hostent *hp;
	struct sockaddr_in addr;
    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(int));
    setsockopt(sock, IPPROTO_TCP, TCP_USER_TIMEOUT, (const char *)&timeout, sizeof(int));

    if(sock == -1){
		return ERR_NO_SOCK;
	}

    if((hp = gethostbyname(config_server_host)) == NULL){
		return ERR_HOSTNAME;
	}
	bcopy(hp->h_addr, &addr.sin_addr, hp->h_length);
	addr.sin_port = htons(config_server_port);
	addr.sin_family = AF_INET;

    if(connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1){
		return ERR_CONNECTION;
	}

    return sock;
}

int status_from_response(char* response) {
    char response_arr[BUF_SIZE];
    strcpy(response_arr, response);
    char *protocol = strtok(response_arr, " ");
    char *status = strtok(NULL, " ");
    return atoi(status);
}

int post(char* endpoint, char* content) {
    char request[BUF_SIZE];
    create_request(endpoint, content, request);
    int sock = open_socket(config_server_host);

    if(sock < 0) {
        return sock;
    }

    write(sock, request, strlen(request));

    char response[BUF_SIZE];
    bzero(response, BUF_SIZE);
    read(sock, response, BUF_SIZE);

    return status_from_response(response);
}