CC=gcc
CFLAGS=-Wall

all:
	make static
	make shared
	make dll

static: library.c application.c
	$(CC) $(CFLAGS) -c library.c
	ar rcs library.a library.o
	$(CC) $(CFLAGS) application.c -o appStatic.exe -L . library.a

shared: library.c application.c
	$(CC) -fPIC $(CFLAGS) -c library.c
	$(CC) $(CFLAGS) -shared -o liblibrary.so library.o
	$(CC) $(CFLAGS) application.c -L. -llibrary -o appShared.exe -Wl,-rpath=.

dll: library.c application.c
	$(CC) -fPIC $(CFLAGS) -c library.c
	$(CC) $(CFLAGS) -shared -o library.so library.o
	$(CC) $(CFLAGS) application.c -o appDynamic.exe -ldl -D DYNAMIC

clean:
	rm -f *.o *.gch *.a *.so *.exe test.in *.txt