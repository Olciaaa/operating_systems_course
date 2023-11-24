CC=gcc
CFLAGS =-Wall

all: app

app:
	$(CC) $(CFLAGS) -O2 -o main.exe main.c
	$(CC) $(CFLAGS) -O2 -o mainExec.exe mainExec.c

test: app
	echo Results of program: > results.txt
	echo For ignore: >> results.txt
	./main.exe ignore >> results.txt
	echo For handler: >> results.txt
	./main.exe handler >> results.txt
	echo For mask: >> results.txt
	./main.exe mask >> results.txt
	echo For pending: >> results.txt
	./main.exe pending >> results.txt

clean:
	rm -f *.exe res.txt results.txt