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

void changeAllOccurrences(char block[], int charToReplace, int newChar) {
    char old = charToReplace;
    char new = newChar;

    printf("Char to change: %c\n", old);
    printf("New char: %c\n", new);

    char *currPos = strchr(block,old);
    while (currPos) {
        *currPos = new;
        currPos = strchr(currPos,old);
    }
}

int isNumber(char s[]) {
    for (int i = 0; s[i] != '\0'; i++) {
        if (isdigit(s[i]) == 0)
            return 0;
    }
    return 1;
}

void useFread(FILE *sourceFile, FILE *resultFile, int charToReplace, int newChar, size_t fileSize) {
    char block[fileSize * sizeof(char)];

    clock_gettime(CLOCK_REALTIME, &timeStart);
    times(&tmsStart);

    fread(block, sizeof(char), fileSize, sourceFile);
    changeAllOccurrences(block, charToReplace, newChar);
    fwrite(block, sizeof(char), fileSize, resultFile);

    clock_gettime(CLOCK_REALTIME, &timeEnd);
    times(&tmsEnd);

    printf("Run times for fread and fwrite:\nREAL: %ldns\nUSER: %ldticks\nSYSTEM: %ldticks\n\n", timeEnd.tv_nsec - timeStart.tv_nsec,
           tmsEnd.tms_cutime - tmsStart.tms_utime, tmsEnd.tms_cstime - tmsStart.tms_stime);

    fclose(sourceFile);
    fclose(resultFile);
}

void useRead(char *fileS, char *fileR, int charToReplace, int newChar, size_t fileSize) {
    int sourceFile = open(fileS, O_RDONLY);
    int resultFile = open(fileR, O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR);
    char block[fileSize * sizeof(char)];

    clock_gettime(CLOCK_REALTIME, &timeStart);
    times(&tmsStart);

    read(sourceFile, block, fileSize);
    changeAllOccurrences(block, charToReplace, newChar);
    write(resultFile, block, fileSize);

    clock_gettime(CLOCK_REALTIME, &timeEnd);
    times(&tmsEnd);

    printf("Run times for read and write:\nREAL: %ldns\nUSER: %ldticks\nSYSTEM: %ldticks\n\n", timeEnd.tv_nsec - timeStart.tv_nsec,
           tmsEnd.tms_cutime - tmsStart.tms_utime, tmsEnd.tms_cstime - tmsStart.tms_stime);

    close(sourceFile);
    close(resultFile);
}

int main(int argc, char **argv) {
    if (argc != 5) {
        printf("4 arguments needed!");
        return -1;
    }

    if (isNumber(argv[1]) == 0 || isNumber(argv[2]) == 0) {
        printf("First two arguments must be ASCII!");
        return -1;
    }

    int charToChange = atoi(argv[1]);
    int newChar = atoi(argv[2]);

    FILE *sourceFile = fopen(argv[3], "r");
    FILE *resultFile = fopen(argv[4], "w");

    if (sourceFile == NULL) {
        printf("Source file not found!");
        return -1;
    }

    fseek(sourceFile, 0L, SEEK_END);
    size_t fileSize = (size_t) ftell(sourceFile);
    fseek(sourceFile, 0L, SEEK_SET);

    useFread(sourceFile, resultFile, charToChange, newChar, fileSize);
    useRead(argv[3], argv[4], charToChange, newChar, fileSize);
}
