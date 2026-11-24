#include <http_client.h>
#include <mosquitto.h>
#include <config.h>
#include <json.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 512

// Returns the full request, allocated to fit; the caller frees it. NULL on failure.
char *create_request(const char* endpoint, const char* content) {
    return format_alloc(
        "POST %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %zu\r\n"
        "\r\n"
        "%s",
        endpoint,
        config_server_host,
        strlen(content),
        content
    );
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
        close(sock);
        return ERR_HOSTNAME;
    }

    bcopy(hp->h_addr, &addr.sin_addr, hp->h_length);
    addr.sin_port = htons(config_server_port);
    addr.sin_family = AF_INET;

    if(connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1){
        close(sock);
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

int post(const char* endpoint, const char* content) {
    char *request = create_request(endpoint, content);
    if(request == NULL) {
        return ERR_NO_MEMORY;
    }
    int sock = open_socket(config_server_host);

    if(sock < 0) {
        free(request);
        return sock;
    }

    // write() may send less than asked for, which a long (escaped) body makes more likely.
    size_t length = strlen(request);
    size_t sent = 0;
    while(sent < length) {
        ssize_t written = write(sock, request + sent, length - sent);
        if(written <= 0) {
            free(request);
            close(sock);
            return ERR_CONNECTION;
        }
        sent += (size_t)written;
    }
    free(request);

    char response[BUF_SIZE];
    bzero(response, BUF_SIZE);
    read(sock, response, BUF_SIZE);
    close(sock);

    return status_from_response(response);
}
