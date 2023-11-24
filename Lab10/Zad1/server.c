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
#include "defines.h"
#include <sys/select.h>

#define MAX_CLIENTS 10

int main(int args, char **argv) {
    if (args != 3) {
        perror("Provide 2 arguments");
        exit(EXIT_FAILURE);
    }

    int *clients_fd = (int *)calloc(MAX_CLIENTS, sizeof(int));
    int num_of_clients = 0;

    int fd = -1;
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("Error creating socket\n");
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    const char *unix_socket_path = argv[2];

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(unix_socket_path);
    addr.sin_zero[0] = '\0';
    if (bind(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) == -1) {
        printf("Error binding\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        if (listen(fd, MAX_CLIENTS) == -1) {
            printf("Error listening\n");
            exit(EXIT_FAILURE);
        }

        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(fd, &read_fds);

        int max_fd = fd;
        for (int i = 0; i < num_of_clients; i++) {
            FD_SET(clients_fd[i], &read_fds);
            if (clients_fd[i] > max_fd) {
                max_fd = clients_fd[i];
            }
        }

        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
            printf("Error in select\n");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(fd, &read_fds)) {
            int client_fd;
            struct sockaddr_un client_addr;
            socklen_t client_addr_len = sizeof(client_addr);

            if ((client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_addr_len)) == -1) {
                perror("Błąd podczas akceptowania połączenia");
                exit(EXIT_FAILURE);
            }

            if (num_of_clients < MAX_CLIENTS) {
                clients_fd[num_of_clients] = client_fd;
                num_of_clients++;
            } else {
                printf("Max number of clients reached. Rejecting connection.\n");
                close(client_fd);
            }
        }

        for (int i = 0; i < num_of_clients; i++) {
            if (FD_ISSET(clients_fd[i], &read_fds)) {
                char buf[64];
                ssize_t bytes_received = read(clients_fd[i], buf, sizeof(buf));
                if (bytes_received == -1) {
                    perror("Błąd podczas odbierania wiadomości");
                    exit(EXIT_FAILURE);
                } else if (bytes_received == 0) {
                    printf("Klient rozłączony\n");
                    close(clients_fd[i]);
                    if (i != num_of_clients - 1) {
                        clients_fd[i] = clients_fd[num_of_clients - 1];
                    }
                    num_of_clients--;
                } else {
                    printf("Wiadomość od klienta: \"%s\"\n", buf);
                }
            }
        }
    }

    close(fd);
    free(clients_fd);

    return 0;
}
