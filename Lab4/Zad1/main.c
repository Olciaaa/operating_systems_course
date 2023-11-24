#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void check_pending(int signum) {
    sigset_t sigset;

    if (sigpending(&sigset) != 0)
        perror("sigpending() error");
    else if (sigismember(&sigset, signum))
        printf("a %d signal is pending, parent: %d, child: %d\n", signum, getppid(), getpid());
    else
        printf("no %d signals are pending, parent: %d, child: %d\n", signum, getppid(), getpid());
}

void handle_signal(int sig) {
    printf("Received signal %d, parent: %d, child: %d\n", sig, getppid(), getpid());
}

int main(int argc, char**argv) {
    if(argc != 2){
        perror("No enough arguments, 2 needed");
        return -1;
    }

    struct sigaction action;
    if (strcmp(argv[1], "ignore") == 0) {
        action.sa_handler = SIG_IGN;
    } else {
        action.sa_handler = handle_signal;
    }

    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    if (sigaction(SIGUSR1, &action, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    if (strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0) {
        if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
            perror("sigprocmask");
            return 1;
        }
    }

    raise(SIGUSR1);

    if (strcmp(argv[1], "pending") == 0) {
        check_pending(SIGUSR1);
    }

    fflush(stdout);
    pid_t child_pid = fork();
    if(child_pid == 0){
        if (strcmp(argv[1], "pending") == 0) {
            check_pending(SIGUSR1);
        }
        raise(SIGUSR1);
        exit(0);
    }else if(child_pid == -1){
        perror("Cannot create child process");
        exit(EXIT_FAILURE);
    }else{
        wait(NULL);
        if(strcmp((argv[1]), "handler") != 0){
            execl("./mainExec.exe", "mainExec.exe", NULL);
            perror("a");
        }
    }
    return 0;
}