CC = gcc
CFLAGS = -I../src -I./common -Wall -Wextra -std=c99
LDFLAGS = -lraylib -lraygui -lm

SOURCES = $(shell find . -type f -name "*.c" ! -path "./common/*")
TARGETS = $(patsubst %.c,%.out,$(SOURCES))

all: $(TARGETS)

%.out: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f $(TARGETS)

.PHONY: all clean

