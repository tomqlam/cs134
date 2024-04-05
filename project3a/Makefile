# NAME: Tom Lam, Serena Mao
# EMAIL: tlam@hmc.edu, smao@hmc.edu
# ID: 40210352, 40210158
CC = gcc
CFLAGS = -Wall -g -Wextra -pthread

SHELL = /bin/sh

default: lab3a

lab3a: lab3a.c ext2_fs.h 
	$(CC) $(CFLAGS) -o $@ $< 

clean:
	rm -f *~ *.o *.gz *.txt lab3a

dist:
	tar czf lab3a-40210352.tar.gz lab3a.c ext2_fs.h README Makefile 