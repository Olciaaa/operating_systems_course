CC=gcc
CFLAGS =-Wall

all: app

app:
	$(CC) $(CFLAGS) -O2 -o main.exe main.c

test: app
	echo Results of program: > results.txt
	./main.exe root test test >> results.txt

clean:
	rm -f *.exe res.txt results.txt