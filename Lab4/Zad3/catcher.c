#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

int requests = 0;
int flag = 1;
int cycle = 0;
int func = -1;
bool done = true;

void printNumbers(){
    for(int i = 1; i <= 100; i++){
        printf("%d, ", i);
    }
    printf("\n");
}

void printActualTime(){
    time_t t;
    time( &t );
    printf( "The time is %s\n", ctime(&t) );
}

void printRequests(){
    printf("Number of requests: %d\n", requests);
}

void time_loop(double dt) {
    static double acc = 1.0;

    acc += dt;
    if (acc >= 1.0) {
        printActualTime();
        acc = 0.0;
    }
}

void signal_sigaction(int sig, siginfo_t *info, void *secret){
    cycle = 0;
    printf("Received signal %d, received value: %d, si_pid: %d\n", sig, info->si_value.sival_int, info->si_pid);

    func = info->si_value.sival_int;

    kill(info->si_pid, SIGUSR1);
    done = false;
    requests ++;
}

double maxd(double x, double y) {
    return x > y ? x : y;
}

int main(){
    printf("Catcher pid: %d\n", getpid());

    struct sigaction action;

    action.sa_sigaction = &signal_sigaction;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_SIGINFO;
    if (sigaction(SIGUSR1, &action, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    clock_t begin = clock();
    clock_t end = clock();

    while(flag){
        double dt = maxd(0.0, (double)(end-begin)/CLOCKS_PER_SEC);
        begin = clock();

        if (done)
            continue;

        switch(func){
            case 1:
                printNumbers();
                break;
            case 2:
                printActualTime();
                break;
            case 3:
                printRequests();
                break;
            case 4:
                time_loop(dt);
                break;
            case 5:
                fflush(NULL);
                exit(0);
        }
        if(func != 4){
            done = true;
        }

        end = clock();
    }
}