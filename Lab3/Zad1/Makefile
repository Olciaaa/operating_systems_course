CC=gcc
CFLAGS =-Wall

all: app

app:
	$(CC) $(CFLAGS) -O2 -o main.exe main.c

test: app
	echo Results of programm: > results.txt
	./main.exe 10 > results.txt

clean:
	rm -f *.exe res.txt results.txt