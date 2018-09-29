CC=gcc
CFLAGS=-DDEBUG -g -Wall --std=c99
LFLAGS=-lncurses

all: main.o gapbuffer.o tests.o
	$(CC) tests.o gapbuffer.o -o tests
	$(CC) main.o gapbuffer.o -o texteditor $(LFLAGS) 

main:
	$(CC) -c main.c $(CFLAGS)
gap-buffer:
	$(CC) -c gapbuffer.c $(CFLAGS)
 
tests:
	$(CC) -c tests.c $(CFLAGS)

clean:
	rm -f texteditor tests *.o