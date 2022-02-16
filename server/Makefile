CC=gcc
CFLAGS=-g -Wall
CCLIBS=-lpthread
BINS=server

all: $(BINS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $^ $(CCLIBS)

clean:
	rm -rf *.dSYM $(BINS)