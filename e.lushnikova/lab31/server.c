#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>
#include <sys/time.h>

#define SOCKET_PATH "/tmp/my_unix_socket"
#define MAX_CLIENTS 5
#define MESSAGE_SIZE 100

int main() {
    int server_socket, client_sockets[MAX_CLIENTS], max_sd, activity, sd, new_socket;
    struct sockaddr_un server_address, client_address;
    char message_buffer[MESSAGE_SIZE];

    socklen_t client_len;
    fd_set readfds;

    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_address, 0, sizeof(struct sockaddr_un));
    server_address.sun_family = AF_UNIX;
    strncpy(server_address.sun_path, SOCKET_PATH, sizeof(server_address.sun_path) - 1);

    unlink(SOCKET_PATH);

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(struct sockaddr_un)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server is waiting for connections...\n");

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(server_socket, &readfds);
        max_sd = server_socket;

        for (int i = 0; i < MAX_CLIENTS; i++) {
            sd = client_sockets[i];

            if (sd > 0) {
                FD_SET(sd, &readfds);
            }

            if (sd > max_sd) {
                max_sd = sd;
            }
        }

        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if (activity < 0) {
            perror("select");
        }

        if (FD_ISSET(server_socket, &readfds)) {
            if ((new_socket = accept(server_socket, (struct sockaddr*)&client_address, (socklen_t*)&client_len)) < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            printf("New client connected\n");

            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = new_socket;
                    break;
                }
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
            sd = client_sockets[i];

            if (FD_ISSET(sd, &readfds)) {
                int bytes_received = read(sd, message_buffer, MESSAGE_SIZE);
                if (bytes_received > 0) {
                    for (int i = 0; i < bytes_received; i++) {
                        message_buffer[i] = toupper(message_buffer[i]);
                    }

                    printf("Client %d: %s\n", i + 1, message_buffer);
                }
                else {
                    close(sd);
                    client_sockets[i] = 0;
                }
            }
        }
    }

    unlink(SOCKET_PATH);

    return 0;
}