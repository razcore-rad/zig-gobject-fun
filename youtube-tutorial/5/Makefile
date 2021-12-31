DEBUG=-g
CC=gcc
CFLAGS=-Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -std=c11 \
       $(shell pkg-config --libs --cflags gtk4) \
       $(DEBUG)
BINARY=main
OBJECTS=demo-widget.o demo-resource.o
RESOURCE_FILE = demo.gresource.xml

default: $(BINARY)

$(BINARY): $(OBJECTS)

resources:
	glib-compile-resources --generate-source --target=demo-resource.c \
		$(RESOURCE_FILE)

demo-resource.o: resources
	$(CC) $(CFLAGS) -c -o demo-resource.o demo-resource.c

clean:
	rm -f $(OBJECTS) $(BINARY) demo-resource.c

.PHONY: default resources clean
