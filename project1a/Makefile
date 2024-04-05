# NAME: Tom Lam
# EMAIL: tlam@hmc.edu
# ID: 40210352
CC = gcc
CFLAGS = -Wall -g -Wextra
LDFLAGS = -g

SHELL = /bin/sh

all: lab1a

lab1a: lab1a.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f *tar.gz lab1a

dist:
	tar czf lab1a-40210352.tar.gz lab1a.c README Makefile
