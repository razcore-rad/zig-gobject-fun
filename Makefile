DEBUG=-g
CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -pedantic \
       $(shell pkg-config --libs --cflags gtk+-3.0) \
       $(DEBUG)
BINARY=test
OBJECTS=main.o custom.o

default: all

all: $(OBJECTS)
	$(CC) -o$(BINARY) $(OBJECTS) $(CFLAGS)

%.o:

clean:
	rm -f $(OBJECTS) $(BINARY)
