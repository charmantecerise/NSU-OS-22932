#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>

#define SOCKET_PATH "/tmp/my_unix_socket"
#define MESSAGE_SIZE 100

int main() {
    int server_socket, client_socket;
    socklen_t client_len;
    struct sockaddr_un server_address, client_address;
    char message_buffer[MESSAGE_SIZE];

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

    if (listen(server_socket, 5) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server is waiting for connections...\n");

    client_len = sizeof(struct sockaddr_un);
    client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_len);
    if (client_socket == -1) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    close(server_socket); 

    int bytes_received = read(client_socket, message_buffer, MESSAGE_SIZE);  
    if (bytes_received > 0) {
        for (int i = 0; i < bytes_received; i++) {
            message_buffer[i] = toupper(message_buffer[i]); 
        }

        printf("Received and converted: %s\n", message_buffer);
    }

    close(client_socket);  

    unlink(SOCKET_PATH);  

    return 0;
}
