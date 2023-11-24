#pragma once
#include <stdbool.h>

typedef struct{
    char* src;
    char* dst;
    int first_sqr;
    int last_sqr;
} args;

void handler(int signo);
char *create_grid();
void destroy_grid(char *grid);
void draw_grid(char *grid);
void init_grid(char *grid);
bool is_alive(int row, int col, char *grid);
void update_grid(char *src, char *dst);
int create_threads(char *src, char *dst, int threads_amount);
void* update_square(void* arg);