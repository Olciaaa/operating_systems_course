CC=gcc
CFLAGS =-Wall

all: app

app:
	$(CC) $(CFLAGS) -O2 -o main.exe main.c

test: app
	echo Results of programm: > pomiar_zad_2.txt
	./main.exe test.txt resVersion1.txt resVersion2.txt > pomiar_zad_2.txt
	./main.exe resVersion1.txt test1.txt test2.txt
	diff -s test.txt test1.txt
	diff -s test.txt test2.txt
	diff -s resVersion1.txt resVersion2.txt
clean:
	rm -f *.exe res.txt pomiar_zad_2.txt resVersion1.txt resVersion2.txt test1.txt test2.txt