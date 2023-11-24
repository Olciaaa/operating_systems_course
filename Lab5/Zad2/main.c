#include <stdlib.h>
#include <sys/param.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/times.h>
#include <time.h>

long double f(long double x){
    return 4.0/(x*x+1.0);
}

int main(int argc, char **argv){
    if(argc != 3){
        return -1;
    }

    long double step = atof(argv[1]);
    double n = atof(argv[2]);
    double inter_len = 1/n;
    double sum = 0;
    double curr_x = 0;

    int pipes[atoi(argv[2])]; 
    struct timespec time_start, time_end;

    clock_gettime(CLOCK_REALTIME, &time_start);

    for(int i = 0; i < n; i++){
        int fd[2];
        pipe(fd);
        long double num = step/n;

        if(fork() == 0){
            //printf("parent: %d, child: %d\n", getppid(), getpid());

            close(fd[0]);

            sum = 0;
            double curr_start = i * num;
            curr_x = curr_start;

            while (curr_x < curr_start + inter_len){
                sum = sum + f(curr_x)*step;
                curr_x = curr_x + step;
            }

            write(fd[1],&sum,sizeof(double));
            exit(0);
        }
        else{
            pipes[i] = fd[0];
            close(fd[1]);
        }
    }

    while (wait(NULL) > 0);
    
    sum=0;

    double buff;
    for(int i=0;i<n;i++){
        read(pipes[i],&buff,sizeof(double));
        sum = sum + buff;
    }

    //printf("Calculated value: %f\n" ,sum);
    clock_gettime(CLOCK_REALTIME, &time_end);

    printf("result: %f, step: %Lf, number of processes: %f, time: %lds\n", sum, step, n, time_end.tv_sec - time_start.tv_sec);

    return 0;
}