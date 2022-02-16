CC=gcc
CFLAGS=-g -Wall
CCLIBS=-lpthread
BINS=websocket-chat

all: $(BINS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $^ $(CCLIBS)

clean:
	rm -rf *.dSYM $(BINS)