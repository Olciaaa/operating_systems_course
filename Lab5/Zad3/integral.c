#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

double f(double x){
    return 4/(pow(x, 2) + 1);
}

int main(int argc, char *argv[]){
    if (argc < 4){
        fprintf(stderr,"Wrong number of arguments");
        return -1;
    }

    double step = atof(argv[1]);
    double sum = 0;
    double curr_x = atof(argv[2]);
    double end = atof(argv[3]);
    
    while (curr_x < end){
        sum = sum + f(curr_x)*step;
        curr_x = curr_x + step;
    }

    char buff[256];
    int len = snprintf(buff, 256, "%lf\n", sum);

    FILE * fp= fopen("./fifo", "w");
    fwrite(buff, sizeof(char), len, fp);
    fclose(fp);

    return 0;
}