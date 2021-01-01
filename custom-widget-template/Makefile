DEBUG=-g
CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -pedantic \
       $(shell pkg-config --libs --cflags gtk+-3.0) \
       $(DEBUG)
BINARY=main
OBJECTS=custom.o

default: $(BINARY)

$(BINARY): $(OBJECTS)

%.o:

clean:
	rm -f $(OBJECTS) $(BINARY)
