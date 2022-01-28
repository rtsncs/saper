# Dominik Brzeziński
CFLAGS = -Wall -ansi
CC = gcc
LDFLAGS = -lncurses -lm

all: main.o board.o input.o ui.o
	$(CC) $(CFLAGS) -o saper main.o board.o input.o ui.o $(LDFLAGS)

main.o: main.c colors.h board.h input.h ui.h
	$(CC) $(CFLAGS) -c main.c

board.o: board.c board.h
	$(CC) $(CFLAGS) -c board.c

input.o: input.c board.h
	$(CC) $(CFLAGS) -c input.c

clean:
	rm *.o saper

