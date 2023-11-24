all:
	gcc client.c -o client
	gcc server.c -o server

test:
	./server 7777 0.0.0.0
	# ./client Klient1 siec 0.0.0.0 7777

clean:
	rm -f server_log.txt server client