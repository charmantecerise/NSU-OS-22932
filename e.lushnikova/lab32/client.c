#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>

#define SOCK_PATH "/tmp/socket"
#define BUF_SIZE 1024

int main() {
    int client_fd;
    struct sockaddr_un server_addr;
    char buf[BUF_SIZE];

    if ((client_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCK_PATH, sizeof(server_addr.sun_path) - 1);

    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    fgets(buf, BUF_SIZE, stdin);

    if (send(client_fd, buf, strlen(buf), 0) == -1) {
        perror("send");
        exit(EXIT_FAILURE);
    }

    int recv_len = recv(client_fd, buf, BUF_SIZE, 0);
    if (recv_len == -1) {
        perror("recv");
        exit(EXIT_FAILURE);
    }

    printf("%.*s", recv_len, buf);
    
    close(client_fd);

    return 0;
}