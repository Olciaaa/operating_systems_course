//
// Created by olciaa on 03.03.23.
//
#ifndef LAB1_LIBRARY_H
#define LAB1_LIBRARY_H

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

int counter(dataTable* table, char* filepath);
dataTable *initDataTable(int size);
block *getBlock(dataTable *table, int idx);
int removeBlock(dataTable *table, int idx);
void freeTable(dataTable *table);

#endif //LAB1_LIBRARY_H
