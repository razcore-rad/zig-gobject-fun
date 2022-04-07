DEBUG=-g
CC=gcc
CFLAGS=-Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -std=c11 -pedantic \
       $(shell pkg-config --libs --cflags gtk4) \
       $(DEBUG)
BINARY=main
OBJECTS=demo-animal.o demo-cat.o

default: $(BINARY)

$(BINARY): $(OBJECTS)

clean:
	rm -f $(OBJECTS) $(BINARY)
