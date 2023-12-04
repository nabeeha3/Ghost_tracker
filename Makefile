CC = gcc
CFLAGS = -Wall -pthread  # Add -pthread to include pthread library

all: main

main: main.o house.o logger.o ghost.o utils.o hunter.o
	$(CC) $(CFLAGS) -o main main.o house.o logger.o ghost.o utils.o hunter.o

main.o: main.c defs.h
	$(CC) $(CFLAGS) -c main.c

house.o: house.c defs.h
	$(CC) $(CFLAGS) -c house.c

logger.o: logger.c defs.h
	$(CC) $(CFLAGS) -c logger.c

ghost.o: ghost.c defs.h
	$(CC) $(CFLAGS) -c ghost.c

hunter.o: hunter.c defs.h
	$(CC) $(CFLAGS) -c hunter.c

utils.o: utils.c defs.h
	$(CC) $(CFLAGS) -c utils.c

clean:
	rm -f *.o main
