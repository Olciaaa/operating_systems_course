#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <signal.h>
#include "defines.h"

#define PORT 7777

char* name;
pid_t client_id;
int client_fd;

void interrupt_handler(int signum) {
    printf("Client sends STOP message to server!\n");
    char buffer[MSGMAX];
    sprintf(buffer, "%d", client_id);
    sendto(client_fd, buffer, strlen(buffer), 0, NULL, 0);
    printf("Client is disconnecting!\n");
    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
    if (argc != 5) {
        printf("Usage: ./client <name> <type> <path> <port>\n");
        return EXIT_FAILURE;
    }

    client_id = getpid();
    name = argv[1];
    char* type = argv[2];
    char* path = argv[3];
    int port = atoi(argv[4]);

    client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_fd == -1) {
        perror("Error creating socket");
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(path);
    memset(server_addr.sin_zero, '\0', sizeof(server_addr.sin_zero));

    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error connecting");
        return EXIT_FAILURE;
    }

    char buf[64];
    sprintf(buf, "%d", getpid());
    if (sendto(client_fd, buf, strlen(buf), 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error sending message");
        return EXIT_FAILURE;
    }

    signal(SIGINT, interrupt_handler);

    while (1) {
        char command[MSGMAX];
        fgets(command, MSGMAX, stdin);
        command[strcspn(command, "\n")] = '\0';

        if (strncmp(command, "LIST", 4) == 0) {
            printf("Client sends LIST message to server!\n");
            sendto(client_fd, command, strlen(command), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
        } else if (strncmp(command, "2ALL", 4) == 0) {
            printf("Client sends 2ALL message to server!\n");
            sendto(client_fd, command, strlen(command), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
        } else if (strncmp(command, "2ONE", 4) == 0) {
            printf("Client sends 2ONE message to server!\n");
            sendto(client_fd, command, strlen(command), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
        } else if (strncmp(command, "STOP", 4) == 0) {
            interrupt_handler(SIGINT);
        } else {
            fprintf(stderr, "INVALID COMMAND! CAN BE LIST, 2ALL, 2ONE, STOP!\n");
        }
    }

    close(client_fd);

    return 0;
}
