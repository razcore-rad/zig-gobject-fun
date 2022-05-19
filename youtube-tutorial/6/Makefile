DEBUG=-g
CC=gcc
CFLAGS=-Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -std=c11 -pedantic \
       $(shell pkg-config --cflags gtk4) \
       $(DEBUG)
LDFLAGS=$(shell pkg-config --libs gtk4)
BINARY=main
OBJECTS=demo-animal.o demo-cat.o

default: $(BINARY)

$(BINARY): $(OBJECTS)

clean:
	rm -f $(OBJECTS) $(BINARY)
