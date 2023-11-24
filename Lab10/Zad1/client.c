#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/un.h>
#include "defines.h"
#include <signal.h>

#define PORT 7777
char* name;
pid_t client_id;

void interrupt_handler(int sigid){
    char buffer[MSGMAX];

    sprintf(buffer, "%d", name);

    printf("Client sends STOP message to server!\n");



    printf("Client is disconnecting!\n");

    exit(EXIT_SUCCESS);
}

int main(int args, char **argv) {
     int fd = -1;

     if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
         printf("Error creating socket\n");
     }

     name = argv[1];
     char* type = argv[2];
     char* path = argv[3];
     int port = atoi(argv[4]);
     client_id = getpid();
     char buffer[MSGMAX];

     struct sockaddr_in addr;
     addr.sin_family = AF_INET;
     addr.sin_port = htons(port);
     addr.sin_addr.s_addr = inet_addr(path);
     addr.sin_zero[0] = '\0';

     if (connect(fd, (struct sockaddr*)&addr, sizeof(struct sockaddr)) == -1) {
         printf("Error connecting\n");
     }

     char buf[64];
     sprintf(buf, "%d", getpid());
     if (write(fd, buf, 64) == -1) {
         printf("Error sending message\n");
     }

    while(1){
        char* command = calloc(MSGMAX, sizeof(char));

        fgets(command, MSGMAX, stdin);

        if(strncmp(command, "LIST", 4) == 0){
            sprintf(buffer, "%d", client_id);

            printf("Client sends LIST message to server!\n");

            write(fd, buffer, MSGMAX);
            
        }else if(strncmp(command, "2ALL", 4) == 0){
            char tmp[MSGMAX];
            sscanf(command, "%*s %s", tmp);

            sprintf(buffer, "%d %s", client_id, tmp);

            printf("Client sends 2ALL message to server!\n");

            write(fd, buffer, MSGMAX);
        }else if(strncmp(command, "2ONE", 4) == 0){
            char tmp[MSGMAX];
            int receiver_id;
            sscanf(command, "%*s %d %s", &receiver_id, tmp);
            sprintf(buffer, "%d %d %s", client_id, receiver_id, tmp);

            printf("Client sends 2ONE message to server!\n");

            write(fd, buffer, MSGMAX);
        }else if(strncmp(command, "STOP", 4) == 0){
            interrupt_handler(SIGINT);
        }else{
            fprintf(stderr, "INVALID COMMAND! CAN BE LIST, 2ALL, 2ONE, STOP!\n");
        }

            free(command);
    }

    close(fd);

     return 0;
}
