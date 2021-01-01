# DEPENDENCIES: glib >= 2.58, gtk3 >= 3.24

DEBUG=-g
CC=gcc
GTK_CFLAGS=$(shell pkg-config --libs --cflags gtk+-3.0)
CFLAGS=-Wall -Wextra -std=c99 -pedantic \
       $(GTK_CFLAGS) \
       $(DEBUG)

BINARY=main
OBJECTS=custom.o

default: ${BINARY}

${BINARY}: ${OBJECTS}

clean:
	rm -f ${BINARY} ${OBJECTS}
