#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define UNIX_PATH_MAX 108

int main() {
    int fd = -1;
    if((fd = socket(AF_UNIX, SOCK_DGRAM, 0 )) == -1) {
        perror("Error craeting socket");
    }

    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path,"\0");

    if(connect(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) == -1)
        perror("Error connecting to server");

    char buff[20];
    int to_send = sprintf(buff, "HELLO From: %zu", getpid());

    if(write(fd, buff, to_send+1) == -1) {
        perror("Error sending msg to server");
    }

    shutdown(fd, SHUT_RDWR);


    return 0;
}
