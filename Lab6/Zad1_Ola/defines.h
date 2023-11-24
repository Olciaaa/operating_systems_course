#ifndef DEFINES_H
#define DEFINES_H
#define MSGMAX 4096
#include <stdlib.h>
#include <string.h>

#define SERVER_IPC_KEY 66433

const int num_max_clients = 10;

typedef enum{
    STOP = 1,
    LIST,
    TOALL,
    TOONE,
    INIT,
    RESPONSE,
    SERVERDOWN,
}messages;

#endif