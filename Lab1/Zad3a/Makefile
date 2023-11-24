CC=gcc
CFLAGS=-Wall

all:
	make static
	make shared
	make dll

test:
	make test_create
	./appStatic.exe < test.txt > ../reports/resultStatic.txt
	./appShared.exe < test.txt > ../reports/resultShared.txt
	./appDynamic.exe < test.txt > ../reports/resultDynamic.txt

test_create:
	echo "init 5" >> test.txt
	echo "count library.c" >> test.txt
	echo "count library.h" >> test.txt
	echo "count application.c" >> test.txt
	echo "count Makefile" >> test.txt
	echo "show 0" >> test.txt
	echo "show 1" >> test.txt
	echo "show 2" >> test.txt
	echo "show 3" >> test.txt
	echo "delete index 0" >> test.txt
	echo "delete index 0" >> test.txt
	echo "delete index 0" >> test.txt
	echo "delete index 0" >> test.txt
	echo "destroy" >> test.txt
	echo "exit" >> test.txt

static: library.c application.c
	$(CC) $(CFLAGS) -c library.c
	ar rcs liblibrary.a library.o
	$(CC) $(CFLAGS) application.c -o appStatic.exe -L . -llibrary

shared: library.c application.c
	$(CC) -fPIC $(CFLAGS) -c library.c
	$(CC) $(CFLAGS) -shared -o liblibrary.so library.o
	$(CC) $(CFLAGS) application.c -L. -llibrary -o appShared.exe -Wl,-rpath=.

dll: library.c application.c
	$(CC) -fPIC $(CFLAGS) -c library.c
	$(CC) $(CFLAGS) -shared -o liblibrary.so library.o
	$(CC) $(CFLAGS) application.c -o appDynamic.exe -ldl -D DYNAMIC

clean:
	rm -f *.o *.gch *.a *.so *.exe test.in *.txt