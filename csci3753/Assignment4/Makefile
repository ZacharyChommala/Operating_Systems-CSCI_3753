# CS3753 - PA4

CC = gcc
CFLAGS = -c -g -Wall -Wextra
LFLAGS = -g -Wall -Wextra

.PHONY: all clean

all: test-lru test-predict test-api

test-lru: simulator.o pager-lru.o
	$(CC) $(LFLAGS) $^ -o $@

test-predict: simulator.o pager-predict.o
	$(CC) $(LFLAGS) $^ -o $@

pager-lru.o: pager-lru.c simulator.h 
	$(CC) $(CFLAGS) $<

pager-predict.o: pager-predict.c simulator.h 
	$(CC) $(CFLAGS) $<

clean:
	rm -f test-lru test-predict
	rm -f *.o
	rm -f *~
	rm -f *.csv
	rm -f *.pdf
	rm -f handout/*~
	rm -f handout/*.log
	rm -f handout/*.aux
	rm -f handout/*.out
