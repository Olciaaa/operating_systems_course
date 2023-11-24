#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/times.h>
#include <time.h>

long double f(long double x){
    return 4.0/(x*x+1.0);
}

int main(int argc, char *argv[]){
    if (argc < 3){
        fprintf(stderr,"Wrong number of arguments");
        return 1;
    }

    double n = atof(argv[2]);
    double inter_len = 1/n;
    double sum = 0;
    double curr_start = 0;

    char * pipePath = "./fifo";
    if(mkfifo(pipePath, 0666)== -1){
        perror("CANNOT CREATE FIFO!");
        return -1;
    }

    struct timespec time_start, time_end;

    clock_gettime(CLOCK_REALTIME, &time_start);
    
    for(int i = 0;i < n;i++){
        if(fork() == 0){
            int len = snprintf(NULL, 0, "%f", curr_start);
            char a[len+1];
            snprintf(a, len+1, "%f", curr_start);
            len = snprintf(NULL, 0, "%f", curr_start + inter_len);
            char b[len+1];
            snprintf(b, len+1, "%f", curr_start + inter_len);

            execl("./integral.exe", "integral", argv[1], a, b, NULL );
        }
        curr_start = curr_start + inter_len;
    }
   
    FILE * fp = fopen(pipePath, "r");
    

    while (wait(NULL) > 0);
    
    sum=0;
    char buff[256];

    for(int i = 0;i < n;i++){
        fgets(buff, 256,fp);
        sum = sum + atof(buff);
    }

    clock_gettime(CLOCK_REALTIME, &time_end);
    printf("result: %f, step: %s, number of processes: %f, time: %lds\n", sum, argv[1], n, time_end.tv_sec - time_start.tv_sec);

    remove(pipePath);

    return 0;
}