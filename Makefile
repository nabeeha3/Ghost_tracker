CC = gcc
CFLAGS = -Wall -pthread

all: main

main: main.o defs.o house.o logger.o ghost.o utils.o
	$(CC) $(CFLAGS) -o main main.o defs.o house.o logger.o ghost.o utils.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

defs.o: defs.c
	$(CC) $(CFLAGS) -c defs.c

house.o: house.c
	$(CC) $(CFLAGS) -c house.c

logger.o: logger.c
	$(CC) $(CFLAGS) -c logger.c

ghost.o: ghost.c
	$(CC) $(CFLAGS) -c ghost.c

utils.o: utils.c
	$(CC) $(CFLAGS) -c utils.c

clean:
	rm -f *.o main
