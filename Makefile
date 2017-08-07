CC=gcc
CFLAGS=-DDEBUG -g -Wall --std=c99


all: gapbuffer.o tests.o
	$(CC) tests.o gapbuffer.o -o tests $(CFLAGS)

gap-buffer:
	$(CC) -c gapbuffer.c $(CFLAGS)
 
tests:
	$(CC) -c tests.c $(CFLAGS)

clean:
	rm -f tests *.o