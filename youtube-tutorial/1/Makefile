DEBUG=-g
CC=gcc
CFLAGS=-Wall -Wextra -std=c11 -pedantic \
       $(shell pkg-config --cflags gtk4) \
       $(DEBUG)
LDFLAGS=$(shell pkg-config --libs gtk4)
BINARY=main
OBJECTS=demowidget.o

default: $(BINARY)

$(BINARY): $(OBJECTS)

%.o:

clean:
	rm -f $(OBJECTS) $(BINARY)
