CC = gcc
CFLAGS = -I../src -I./common -I../external/tiny-regex-c -Wall -Wextra -std=c99
LDFLAGS = -lraylib -lraygui -lm

SOURCES = $(shell find . -type f -name "*.c" ! -path "./common/*")
TARGETS = $(patsubst %.c,%.out,$(SOURCES))

SOURCES_DEP = ../external/tiny-regex-c/re.c

all: $(TARGETS)

%.out: %.c
	$(CC) $(CFLAGS) -o $@ $(SOURCES_DEP) $< $(LDFLAGS)

clean:
	rm -f $(TARGETS)

.PHONY: all clean

