# NAME:	Brian Be
# EMAIL:	bebrian458@gmail.com
# ID: 	204612203

CC = gcc
CFLAGS = 
DEBUG =
LDLIBS = -lmraa -lm -lpthread
DIST = lab4b.c Makefile README

default: lab4b

lab4b: lab4b.c 
	$(CC) $(LDLIBS) -o $@ $<

check:

bad_cmd:

bad_opt:




clean:
	rm -f lab4b *.tar.gz

dist:
	tar -czf lab4b-204612203.tar.gz $(DIST)

