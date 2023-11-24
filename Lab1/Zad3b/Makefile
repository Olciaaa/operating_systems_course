CC=gcc
CFLAGS=-Wall

all:
	make static_O1
	make static_O2
	make static_O3
	make shared_O1
	make shared_O2
	make shared_O3
	make dll_O1
	make dll_O2
	make dll_O3

test:
	make test_create
	./appStatic_O1.exe < test.txt > ../reports/resultStatic_O1.txt
	./appStatic_O2.exe < test.txt > ../reports/resultStatic_O2.txt
	./appStatic_O3.exe < test.txt > ../reports/resultStatic_O3.txt
	./appShared_O1.exe < test.txt > ../reports/resultShared_O1.txt
	./appShared_O2.exe < test.txt > ../reports/resultShared_O2.txt
	./appShared_O3.exe < test.txt > ../reports/resultShared_O3.txt
	./appDynamic_O1.exe < test.txt > ../reports/resultDynamic_O1.txt
	./appDynamic_O2.exe < test.txt > ../reports/resultDynamic_O2.txt
	./appDynamic_O3.exe < test.txt > ../reports/resultDynamic_O3.txt

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

static_O1: library.c application.c
	$(CC) $(CFLAGS) -O1 -c library.c
	ar rcs liblibrary_O1.a library.o
	$(CC) $(CFLAGS) -O1 application.c -o appStatic_O1.exe -L . -llibrary_O1

static_O2: library.c application.c
	$(CC) $(CFLAGS) -O2 -c library.c
	ar rcs liblibrary_O2.a library.o
	$(CC) $(CFLAGS) -O2 application.c -o appStatic_O2.exe -L . -llibrary_O2

static_O3: library.c application.c
	$(CC) $(CFLAGS) -O3 -c library.c
	ar rcs liblibrary_O3.a library.o
	$(CC) $(CFLAGS) -O3 application.c -o appStatic_O3.exe -L . -llibrary_O3

shared_O1: library.c application.c
	$(CC) -fPIC $(CFLAGS) -O1 -c library.c
	$(CC) $(CFLAGS) -O1 -shared -o liblibrary.so library.o
	$(CC) $(CFLAGS) -O1 application.c -L. -llibrary -o appShared_O1.exe -Wl,-rpath=.

shared_O2: library.c application.c
	$(CC) -fPIC $(CFLAGS) -O2 -c library.c
	$(CC) $(CFLAGS) -O2 -shared -o liblibrary.so library.o
	$(CC) $(CFLAGS) -O2 application.c -L. -llibrary -o appShared_O2.exe -Wl,-rpath=.

shared_O3: library.c application.c
	$(CC) -fPIC $(CFLAGS) -O3 -c library.c
	$(CC) $(CFLAGS) -O3 -shared -o liblibrary.so library.o
	$(CC) $(CFLAGS) -O3 application.c -L. -llibrary -o appShared_O3.exe -Wl,-rpath=.

dll_O1: library.c application.c
	$(CC) -fPIC $(CFLAGS) -O1 -c library.c
	$(CC) $(CFLAGS) -O1 -shared -o liblibrary.so library.o
	$(CC) $(CFLAGS) -O1 application.c -o appDynamic_O1.exe -ldl -D DYNAMIC

dll_O2: library.c application.c
	$(CC) -fPIC $(CFLAGS) -O2 -c library.c
	$(CC) $(CFLAGS) -O2 -shared -o liblibrary.so library.o
	$(CC) $(CFLAGS) -O2 application.c -o appDynamic_O2.exe -ldl -D DYNAMIC

dll_O3: library.c application.c
	$(CC) -fPIC $(CFLAGS) -O3 -c library.c
	$(CC) $(CFLAGS) -O3 -shared -o liblibrary.so library.o
	$(CC) $(CFLAGS) -O3 application.c -o appDynamic_O3.exe -ldl -D DYNAMIC

clean:
	rm -f *.o *.gch *.a *.so *.exe test.in *.txt