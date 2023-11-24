CC=gcc
CFLAGS =-Wall

all: app

app:
	$(CC) $(CFLAGS) -O2 -o main.exe main.c

test: app
	./main.exe

clean:
	rm -f *.exe res.txt results.txt