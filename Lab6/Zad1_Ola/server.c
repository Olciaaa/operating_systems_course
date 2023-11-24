#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>
#include <sys/ipc.h>
#include <signal.h>
#include <time.h>
#include "defines.h"

int server_msg;
int num_of_clients = 0;
int list_of_clients[10];
time_t t;
FILE* log_file;

void interrupt_handler(int sigid){
    struct{
        long type;
        char buffer[MSGMAX];
    }message_data;

    message_data.type = SERVERDOWN;

    printf("Server is shuting down!\n");
    printf("Server sends messages to clients to disconnect!\n");

    msgctl(server_msg, IPC_RMID, 0);

    for(int i = 0; i < num_of_clients; i++){
        if(list_of_clients[i] != -1){
            sprintf(message_data.buffer, "%d", i);
            msgsnd(list_of_clients[i], &message_data, strlen(message_data.buffer)+1, IPC_NOWAIT);
        }
    }

    char log[100];
    time(&t);
    sprintf(log, "%s SERVERDOWN %s\n", ctime(&t), message_data.buffer);

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

    struct{
        long mtype;
        char buffer[MSGMAX];
    }message_data;

    sprintf(message_data.buffer, "%d %s%s", sender_id, ctime(&t), message);
    message_data.mtype = RESPONSE;

    msgsnd(list_of_clients[receiver_id], &message_data, strlen(message_data.buffer)+1, IPC_NOWAIT);

    printf("Server sent message to client %d from client %d\n", receiver_id, sender_id);
}

void handle2ALL(char* message_buffer){
    int sender_id;
    char message[MSGMAX];
    sscanf(message_buffer, "%d %s", &sender_id, message);

    time(&t);

    struct{
        long mtype;
        char buffer[MSGMAX];
    }message_data;

    sprintf(message_data.buffer, "%d %s%s", sender_id, ctime(&t), message);
    message_data.mtype = RESPONSE;

    for(int i = 0; i < num_max_clients; i++){
        if(list_of_clients[i] != -1 && i != sender_id){
            msgsnd(list_of_clients[i], &message_data, strlen(message_data.buffer)+1, IPC_NOWAIT);
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

    struct{
        long mtype;
        char buffer[MSGMAX];
    }message_data;

    message_data.mtype = RESPONSE;
    strcpy(message_data.buffer, "List of ids of active users: ");

    char tmp[4];

    for(int i = 0; i < num_of_clients; i++){
        if(list_of_clients[i] != -1){
            sprintf(tmp, "%d ", i);
            strcat(message_data.buffer, tmp);
        }
    }

    msgsnd(list_of_clients[sender_id], &message_data, strlen(message_data.buffer)+1, IPC_NOWAIT);

    printf("List of ids of active users: ");

    printf("%s\n", message_data.buffer);
}

int handleINIT(char* message_buffer){
    int queue_id;

    sscanf(message_buffer, "%d", &queue_id);

    list_of_clients[num_of_clients] = queue_id;

    struct{
        long mtype;
        char buffer[3];
    }message_data;

    message_data.mtype = RESPONSE;
    sprintf(message_data.buffer, "%d", num_of_clients);

    msgsnd(queue_id, &message_data, strlen(message_data.buffer)+1, IPC_NOWAIT);

    printf("Server responded to INIT, new client with id %d will communicate thru queue with id %d\n", num_of_clients, queue_id);
    
    num_of_clients++;

    return num_of_clients;
}

int main(){

    printf("Server is starting!\n");

    key_t server_key = SERVER_IPC_KEY;

    msgctl(server_key, IPC_RMID, NULL);

    server_msg = msgget(server_key, IPC_CREAT | IPC_EXCL | 0666);

    if(server_msg == -1){
        if(errno == EEXIST){
            server_msg = msgget(server_key, 0);

            if(msgctl(server_msg, IPC_RMID, 0) == -1){
                perror("CANNOT CREATE IPC!");
                return -1;
            }else{
                server_msg = msgget(server_key, IPC_CREAT | IPC_EXCL | 0666);
            }
        }
    }

    printf("Server is running!\n");

    struct{
        long mtype;
        char buffer[MSGMAX+1];
    } message_data;

    signal(SIGINT, interrupt_handler);

    log_file = fopen("server_log.txt", "w");
    char log[100];

    while(1){
        printf("Server is waiting for messages!\n");

        size_t read_size = msgrcv(server_msg, &message_data, sizeof(message_data) - sizeof(long), -100, MSG_NOERROR);

        printf("Server received message!\n");

        message_data.buffer[read_size] = 0;

        switch(message_data.mtype){
            case INIT:
                printf("Server is handling INIT!\n");
                if( num_of_clients < num_max_clients){
                    num_of_clients = handleINIT(message_data.buffer);

                    time(&t);
                    sprintf(log, "%s INIT %s\n", ctime(&t), message_data.buffer);

                    fwrite(log, sizeof(char), strlen(log), log_file);
                }
                else{
                    fprintf(stderr, "CANNOT ADD NEW CLIENT, THE MAXIMUM NUMBER OF CLIENT IS %d!\n", num_max_clients);
                }
                break;
            case LIST:
                printf("Server is handling LIST!\n");
                handleLIST(message_data.buffer);

                time(&t);
                sprintf(log, "%s LIST %s\n", ctime(&t), message_data.buffer);

                fwrite(log, sizeof(char), strlen(log), log_file);
                break;
            case TOALL:
                printf("Server is handling 2ALL!\n");
                handle2ALL(message_data.buffer);

                time(&t);
                sprintf(log, "%s 2ALL %s\n", ctime(&t), message_data.buffer);

                fwrite(log, sizeof(char), strlen(log), log_file);
                break;
            case TOONE:
                printf("Server is handling 2ONE!\n");
                handle2ONE(message_data.buffer);

                time(&t);
                sprintf(log, "%s 2ONE %s\n", ctime(&t), message_data.buffer);

                fwrite(log, sizeof(char), strlen(log), log_file);
                break;
            case STOP:
                printf("Server is handling STOP!\n");
                handleSTOP(message_data.buffer);

                time(&t);
                sprintf(log, "%s STOP %s\n", ctime(&t), message_data.buffer);

                fwrite(log, sizeof(char), strlen(log), log_file);

                break;
            default:
                break;
        }
    }

    return 0;
}