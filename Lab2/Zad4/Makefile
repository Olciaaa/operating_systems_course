CC=gcc
CFLAGS =-Wall

all: app

app:
	$(CC) $(CFLAGS) -O2 -o main.exe main.c

test: app
	echo Results of programm: > pomiar_zad_4.txt
	./main.exe /home > pomiar_zad_4.txt
	#find /home | xargs wc --bytes > tmp
clean:
	rm -f *.exe res.txt pomiar_zad_4.txt tmp