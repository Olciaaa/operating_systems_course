#include "grid.h"
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

const int grid_width = 30;
const int grid_height = 30;
pthread_t *threads;
int threads_amount;

void handler(int sig){};

char *create_grid()
{
    return malloc(sizeof(char) * grid_width * grid_height);
}

void destroy_grid(char *grid)
{
    free(grid);
}

void draw_grid(char *grid)
{
    for (int i = 0; i < grid_height; ++i)
    {
        // Two characters for more uniform spaces (vertical vs horizontal)
        for (int j = 0; j < grid_width; ++j)
        {
            if (grid[i * grid_width + j])
            {
                mvprintw(i, j * 2, "■");
                mvprintw(i, j * 2 + 1, " ");
            }
            else
            {
                mvprintw(i, j * 2, " ");
                mvprintw(i, j * 2 + 1, " ");
            }
        }
    }

    refresh();
}

void init_grid(char *grid)
{
    for (int i = 0; i < grid_width * grid_height; ++i)
        grid[i] = rand() % 2 == 0;
}

bool is_alive(int row, int col, char *grid)
{

    int count = 0;
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0)
            {
                continue;
            }
            int r = row + i;
            int c = col + j;
            if (r < 0 || r >= grid_height || c < 0 || c >= grid_width)
            {
                continue;
            }
            if (grid[grid_width * r + c])
            {
                count++;
            }
        }
    }

    if (grid[row * grid_width + col])
    {
        if (count == 2 || count == 3)
            return true;
        else
            return false;
    }
    else
    {
        if (count == 3)
            return true;
        else
            return false;
    }
}

void update_grid(char *src, char *dst)
{
    for (int i = 0; i < grid_height*grid_width; ++i)
    {
        pthread_kill(threads[i], SIGUSR1);
    }
    // for (int i = 0; i < grid_height; ++i)
    // {
    //     for (int j = 0; j < grid_width; ++j)
    //     {
    //         //pthread_kill(threads[i * j], SIGUSR1);
    //         dst[i * grid_width + j] = is_alive(i, j, src);
    //     }
    // }
}

void create_threads(char* src, char* dst, int n){
    threads_amount = n;
    if(threads_amount > grid_height*grid_width){
        printf("Za dużo wątków dla zbyt małej planszy\n");
        return -1;
    }

    threads = malloc(sizeof(pthread_t) * threads_amount);
    signal(SIGUSR1, handler);

    int sqrs_per_thread = grid_height*grid_width/threads_amount;
    int rest_of_sqrs = grid_height*grid_width - sqrs_per_thread*threads_amount;
    int sqr_index = 0;

    for (int i = 0; i < threads_amount; i++)
    {
        args* arg = malloc(sizeof(args));
        arg->src = src;
        arg->dst = dst;

        arg->first_sqr = sqr_index;
        arg->last_sqr = sqr_index+sqrs_per_thread+rest_of_sqrs;
        sqr_index += sqrs_per_thread+rest_of_sqrs;
        
        if(rest_of_sqrs > 0)
            rest_of_sqrs--;

        printf("%d %d\n", arg->first_sqr, arg->last_sqr);

        pthread_create(&threads[i], NULL, update_square, (void*) arg);
    }

    return 0;
}

void* update_square(void* arg){
    args* new = (args*) arg;

    while (true)
    {

        for(int i = new->first_sqr; i < new->last_sqr; i++){
            int x = (i) / grid_width;
            int y = (i) % grid_width;

            new->dst[i] = is_alive(x, y, new->src);
        }

        pause();

        char* tmp = new->src;
        new->src = new->dst;
        new->dst = tmp;
    }
    
    return NULL;
}