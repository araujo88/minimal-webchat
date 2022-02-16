CC=gcc
CFLAGS=-g -Wall
CCLIBS=-lpthread
BINS=client

all: $(BINS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $^ $(CCLIBS)

clean:
	rm -rf *.dSYM $(BINS)
