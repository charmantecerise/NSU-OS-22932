#define _CRT_SECURE_NO_WARNINGS
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

int count = 0;  

void handleSIGQUIT() {
    printf("Number of sounds: %d", count);
    exit(0);
}

void handleSIGINT() {
    printf("\a");
    fflush(NULL);
    count++;
}

int main() {
    signal(SIGINT, &handleSIGINT);  
    signal(SIGQUIT, &handleSIGQUIT);  
    while (1);
}