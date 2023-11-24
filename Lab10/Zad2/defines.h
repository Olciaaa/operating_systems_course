#ifndef DEFINES_H
#define DEFINES_H
#define MSGMAX 4096

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