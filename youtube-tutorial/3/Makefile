DEBUG=-g
CC=gcc
CFLAGS=-Wall -Wextra -Wno-unused-parameter -std=c11 -pedantic \
       $(shell pkg-config --libs --cflags gtk4) \
       $(DEBUG)
BINARY=main
OBJECTS=demowidget.o

default: $(BINARY)

$(BINARY): $(OBJECTS)

clean:
	rm -f $(OBJECTS) $(BINARY)
