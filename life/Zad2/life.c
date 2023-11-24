#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include "grid.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char** argv)
{
	if(argc != 2){
		fprintf(stderr, "One argument needed, provided different!\n");
		return -1;
	}

	int num_of_threads;

	sscanf(argv[1], "%d", &num_of_threads);

	srand(time(NULL));
	setlocale(LC_CTYPE, "");
	initscr(); // Start curses mode

	char *foreground = create_grid();
	char *background = create_grid();
	char *tmp;

	init_grid(foreground);
	if(create_threads(foreground, background, num_of_threads) == -1){
		fprintf(stderr, "Number of threads greater than number of squares!\n");
		endwin();
		destroy_grid(foreground);
		destroy_grid(background);
		return -1;
	}
	
	while (true)
	{
		draw_grid(foreground);
		usleep(500 * 1000);

		// Step simulation
		update_grid(foreground, background);
		tmp = foreground;
		foreground = background;
		background = tmp;
	}
	
	endwin(); // End curses mode
	destroy_grid(foreground);
	destroy_grid(background);

	return 0;
}
