CC=gcc
CFLAGS=-g -Wall -Wextra -Wpedantic
CCLIBS=-pthread
BINS=n00bscanner

all: $(BINS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $^ $(CCLIBS)

clean:
	rm -rf *.dSYM $(BINS)
