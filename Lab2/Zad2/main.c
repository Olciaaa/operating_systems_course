#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

struct timespec timeStart, timeEnd;
struct tms tmsStart, tmsEnd;

void reverseBlock(char str[]) {
    if (!str || !*str)
        return;

    size_t i = strlen(str) - 1;
    size_t j = 0;
    char ch;

    while (i > j) {
        ch = str[i];
        str[i] = str[j];
        str[j] = ch;
        i--;
        j++;
    }
}

void reverseWithBlocks(FILE *sourceFile, FILE *resultFile2) {
    int sizeOfBlock = 1024;
    char block[sizeof(char) * sizeOfBlock];

    fseek(sourceFile, 0L, SEEK_END);
    int flag = 1;

    while (flag == 1) {
        if (ftell(sourceFile) > 2 * sizeOfBlock) {
            long size = sizeOfBlock * 2;
            fseek(sourceFile, -size, SEEK_CUR);
            printf("%ld\n", ftell(sourceFile));

            fread(block, sizeof(char), sizeOfBlock, sourceFile);
            reverseBlock(block);
            fwrite(block, sizeof(char), sizeOfBlock, resultFile2);
        } else {
            int size = ftell(sourceFile);
            fseek(sourceFile, 0L, SEEK_SET);
            char blockSmall[sizeof(char) * size];

            fread(blockSmall, sizeof(char), size, sourceFile);
            reverseBlock(blockSmall);
            fwrite(blockSmall, sizeof(char), size, resultFile2);
            flag = 0;
        }

    }
}

void reverseCharByChar(FILE *sourceFile, FILE *resultFile1) {
    char block[sizeof(char)];

    fseek(sourceFile, 1L, SEEK_END);

    while (ftell(sourceFile) > 1) {
        fseek(sourceFile, -2L, SEEK_CUR);
        fread(block, sizeof(char), 1, sourceFile);
        fwrite(block, sizeof(char), 1, resultFile1);
        //printf("%ld\n", ftell(sourceFile));
    }
}

void useFread(FILE *sourceFile, FILE *resultFile1, FILE *resultFile2) {
    clock_gettime(CLOCK_REALTIME, &timeStart);
    times(&tmsStart);
    reverseCharByChar(sourceFile, resultFile1);
    clock_gettime(CLOCK_REALTIME, &timeEnd);
    times(&tmsEnd);
    printf("Run times for reverse char by char:\nREAL: %ldns\nUSER: %ldticks\nSYSTEM: %ldticks\n\n",
           timeEnd.tv_nsec - timeStart.tv_nsec,
           tmsEnd.tms_cutime - tmsStart.tms_utime, tmsEnd.tms_cstime - tmsStart.tms_stime);

    clock_gettime(CLOCK_REALTIME, &timeStart);
    times(&tmsStart);
    reverseWithBlocks(sourceFile, resultFile2);
    clock_gettime(CLOCK_REALTIME, &timeEnd);
    times(&tmsEnd);
    printf("Run times for reverse with blocks:\nREAL: %ldns\nUSER: %ldticks\nSYSTEM: %ldticks\n\n",
           timeEnd.tv_nsec - timeStart.tv_nsec,
           tmsEnd.tms_cutime - tmsStart.tms_utime, tmsEnd.tms_cstime - tmsStart.tms_stime);

    fclose(sourceFile);
    fclose(resultFile1);
    fclose(resultFile2);
}

int main(int argc, char **argv) {
    if (argc != 4) {
        printf("3 arguments needed!");
        return -1;
    }

    FILE *sourceFile = fopen(argv[1], "r");
    FILE *resultFile1 = fopen(argv[2], "w");
    FILE *resultFile2 = fopen(argv[3], "w");

    if (sourceFile == NULL) {
        printf("Source file not found!");
        return -1;
    }

    useFread(sourceFile, resultFile1, resultFile2);
}
