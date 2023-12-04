CC = gcc
CFLAGS = -Wall -pthread

all: main

main: main.o house.o logger.o ghost.o utils.o
	$(CC) $(CFLAGS) -o main main.o house.o logger.o ghost.o utils.o

main.o: main.c defs.h
	$(CC) $(CFLAGS) -c main.c

house.o: house.c defs.h
	$(CC) $(CFLAGS) -c house.c

logger.o: logger.c defs.h
	$(CC) $(CFLAGS) -c logger.c

ghost.o: ghost.c defs.h
	$(CC) $(CFLAGS) -c ghost.c

utils.o: utils.c defs.h
	$(CC) $(CFLAGS) -c utils.c

clean:
	rm -f *.o main

