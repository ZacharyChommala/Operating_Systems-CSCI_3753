CC = gcc
CFLAGS = -c -g -Wall -Wextra -std=gnu99
LFLAGS = -Wall -Wextra -pthread

.PHONY: all clean

all: multi-lookup

multi-lookup: multi-lookup.o util.o queue.o
	$(CC) -std=c99 $(LFLAGS) $^ -o $@

queue.o: queue.c queue.h
	$(CC) $(CFLAGS) $<

multi-lookup.o: multi-lookup.c multi-lookup.h
	$(CC) -std=c99 $(CFLAGS) $<

util.o: util.c util.h
	$(CC) $(CFLAGS) $<

clean:
	rm -f multi-lookup queueTest pthread-hello
	rm -f *.o
	rm -f *~
	rm -f results.txt
