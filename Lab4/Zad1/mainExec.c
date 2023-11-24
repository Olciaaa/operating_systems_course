#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main(){
    int sigid = SIGUSR1;

    sigset_t pending;
    sigpending(&pending);

    if(sigismember(&pending, sigid)){
        printf("IN OTHER PROGRAM SIGNAL IS PENDING\n");
    }else{
        printf("IN OTHER PROGRAM SIGNAL IS NOT PENDING\n");
    }

    printf("RAISE SIGNAL IN OTHER PROGRAM\n");
    raise(sigid);

    return 0;
}