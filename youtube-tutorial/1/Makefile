DEBUG=-g
CC=gcc
CFLAGS=-Wall -Wextra -std=c11 -pedantic \
       $(shell pkg-config --libs --cflags gtk4) \
       $(DEBUG)
BINARY=main
OBJECTS=demowidget.o

default: $(BINARY)

$(BINARY): $(OBJECTS)

%.o:

clean:
	rm -f $(OBJECTS) $(BINARY)
