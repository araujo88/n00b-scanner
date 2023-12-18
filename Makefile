CC=gcc
CFLAGS=-g -Wall -Wextra -Wpedantic
CCLIBS=-pthread
BIN=main

all: $(BIN)

%: %.c
	$(CC) $(CFLAGS) -o $@ $^ $(CCLIBS)

clean:
	rm -rf *.dSYM $(BIN)
