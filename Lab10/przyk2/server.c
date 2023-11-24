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
#include <netdb.h>

#define UNIX_PATH_MAX 108
#define PORT 7777

int main() {
    int fd = -1;
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("Error creating socket\n");
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    addr.sin_zero[0] = '\0';
    if (bind(fd, (struct sockaddr*)&addr, sizeof(struct sockaddr)) == -1) {
        printf("Error binding\n");
    }

    while (1)
    {
        if(listen(fd, 10) == -1){
        printf("Error listening\n");
        }

        char buf[64];
        int client_fd;
        struct sockaddr_un client_addr;
        socklen_t client_addr_len = sizeof(client_addr);

        if ((client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_addr_len)) == -1) {
            perror("Błąd podczas akceptowania połączenia");
            exit(EXIT_FAILURE);
        }

        if (read(client_fd, buf, 20) == -1) {
            perror("Błąd podczas odbierania wiadomości");
            exit(EXIT_FAILURE);
        }

        printf("Wiadomość od klienta: \"%s\"\n", buf);

        shutdown(client_fd, SHUT_RDWR);
        close(client_fd);
    }
    close(fd);
}