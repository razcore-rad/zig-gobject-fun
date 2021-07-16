CFLAGS ?= -Wall -Wextra -pedantic -std=c11 `pkg-config --libs --cflags gtk4`

BINARY = main

default: $(BINARY)

clean:
	rm -f $(BINARY)

.PHONY: default clean
