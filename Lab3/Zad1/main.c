#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>

int isNumber(char s[]) {
    for (int i = 0; s[i] != '\0'; i++) {
        if (isdigit(s[i]) == 0)
            return 0;
    }
    return 1;
}

int main(int argc, char** argcv){
    if(argc != 2){
        printf("Powinien być podany jeden argument!!!");
        return -1;
    }

    if(isNumber(argcv[1]) == 0){
        printf("Argument powinien być liczbą (ilością procesów które mają się wykonać)");
    }

    int numberOfProcesses = atoi(argcv[1]);
    pid_t child_pid;

    for(int i = 0; i < numberOfProcesses; i++){
        child_pid = fork();
        if(child_pid == 0){
            printf("Proces rodzica ma pid:%d\n", (int)getppid());
            printf("Proces dziecka ma pid:%d\n", (int) getpid());
            exit(0);
        }
    }

    while(wait(NULL) > 0);
    printf("Argument ma wartość: %d\n", numberOfProcesses);

    return 0;
}
