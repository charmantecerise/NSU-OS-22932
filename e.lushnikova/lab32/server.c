#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <ctype.h>

#define SOCK_PATH "/tmp/socket"
#define BUF_SIZE 1024
#define MAX_CLIENTS 5

int main() {
    int server_fd, client_fds[MAX_CLIENTS], max_fd, fd_num;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len;
    char buf[BUF_SIZE];
    fd_set read_fds, write_fds, all_fds;

    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCK_PATH, sizeof(server_addr.sun_path) - 1);

    unlink(SOCK_PATH);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_CLIENTS) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    FD_ZERO(&all_fds);
    FD_SET(server_fd, &all_fds);
    max_fd = server_fd;

    while (1) {
        read_fds = all_fds;
        write_fds = all_fds;

        if ((fd_num = select(max_fd + 1, &read_fds, &write_fds, NULL, NULL)) == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(server_fd, &read_fds)) {
            client_len = sizeof(client_addr);

            if ((client_fds[fd_num] = accept(server_fd, (struct sockaddr*)&client_addr, &client_len)) == -1) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            printf("New client connected\\n");

            FD_SET(client_fds[fd_num], &all_fds);
            if (client_fds[fd_num] > max_fd) {
                max_fd = client_fds[fd_num];
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (FD_ISSET(client_fds[i], &read_fds)) {
                int recv_len = recv(client_fds[i], buf, BUF_SIZE, 0);
                if (recv_len <= 0) {
                    printf("Client disconnected\\n");

                    close(client_fds[i]);
                    FD_CLR(client_fds[i], &all_fds);
                }
                else {
                    for (int j = 0; j < recv_len; j++) {
                        buf[j] = toupper(buf[j]);
                    }

                    int send_len = send(client_fds[i], buf, recv_len, 0);
                    if (send_len == -1) {
                        perror("send");
                        exit(EXIT_FAILURE);
                    }
                }
            }
        }
    }

    return 0;
}