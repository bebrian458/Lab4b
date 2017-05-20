# NAME:	Brian Be
# EMAIL:	bebrian458@gmail.com
# ID: 	204612203

CC = gcc
LFLAGS = -lmraa -lm -lpthread
DEBUG =

default: lab4b

lab4b: lab4b.c 
	$(CC) $(LFLAGS) -o $@ $<

check:

clean:
	rm -f lab4b

dist:

