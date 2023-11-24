//Przetestuj działanie trzech wybranych flag w funkcji sigation. Jedną z nich powinna być flaga SA_SIGINFO. Dla tej flagi zainstaluj procedurę obsługi sygnału (handler)
//dla odpowiednio dobranych sygnałów stosując składnie procedury handlera z trzema argumentami. Wypisz i skomentuj (przygotowując odpowiednie scenariusze) trzy różne
//informacje, a dodatkowo także numer sygnału oraz identyfikator PID procesu wysyłającego dostarczane w strukturze siginfo_t przekazywanej jako drugi argument funkcji handlera.

#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/times.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

int flag = 5;

void signal_sigaction(int sig, siginfo_t *info, void *secret){
    printf("Received signal %d, PID: %d, exit value of signal: %d, user time consumed: %ld, system time consumed: %ld\n",
           sig, info->si_pid, info->si_status, info->si_utime, info->si_stime);
}

void signal_handler1(int sig){
    printf("Signal %d received\n", sig);
    flag -= 1;
    if (flag >= 0) {
        printf("Raising signal %d again\n", sig);
        raise(sig);
    }
    else {
        printf("Signal %d handled, exiting\n", sig);
    }
}

void signal_handler2(int sig){
    printf("Signal %d received\n", sig);
}

int main(){
    printf("For SA_SIGINFO:\n");
    struct sigaction action;

    action.sa_sigaction = &signal_sigaction;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_SIGINFO;
    if (sigaction(SIGUSR1, &action, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    raise(SIGUSR1);

    printf("For SA_NODEFER:\n");

    struct sigaction action1;
    action1.sa_handler = signal_handler1;
    action1.sa_flags = SA_NODEFER;

    if (sigaction(SIGUSR1, &action1, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    raise(SIGUSR1);

    printf("For SA_RESETHAND:\n");

    struct sigaction action2;
    action2.sa_handler = signal_handler2;
    action2.sa_flags = SA_RESETHAND;

    if (sigaction(SIGUSR1, &action2, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("Sending SIGUSR1...\n");
    raise(SIGUSR1);

    printf("Sending SIGUSR1 again...\n");
    raise(SIGUSR1);

    printf("Exiting...\n");

    return 0;
}

//int main(){
//    printf("For SA_SIGINFO:\n");
//    struct sigaction action;
//
//    action.sa_handler = &signal_handler;
//    sigemptyset(&action.sa_mask);
//    action.sa_flags = SA_RESTART;
//    if (sigaction(SIGUSR1, &action, NULL) == -1) {
//        perror("sigaction");
//        return 1;
//    }
//
//    int pid = fork();
//    if(pid == 0){
//        printf("Wywołujemy funkcję systemową 'sleep'...\n");
//        int res = sleep(5);
//        printf("%d\n", res);
//        fflush(stdout);
//        if (res != 0) {
//            printf("Funkcja 'sleep' została przerwana przez sygnał\n");
//        } else {
//            printf("Funkcja 'sleep' zakończyła działanie z powodzeniem\n");
//        }

//        printf("Wywołujemy funkcję systemową 'read'...\n");
//        char buf[1024];
//        int sourceFile = open("main.c", O_RDONLY);
//        ssize_t res = read(sourceFile, buf, sizeof(buf));
//
//        if (res == -1) {
//            printf("Funkcja 'read' została przerwana przez sygnał\n");
//        } else {
//            printf("Odczytano %ld bajtów z wejścia standardowego\n", res);
//        }
//        pause();
//
//
//        fflush(stdout);
//        system("cat main.c");
//        printf("siema");
//        exit(0);
//    }else{
//        sleep(1);
//        kill(pid, SIGUSR1);
//        kill(pid, SIGUSR1);
//    }
//
//    printf("For SA_REPEAT:\n");
//
//    struct sigaction sa;
//    sa.sa_handler = signal_handler;
//    sa.sa_flags = SA_RESTART;
//    sigemptyset(&sa.sa_mask);
//    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
//        perror("sigaction");
//        return 1;
//    }
//
//    int pid = fork();
//    if(pid == 0){
//        printf("Wywołujemy funkcję systemową 'read'...\n");
//        char buf[10];
//        int sourceFile = open("main.c", O_RDONLY);
//        ssize_t res = read(sourceFile, buf, sizeof(buf));
//
//        if (res == -1) {
//            printf("Funkcja 'read' została przerwana przez sygnał\n");
//        } else {
//            printf("Odczytano %ld bajtów z wejścia standardowego\n", res);
//        }
//        exit(0);
//    }else{
//        kill(pid, SIGUSR1);
//    }
//
//    return 0;
//}