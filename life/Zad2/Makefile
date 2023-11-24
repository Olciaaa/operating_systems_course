CC = gcc
CFLAGS = -Wall

all: life
	
life: grid.c life.c
	$(CC) $(CFLAGS) grid.c life.c -o life -l ncurses -l pthread -lrt
	
clean:
	rm -f life
