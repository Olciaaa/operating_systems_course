CC = gcc
CFLAGS = -Wall
LIB = library

all:
	make library_static
	make library_shared

library_static: $(LIB).c
	$(CC) $(CFLAGS) -c $(LIB).c
	ar cr $(LIB).a $(LIB).o

library_shared: $(LIB).c
	$(CC) $(CFLAGS) -c -fPIC $(LIB).c
	$(CC) $(CFLAGS) -shared -o $(LIB).so $(LIB).o

clean:
	rm -f *.gch *.o *.a *.so
