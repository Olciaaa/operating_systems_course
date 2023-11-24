CC=gcc
CFLAGS =-Wall

all: app

app:
	$(CC) $(CFLAGS) -O2 -o catcher.exe catcher.c
	$(CC) $(CFLAGS) -O2 -o sender.exe sender.c

test: app
	./catcher.exe

clean:
	rm -f *.exe