CC=gcc
CFLAGS=-std=gnu99 -Wall -pedantic
LFLAGS=-lpthread -lrt
BIN=projekt2
SOURCE=projekt2.c

all:
	$(CC) $(CFLAGS) $(SOURCE) -o $(BIN) $(LFLAGS)

run: all
	./$(BIN)

clean:
	rm $(BIN)