DEBUG=-g
CC=gcc
# NOTE: If this doesn't work, try splitting up
# CFLAGS and LDFLAGS as per the Makefile in part 1.
CFLAGS=-Wall -Wextra -Wno-unused-parameter -std=c11 -pedantic \
       $(shell pkg-config --libs --cflags gtk4) \
       $(DEBUG)
BINARY=main
OBJECTS=demowidget.o

default: $(BINARY)

$(BINARY): $(OBJECTS)

clean:
	rm -f $(OBJECTS) $(BINARY)
