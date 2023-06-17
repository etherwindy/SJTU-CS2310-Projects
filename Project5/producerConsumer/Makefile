CC=gcc
CFLAGS=-Wall
PTHREADS=-lpthread

all: main.o buffer.o
	$(CC) $(CFLAGS) -o producer-consumer main.o buffer.o $(PTHREADS)

main.o: buffer.c
	$(CC) $(CFLAGS) -c main.c $(PTHREADS)

buffer.o: buffer.c buffer.h
	$(CC) $(CFLAGS) -c buffer.c $(PTHREADS)

clean:
	rm -rf *.o
	rm -rf producer-consumer

