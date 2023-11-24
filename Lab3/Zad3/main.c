#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ftw.h>
#include <linux/limits.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>

char *strToFind;
struct stat buffor;

void checkForStr(char *filename){
    FILE *file = fopen(filename, "r");

    if (file == NULL){
        perror("CANNOT OPEN FILE!");
        return;
    }

    size_t sizeOfStr = strlen(strToFind);
    char *block = calloc(sizeof(char), sizeOfStr);
    fread(block, sizeof(char), sizeOfStr, file);
    if (strcmp(block, strToFind) == 0) {
        printf("Filepath: %s, Parent: %d, Child: %d \n", filename, (int)getppid(), (int)getpid());
    }
    free(block);
    fclose(file);
}

void openDir(char* filename){
    //printf("wchodzę %s \n", filename);
    stat(filename, &buffor);
    if (!S_ISDIR(buffor.st_mode)) {
        checkForStr(filename);
        return;
    }

    struct dirent *dir;
    DIR *folder = opendir(filename);
    pid_t child_id = fork();
    if (child_id == 0){
        while ((dir = readdir(folder)) != NULL) {
            if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
                char temp[strlen(filename) + strlen(dir->d_name)];
                strcpy(temp, filename);
                strcat(temp, "/");
                strcat(temp, dir->d_name);

                openDir(temp);
            }
        }
        exit(0);
    }
}

int main(int argc, char** argcv){
    if(argc != 3){
        perror("Powinny być podane dwa argumenty!!!");
        return -1;
    }

    if (strlen(argcv[2]) > 255) {
        perror("second argument too long!!!");
        return 2;
    }

    strToFind = argcv[2];
    openDir(argcv[1]);

    while(wait(NULL) > 0);

    return 0;
}
