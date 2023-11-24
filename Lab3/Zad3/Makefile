CC=gcc
CFLAGS =-Wall

all: app

app:
	$(CC) $(CFLAGS) -O2 -o main.exe main.c

test: app
	echo Results of programm: > results.txt
	./main.exe /home/olciaa/Sysopy/Lab3 "#include"> results.txt

clean:
	rm -f *.exe res.txt results.txt