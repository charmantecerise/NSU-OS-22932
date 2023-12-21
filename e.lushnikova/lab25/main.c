#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <wait.h>

int main() {
    int fd[2]; 
    pid_t pid;

    if (pipe(fd) == -1) {  
        perror("pipe");
        exit(1);
    }

    pid = fork();  

    if (pid == -1) { 
        perror("fork");
        exit(1);
    }
    else if (pid == 0) {  
        close(fd[1]);  

        char buffer[100];
        int n;

        while ((n = read(fd[0], buffer, sizeof(buffer))) > 0) { 
            for (int i = 0; i < n; i++) {
                buffer[i] = toupper(buffer[i]); 
            }
            write(STDOUT_FILENO, buffer, n);  
        }

        close(fd[0]);  
        exit(0);
    }
    else {  
        close(fd[0]);  

        char* text = "Hello!\n";  

        write(fd[1], text, strlen(text));  

        close(fd[1]);  
        wait(NULL);  
    }

    return 0;
}