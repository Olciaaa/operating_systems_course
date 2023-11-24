CC=gcc
CFLAGS =-Wall

all: app

app:
	$(CC) $(CFLAGS) -O2 -o integral.exe integral.c
	$(CC) $(CFLAGS) -O2 -o main.exe main.c

test: app
	echo Results of program: > results.txt
	./main.exe 0.00000001 3 >> results.txt
	./main.exe 0.00000001 100 >> results.txt
	./main.exe 0.00000001 500 >> results.txt
	./main.exe 0.000000001 3 >> results.txt
	./main.exe 0.000000001 100 >> results.txt
	./main.exe 0.000000001 500 >> results.txt
	./main.exe 0.0000000001 3 >> results.txt
	./main.exe 0.0000000001 100 >> results.txt
	./main.exe 0.0000000001 500 >> results.txt

clean:
	rm -f *.exe results.txt