PGMS=CSCI340_Project3 queue

CC=gcc
CFLAGS=-Wall -pthread

all: $(PGMS)

clean:
	rm -f $(PGMS)
	rm -f *.o