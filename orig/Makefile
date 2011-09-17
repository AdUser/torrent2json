CFLAGS = -Wall -O2 -pedantic

all: torread torwrite

torread: torread.c
	gcc $(CFLAGS) torread.c -o torread

torwrite: torwrite.c
	gcc $(CFLAFS) torwrite.c -o torwrite

clean:
	rm -f *.o
	rm -f torread
	rm -f torwrite

# vim:noet:ts=4:ai
