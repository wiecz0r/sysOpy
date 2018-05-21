CC = gcc -Wall

all:
	$(CC) functions.c golibroda.c -o golibroda -lpthread -lrt
	$(CC) functions.c clients.c -o clients -lpthread -lrt

clear:
	rm golibroda clients
