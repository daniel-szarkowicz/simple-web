#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <assert.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "http.h"
#include "request.h"
#include "response.h"

void handle_client(int fd) {
    Request req = {0};
    HttpStatus error = parse_request(fd, &req);
    if (error != NO_STATUS) return onlystatus(fd, error);
    respond(fd, &req);
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    int option = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(60000),
        .sin_addr.s_addr = inet_addr("127.0.0.1"),
    };
    int err =
        bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (err != 0) {
        perror("Failed to bind socket");
        exit(1);
    }

    err = listen(server_fd, 100);
    if (err != 0) {
        perror("Could not listen to socket");
        exit(1);
    }

    for (int i = 0; i < 100; ++i) {
        // struct sockaddr_in client_addr;
        // socklen_t client_addrlen;
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) {
            perror("Could not accept connection");
            exit(1);
        }
        // assert(client_addrlen == sizeof(client_addr));

        handle_client(client_fd);

        shutdown(client_fd, SHUT_RDWR);
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
