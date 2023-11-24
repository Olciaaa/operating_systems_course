#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>

void signal_handler(int signal){
    printf("Killed by catcher :(\n");
}

int main(int argc, char **argv){
    if(argc < 3){
        perror("Not enough arguments, at least 3 needed");
        return 1;
    }

    pid_t catcher_id;
    sscanf(argv[1], "%d", &catcher_id);

    struct sigaction action;
    sigset_t block_set;

    sigfillset( &block_set );
    sigdelset( &block_set, SIGUSR1);

    action.sa_handler = &signal_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    if (sigaction(SIGUSR1, &action, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    union sigval sigspec;

    for(int i = 2; i < argc; i++){
        sscanf(argv[i], "%d", &sigspec.sival_int);
        sigqueue(catcher_id, SIGUSR1, sigspec);

        sigsuspend( &block_set);
    }

    return 1;
}