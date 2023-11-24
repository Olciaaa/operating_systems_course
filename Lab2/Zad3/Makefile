CC=gcc
CFLAGS =-Wall

all: app

app:
	$(CC) $(CFLAGS) -O2 -o main.exe main.c

test: app
	echo Results of programm: > pomiar_zad_3.txt
	./main.exe > pomiar_zad_3.txt
	wc --bytes * > tmp
clean:
	rm -f *.exe res.txt pomiar_zad_3.txt tmp