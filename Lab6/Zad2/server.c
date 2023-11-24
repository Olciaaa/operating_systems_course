#include <errno.h>
#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include "defines.h"

mqd_t server_msg;
int num_of_clients = 0;
int list_of_clients[num_max_clients];
time_t t;
FILE* log_file;

void interrupt_handler(int sigid){
    char buffer[MSGMAX];

    printf("Server is shuting down!\n");
    printf("Server sends messages to clients to disconnect!\n");

    for(int i = 0; i < num_of_clients; i++){
        if(list_of_clients[i] != -1){
            sprintf(buffer, "%d", i);
            mq_send(list_of_clients[i], buffer, MSGMAX, SERVERDOWN);
            mq_close(list_of_clients[i]);
        }
    }

    mq_close(server_msg);
    mq_unlink(server_msg_name);

    char log[100];
    time(&t);
    sprintf(log, "%s SERVERDOWN %s\n", ctime(&t), buffer);

    fwrite(log, sizeof(char), strlen(log), log_file);

    fclose(log_file);

    exit(EXIT_SUCCESS);
}

void handleSTOP(char* message_buffer){
    int sender_id;
    sscanf(message_buffer, "%d", &sender_id);

    if(sender_id >= num_of_clients){
        fprintf(stderr, "CLIENT WITH GIVEN ID DOESN'T EXIST!\n");
        return;
    }

    if(list_of_clients[sender_id] == -1){
        fprintf(stderr, "CLIENT WITH GIVEN ID IS ALREADY INACTIVE!\n");
        return;
    }

    mq_close(list_of_clients[sender_id]);
    list_of_clients[sender_id] = -1;

    printf("Server disactivated client %d\n", sender_id);
}

void handle2ONE(char* message_buffer){
    int sender_id;
    int receiver_id;
    char message[MSGMAX];
    sscanf(message_buffer, "%d %d %s", &sender_id, &receiver_id, message);

    if(receiver_id >= num_of_clients){
        fprintf(stderr, "CLIENT WITH GIVEN ID DOESN'T EXIST!\n");
        return;
    }

    if(list_of_clients[receiver_id] == -1){
        fprintf(stderr, "CLIENT WITH GIVEN ID IS ALREADY INACTIVE!\n");
        return;
    }

    time(&t);

    char buffer[MSGMAX];

    sprintf(buffer, "%d %s%s", sender_id, ctime(&t), message);
    
    mq_send(list_of_clients[receiver_id], buffer, MSGMAX, RESPONSE);

    printf("Server sent message to client %d from client %d\n", receiver_id, sender_id);
}

void handle2ALL(char* message_buffer){
    int sender_id;
    char message[MSGMAX];
    sscanf(message_buffer, "%d %s", &sender_id, message);

    time(&t);

    char buffer[MSGMAX];

    sprintf(buffer, "%d %s%s", sender_id, ctime(&t), message);

    for(int i = 0; i < num_max_clients; i++){
        if(list_of_clients[i] != -1 && i != sender_id){
            mq_send(list_of_clients[i], buffer, MSGMAX, RESPONSE);
        }
    }

    printf("Server sent message to all clients exept %d\n", sender_id);
}

void handleLIST(char* message_buffer){
    int sender_id;
    sscanf(message_buffer, "%d", &sender_id);

    if(sender_id >= num_of_clients){
        fprintf(stderr, "CLIENT WITH GIVEN ID DOESN'T EXIST!\n");
        return;
    }

    if(list_of_clients[sender_id] == -1){
        fprintf(stderr, "CLIENT WITH GIVEN ID IS ALREADY INACTIVE!\n");
        return;
    }    

    char buffer[MSGMAX];
    strcpy(buffer, "List of ids of active users: ");

    char tmp[4];

    for(int i = 0; i < num_of_clients; i++){
        if(list_of_clients[i] != -1){
            sprintf(tmp, "%d ", i);
            strcat(buffer, tmp);
        }
    }

    mq_send(list_of_clients[sender_id], buffer, MSGMAX, RESPONSE);

    printf("List of ids of active users: ");

    printf("%s\n", buffer);
}

int handleINIT(char* message_buffer){
    
    mqd_t queue_id = mq_open(message_buffer, O_WRONLY | O_NONBLOCK);

    list_of_clients[num_of_clients] = queue_id;

    char buffer[3];
    sprintf(buffer, "%d\n", num_of_clients);

    mq_send(queue_id, buffer, MSGMAX, RESPONSE);

    printf("Server responded to INIT, new client with id %d will communicate thru queue with id %d\n", num_of_clients, queue_id);
    
    num_of_clients++;

    return num_of_clients;
}

int main(){

    printf("Server is starting!\n");

    struct mq_attr mqAttr;
    mqAttr.mq_flags = O_RDONLY | O_NONBLOCK | O_CREAT | O_EXCL;
    mqAttr.mq_maxmsg = 10;
    mqAttr.mq_msgsize = MSGMAX;
    mqAttr.mq_curmsgs = 0;

    server_msg = mq_open(server_msg_name, O_RDONLY | O_NONBLOCK | O_CREAT | O_EXCL, 0666, &mqAttr);

    if(server_msg == -1){
        mq_unlink(server_msg_name);
        server_msg = mq_open(server_msg_name, O_RDONLY | O_NONBLOCK | O_CREAT | O_EXCL, 0666, &mqAttr);

        if(server_msg == -1){
            perror("CANNOT CREATE QUEUE!");
            return -1;
        }
    }

    printf("Server is running!\n");

    signal(SIGINT, interrupt_handler);

    messages messageType;
    char buffer[MSGMAX];
    buffer[MSGMAX] = 0;

    log_file = fopen("server_log.txt", "w");
    char log[100];

    while(1){
        if(mq_receive(server_msg, buffer, MSGMAX, &messageType) != -1){

            switch(messageType){
                case INIT:
                    printf("Server is handling INIT!\n");
                    if( num_of_clients < num_max_clients){
                        num_of_clients = handleINIT(buffer);

                        time(&t);
                        sprintf(log, "%s INIT %s\n", ctime(&t), buffer);

                        fwrite(log, sizeof(char), strlen(log), log_file);
                    }
                    else{
                        fprintf(stderr, "CANNOT ADD NEW CLIENT, THE MAXIMUM NUMBER OF CLIENT IS %d!\n", num_max_clients);
                    }
                    break;
                case LIST:
                    printf("%s\n", buffer);
                    printf("Server is handling LIST!\n");
                    handleLIST(buffer);

                    time(&t);
                    sprintf(log, "%s LIST %s\n", ctime(&t), buffer);

                    fwrite(log, sizeof(char), strlen(log), log_file);
                    break;
                case TOALL:
                    printf("Server is handling 2ALL!\n");
                    handle2ALL(buffer);

                    time(&t);
                    sprintf(log, "%s 2ALL %s\n", ctime(&t), buffer);

                    fwrite(log, sizeof(char), strlen(log), log_file);
                    break;
                case TOONE:
                    printf("Server is handling 2ONE!\n");
                    handle2ONE(buffer);

                    time(&t);
                    sprintf(log, "%s 2ONE %s\n", ctime(&t), buffer);

                    fwrite(log, sizeof(char), strlen(log), log_file);
                    break;
                case STOP:
                    printf("Server is handling STOP!\n");
                    printf("\n\n%s\n\n", buffer);
                    
                    handleSTOP(buffer);

                    time(&t);
                    sprintf(log, "%s STOP %s\n", ctime(&t), buffer);

                    fwrite(log, sizeof(char), strlen(log), log_file);

                    break;
                default:
                    break;
            }
        }
    }

    return 0;
}