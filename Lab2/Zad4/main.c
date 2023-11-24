#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>

long long size = 0;

static int getInfo(const char *fpath, const struct stat *sb, int tflag){
    if(!S_ISDIR(sb->st_mode)){
        printf("Size: %ld, file name: %s\n", sb->st_size, fpath);
        size += sb ->st_size;
    }

    return 0;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("1 argument needed!");
        return -1;
    }

    ftw(argv[1], getInfo, 20);
    printf("%lld total size\n", size);
    return 0;
}