CFLAGS=-Wall -g -std=c11

all: memplay

memplay: memplay.o

clean:
	rm -f *.o
	rm -f memplay
	rm -rf *.dSYM
