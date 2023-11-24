#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <errno.h>
#include <sys/select.h>
#include <string.h>
#include "defines.h"

int client_id;
mqd_t client_msg;
mqd_t server_msg;
int is_running = 1;
char client_msg_name[100];

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
    char buffer[MSGMAX];

    sprintf(buffer, "%d", client_id);

    printf("Client sends STOP message to server!\n");

    mq_send(server_msg, buffer, MSGMAX, STOP);

    printf("Client is disconnecting!\n");

    mq_close(server_msg);
    mq_close(client_msg);
    mq_unlink(client_msg_name);

    exit(EXIT_SUCCESS);
}

int main(){
    struct mq_attr mqAttr;
    mqAttr.mq_flags = O_RDONLY | O_NONBLOCK | O_CREAT | O_EXCL;
    mqAttr.mq_maxmsg = 10;
    mqAttr.mq_msgsize = MSGMAX;
    mqAttr.mq_curmsgs = 0;

    pid_t client_pid = getpid();
    sprintf(client_msg_name, "/clientQueue_%d", client_pid);

    client_msg = mq_open(client_msg_name, O_RDONLY | O_NONBLOCK | O_CREAT | O_EXCL, 0666, &mqAttr);

    if(client_msg == -1){
        mq_unlink(client_msg_name);
        client_msg = mq_open(client_msg_name, O_RDONLY | O_NONBLOCK | O_CREAT | O_EXCL, 0666, &mqAttr);

        if(client_msg == -1){
            perror("CANNOT CREATE QUEUE!");
            return -1;
        }
    }

    printf("Client is starting!\n");

    server_msg = mq_open(server_msg_name, O_WRONLY);
    
    if(server_msg == -1){
        perror("CANNOT OPEN SERVER'S QUEUE!");
        return -1;
    }

    if(client_msg == -1){
        perror("CANNOT CREATE CLIENT'S QUEUE!");
        return -1;
    }

    printf("Client is running with queue id %d\n", client_msg);

    char buffer[MSGMAX];
    sprintf(buffer, "%s", client_msg_name);

    printf("\n**\n%s\n**\n", buffer);

    printf("Client sends INIT message to server!\n");

    mq_send(server_msg, buffer, MSGMAX, INIT);

    printf("Client is waiting for server to response!\n");

    size_t size;

    do{
        errno = 0;
        size = mq_receive(client_msg, buffer, MSGMAX, NULL);
    }while(errno == EAGAIN || size == 0);

    sscanf(buffer, "%d", &client_id);

    printf("Client got response and now has id %d\n", client_id);

    signal(SIGINT, interrupt_handler);

    fflush(stdout);

    while(is_running){
        if(can_send() > 0){
            char* command = calloc(MSGMAX, sizeof(char));

            fgets(command, MSGMAX, stdin);

            if(strncmp(command, "LIST", 4) == 0){
                sprintf(buffer, "%d", client_id);

                printf("Client sends LIST message to server!\n");

                mq_send(server_msg, buffer, MSGMAX, LIST);
            }else if(strncmp(command, "2ALL", 4) == 0){
                char tmp[MSGMAX];
                sscanf(command, "%*s %s", tmp);

                sprintf(buffer, "%d %s", client_id, tmp);

                printf("Client sends 2ALL message to server!\n");

                mq_send(server_msg, buffer, MSGMAX, TOALL);
            }else if(strncmp(command, "2ONE", 4) == 0){
                char tmp[MSGMAX];
                int receiver_id;
                sscanf(command, "%*s %d %s", &receiver_id, tmp);
                sprintf(buffer, "%d %d %s", client_id, receiver_id, tmp);

                printf("Client sends 2ONE message to server!\n");

                mq_send(server_msg, buffer, MSGMAX, TOONE);
            }else if(strncmp(command, "STOP", 4) == 0){
                interrupt_handler(SIGINT);
            }else{
                fprintf(stderr, "INVALID COMMAND! CAN BE LIST, 2ALL, 2ONE, STOP!\n");
            }

            free(command);
        }
        unsigned prio;
        if(mq_receive(client_msg, buffer, MSGMAX, &prio) > 0){
            if(prio == RESPONSE){
                printf("Client with id %d received message: %s\n", client_id, buffer);
            }else if(prio == SERVERDOWN){
                printf("Server is shuting down!\n");
                printf("Client with id %d is disconnecting!\n", client_id);
                mq_close(server_msg);
                mq_close(client_msg);
                mq_unlink(client_msg_name);
                is_running = 0;
            }
        }
    }

    return 0;
}