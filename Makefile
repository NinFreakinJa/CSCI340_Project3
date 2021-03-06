TARGET=CSCI340_Project3.out
OBJECTS=CSCI340_Project3.c

CC=gcc
CFLAGS=-Wall -g -o
LDFLAGS=-pthread

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

clean:
	rm -f $(TARGET)
	rm -f *.o

depends:
	$(CC) -MM $(OBJECTS:.o=.c) > depends