#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/my_unix_socket"
#define MESSAGE "Some text"

int main() {
    int client_socket;
    struct sockaddr_un server_address;
    ssize_t bytes_sent;

    client_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_address, 0, sizeof(struct sockaddr_un));
    server_address.sun_family = AF_UNIX;
    strncpy(server_address.sun_path, SOCKET_PATH, sizeof(server_address.sun_path) - 1);

    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(struct sockaddr_un)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    printf("Sending: %s\n", MESSAGE);

    bytes_sent = write(client_socket, MESSAGE, strlen(MESSAGE));
    if (bytes_sent == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    close(client_socket);  

    return 0;
}

