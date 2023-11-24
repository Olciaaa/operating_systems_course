//
// Created by olciaa on 07.03.23.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/times.h>

#ifdef DYNAMIC
#include <dlfcn.h>

typedef struct block {
    int linesCount;
    int wordsCount;
    int charsCount;
    char *fileName;
} block;

typedef struct dataTable {
    block *blocks;
    int capacity;
    int numOfBlocks;
} dataTable;
#else

#include "library.h"

#endif

dataTable *data;
struct timespec start, stop, delta;
double timesCounted[3];
struct tms buf;
int initialized = 0;

char **readCommand() {
    char wiersz[50];
    fgets(wiersz, 50, stdin);

    size_t len = strlen(wiersz);
    if (len > 0 && wiersz[len - 1] == '\n') {
        wiersz[--len] = '\0';
    }

    char *sentence = strtok(wiersz, " ");
    char **wordsArray = malloc(3 * sizeof(char *));
    for (int i = 0; i < 3; i++) {
        wordsArray[i] = NULL;
        if (sentence != NULL) {
            wordsArray[i] = malloc(sizeof(char) * strlen(sentence));
            strcpy(wordsArray[i], sentence);
            sentence = strtok(NULL, " ");
        }
    }

    free(sentence);
    return wordsArray;
}

int isNumber(char s[]) {
    for (int i = 0; s[i] != '\0'; i++) {
        if (isdigit(s[i]) == 0)
            return 0;
    }
    return 1;
}

void countTimes() {
    clock_gettime(CLOCK_REALTIME, &stop);
    struct tms end;
    times(&end);
    timesCounted[0] = (double) stop.tv_nsec - start.tv_nsec;
    timesCounted[1] = (double) end.tms_cutime - buf.tms_cutime;
    timesCounted[2] = (double) end.tms_cstime - buf.tms_cstime;
}

int main() {
#ifdef DYNAMIC
    void *handle = dlopen("./liblibrary.so", RTLD_LAZY);

    dataTable* (*initDataTable)(int) = dlsym(handle, "initDataTable");
    int (*counter)(dataTable*, char*) = dlsym(handle, "counter");
    block* (*getBlock)(dataTable*, int) = dlsym(handle, "getBlock");
    int (*removeBlock)(dataTable*, int) = dlsym(handle, "removeBlock");
    void (*freeTable)(dataTable*) = dlsym(handle, "freeTable");
#endif
    while (1) {
        char **wordsArray = readCommand();

        if (strcmp(wordsArray[0], "init") == 0 && wordsArray[1] != NULL && isNumber(wordsArray[1]) == 1) {
            clock_gettime(CLOCK_REALTIME, &start);
            times(&buf);
            data = initDataTable(atoi(wordsArray[1]));
            countTimes();

            printf("Init: %d, real time: %f, user time: %f, system time: %f\n", atoi(wordsArray[1]), timesCounted[0],
                   timesCounted[1], timesCounted[2]);
            initialized = 1;
        } else if (strcmp(wordsArray[0], "count") == 0 && wordsArray[1] != NULL) {
            clock_gettime(CLOCK_REALTIME, &start);
            times(&buf);
            if (counter(data, wordsArray[1]) == 1) {
                countTimes();
                printf("count: %s, real time: %f, user time: %f, system time: %f\n", wordsArray[1], timesCounted[0],
                       timesCounted[1], timesCounted[2]);
            } else {
                printf("no file or table not initialized, please try to use command init [size] \n");
            }
        } else if (strcmp(wordsArray[0], "show") == 0 && wordsArray[1] != NULL && isNumber(wordsArray[1]) == 1) {
            clock_gettime(CLOCK_REALTIME, &start);
            times(&buf);
            block *b = getBlock(data, atoi(wordsArray[1]));
            countTimes();
            if (b != NULL) {
                printf("lines: %d, words: %d, chars: %d, filepath: %s, real time: %f, user time: %f, system time: %f\n",
                       b->linesCount, b->wordsCount, b->charsCount,
                       b->fileName, timesCounted[0], timesCounted[1], timesCounted[2]);
            } else {
                printf("no block found with index %d or table not initialized \n", atoi(wordsArray[1]));
            }
        } else if (strcmp(wordsArray[0], "delete") == 0 && strcmp(wordsArray[1], "index") == 0 &&
                   wordsArray[2] != NULL &&
                   isNumber(wordsArray[2]) == 1) {
            clock_gettime(CLOCK_REALTIME, &start);
            times(&buf);
            if (removeBlock(data, atoi(wordsArray[2])) == 1) {
                countTimes();
                printf("block deleted successfully, real time: %f, user time: %f, system time: %f \n", timesCounted[0],
                       timesCounted[1], timesCounted[2]);
            } else {
                printf("no block found with index %d or table not initialized \n", atoi(wordsArray[2]));
            }
        } else if (strcmp(wordsArray[0], "destroy") == 0) {
            if (initialized == 1) {
                clock_gettime(CLOCK_REALTIME, &start);
                times(&buf);
                freeTable(data);
                countTimes();
                printf("destroy, real time: %f, user time: %f, system time: %f\n", timesCounted[0], timesCounted[1],
                       timesCounted[2]);
                initialized = 0;
                data = NULL;
            }
        } else if (strcmp(wordsArray[0], "exit") == 0) {
            return 0;
        } else {
            printf("try again, no command found \n");
        }

//        for (int i = 0; i < 3; i++) {
//            free(wordsArray[i]);
//        }
//        free(wordsArray);
    }
    return 0;
}