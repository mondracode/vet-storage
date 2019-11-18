CC=gcc

server: p2-dogServer.c
	$(CC) p2-dogServer.c myqueue.o -o dogServer -lpthread


client: p2-dogClient.c
	$(CC) p2-dogClient.c -o dogClient

generator: struct-generator.c
	$(CC) struct-generator.c -o generator

all: p2-dogServer.c p2-dogClient.c struct-generator.c
	$(CC) p2-dogServer.c myqueue.o -o dogServer -lpthread
	$(CC) p2-dogClient.c -o dogClient
	$(CC) struct-generator.c -o generator
