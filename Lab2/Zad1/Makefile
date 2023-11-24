CC=gcc
CFLAGS =-Wall

all: app

app:
	$(CC) $(CFLAGS) -O2 -o main.exe main.c

test: app
	echo Results of programm: > pomiar_zad_1.txt
	./main.exe "97" "98" test.txt res.txt > pomiar_zad_1.txt
	tr a b < test.txt > tmp
	diff -s tmp res.txt

clean:
	rm -f *.exe res.txt pomiar_zad_1.txt tmp