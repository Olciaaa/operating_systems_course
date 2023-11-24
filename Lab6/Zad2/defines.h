#ifndef DEFINES_H
#define DEFINES_H
#define MSGMAX 4096
#define num_max_clients 10
#include <stdlib.h>
#include <string.h>

const char server_msg_name [] = "/serverqueue";

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