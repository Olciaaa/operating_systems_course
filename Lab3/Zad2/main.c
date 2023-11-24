#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char** argcv){
    if(argc != 2){
        printf("Powinien być podany jeden argument!!!");
        return -1;
    }

    printf("%s ", argcv[0]);
    fflush(stdout);
    execl("/bin/ls", "ls", argcv[1], NULL);
    //execl("/bin/ls", argcv[1], NULL);

    return 0;
}
