#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>

int main(){
    DIR *folder = opendir("./");

    if(!folder){
        printf("Cannot open local folder!");
        return -1;
    }

    long long size = 0;
    struct dirent* dir;
    struct stat buffor;

    while((dir = readdir(folder)) != NULL){
        stat(dir->d_name, &buffor);

        if(!S_ISDIR(buffor.st_mode)){
            printf("%ld %s\n", buffor.st_size, dir->d_name);
            size += buffor.st_size;
        }
    }

    printf("%lld total size\n", size);
    free(dir);
    closedir(folder);
    return 0;
}