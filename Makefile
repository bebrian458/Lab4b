# NAME:	Brian Be
# EMAIL:	bebrian458@gmail.com
# ID: 	204612203

CC = gcc
LFLAGS = -lmraa -lm -lpthread
DEBUG =
DIST = lab4b.c Makefile README

default: lab4b

lab4b: lab4b.c 
	$(CC) $(LFLAGS) -o $@ $<

check:

clean:
	rm -f lab4b

dist:
	tar -czf lab4b-204612203.tar.gz $(DIST)

