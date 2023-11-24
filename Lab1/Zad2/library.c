#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "library.h"

#define TMP_FILE_NAME "/tmp/tempfile"

int saveToTempFile(char *filepath) {
    char temp[strlen(filepath) + 25];

    strcpy(temp, "wc -lwm ");
    strcat(temp, filepath);
    strcat(temp, " > ");
    strcat(temp, TMP_FILE_NAME);

    int check = system(temp);
    return check;
}

dataTable *initDataTable(int size) {
    dataTable *table = malloc(sizeof(dataTable));
    table->capacity = size;
    table->numOfBlocks = 0;
    table->blocks = calloc(table->capacity, sizeof(block));

    return table;
}

int counter(dataTable *table, char *filepath) {
    if (saveToTempFile(filepath) != 0 || table == NULL) {
        return 0;
    }

    FILE *file = fopen(TMP_FILE_NAME, "r");

    if (file == NULL) {
        return 0;
    }

    block *b1 = calloc(1, sizeof(block));
    b1->charsCount = 5;
    b1->fileName = malloc(strlen(filepath) * sizeof(char));
    strcpy(b1->fileName, filepath);

    int numberArray[3];
    for (int i = 0; i < 3; i++) {
        fscanf(file, "%d ", &numberArray[i]);
    }

    remove(TMP_FILE_NAME);
    fclose(file);

    if (table->numOfBlocks == table->capacity) {
        table->capacity *= 2;
        table->blocks = realloc(table->blocks, table->capacity * sizeof(block));
    }

    b1->linesCount = numberArray[0];
    b1->wordsCount = numberArray[1];
    b1->charsCount = numberArray[2];
    table->blocks[table->numOfBlocks] = *b1;
    table->numOfBlocks += 1;
    return 1;
}

block *getBlock(dataTable *table, int idx) {
    if (table == NULL || idx < 0 || idx >= table->numOfBlocks) {
        return NULL;
    }

    //return table->blocks + idx * sizeof(block);
    return &table->blocks[idx];
}

void freeBlock(block *block) {
    if(block != NULL){
        free(block->fileName);
    }
}

int removeBlock(dataTable *table, int idx) {
    if (table == NULL || idx < 0 || idx >= table->numOfBlocks) {
        return 0;
    }

    freeBlock(&table->blocks[idx]);

    for (int i = idx + 1; i < table->numOfBlocks; ++i) {
        table->blocks[i - 1] = table->blocks[i];
    }

    table->numOfBlocks--;
    return 1;
}

void freeTable(dataTable *table) {
    for (int i = 0; i < table->numOfBlocks; i++) {
        freeBlock(&table->blocks[i]);
    }

    if(table != NULL){
        free(table->blocks);
    }

    free(table);
}