#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/select.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include "defines.h"

int client_id;
int client_msg;
int server_msg;
int is_running = 1;

int can_send(){
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
    return (FD_ISSET(0,  &fds));
}

void interrupt_handler(int sigid){
    struct{
        long type;
        char buffer[MSGMAX];
    }message_data;

    message_data.type = STOP;
    sprintf(message_data.buffer, "%d", client_id);

    printf("Client sends STOP message to server!\n");

    msgsnd(server_msg, &message_data, strlen(message_data.buffer), IPC_NOWAIT);

    printf("Client is disconnecting!\n");

    msgctl(client_msg, IPC_RMID, 0);

    is_running = 0;
}

int main(){
    char* home_path = getenv("HOME");

    if(home_path == NULL){
        fprintf(stderr, "CANNOT GET PATH $HOME!");
        return -1;
    }

    srand(time(NULL));

    printf("Client is starting!\n");

    server_msg = msgget(SERVER_IPC_KEY, 0);
    
    if(server_msg == -1){
        perror("CANNOT OPEN SERVER'S QUEUE!");
        return -1;
    }

    key_t client_key = ftok(home_path, rand() % 256);
    client_msg = msgget(client_key, IPC_CREAT | IPC_EXCL | 0666);

    if(client_msg == -1){
        perror("CANNOT CREATE CLIENT'S QUEUE!");
        return -1;
    }

    printf("Client is running with queue id %d\n", client_msg);

    struct{
        long type;
        char buffer[MSGMAX];
    }message_data;

    message_data.type = INIT;
    sprintf(message_data.buffer, "%d", client_msg);

    printf("Client sends INIT message to server!\n");

    msgsnd(server_msg, &message_data, 128, IPC_NOWAIT);

    printf("Client is waiting for server to response!\n");

    msgrcv(client_msg, &message_data, MSGMAX, RESPONSE, 0);

    sscanf(message_data.buffer, "%d", &client_id);

    printf("Client got response and now has id %d\n", client_id);

    signal(SIGINT, interrupt_handler);

    fflush(stdout);

    while(is_running){
        if(can_send() > 0){
            char* command = calloc(MSGMAX, sizeof(char));

            fgets(command, MSGMAX, stdin);

            if(strncmp(command, "LIST", 4) == 0){
                message_data.type = LIST;
                sprintf(message_data.buffer, "%d", client_id);

                printf("Client sends LIST message to server!\n");

                msgsnd(server_msg, &message_data, strlen(message_data.buffer), IPC_NOWAIT);
            }else if(strncmp(command, "2ALL", 4) == 0){
                message_data.type = TOALL;
                char tmp[MSGMAX];
                sscanf(command, "%*s %s", tmp);

                sprintf(message_data.buffer, "%d %s", client_id, tmp);

                printf("Client sends 2ALL message to server!\n");

                msgsnd(server_msg, &message_data, strlen(message_data.buffer), IPC_NOWAIT);
            }else if(strncmp(command, "2ONE", 4) == 0){
                message_data.type = TOONE;
                char tmp[MSGMAX];
                int receiver_id;
                sscanf(command, "%*s %d %s", &receiver_id, tmp);
                sprintf(message_data.buffer, "%d %d %s", client_id, receiver_id, tmp);

                printf("Client sends 2ONE message to server!\n");

                msgsnd(server_msg, &message_data, strlen(message_data.buffer), IPC_NOWAIT);
            }else if(strncmp(command, "STOP", 4) == 0){
                interrupt_handler(SIGINT);
            }else{
                fprintf(stderr, "INVALID COMMAND! CAN BE LIST, 2ALL, 2ONE, STOP!\n");
            }

            free(command);
        }

        if(msgrcv(client_msg, &message_data, MSGMAX, RESPONSE, IPC_NOWAIT) > 0){
            printf("Client with id %d received message: %s\n", client_id, message_data.buffer);
        }else if(msgrcv(client_msg, &message_data, MSGMAX, SERVERDOWN, IPC_NOWAIT) > 0){
            printf("Server is shuting down!\n");
            printf("Client with id %d is disconnecting!\n", client_id);
            msgctl(client_msg, IPC_RMID, 0);
            is_running = 0;
        }
    }

    return 0;
}