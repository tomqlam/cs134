# NAME: Tom Lam
# EMAIL: tlam@hmc.edu
# ID: 40210352
CC = gcc
CFLAGS = -Wall -g -Wextra -pthread
LDFLAGS = -g

SHELL = /bin/sh

all: lab2_add lab2_list

lab2_add: lab2_add.c
	$(CC) $(CFLAGS) -o $@ $<

lab2_list: lab2_list.o SortedList.o
	$(CC) $(CFLAGS) -o $@ lab2_list.o SortedList.o

clean:
	rm -f *tar.gz lab2_add lab2_list *.o

clobber:
	rm -f *tar.gz lab2_add lab2_list *.o *.png *.csv

graphs: 
	gnuplot lab2_add.gp
	gnuplot lab2_list.gp

dist: lab2_add lab2_list tests graphs
	tar czf lab2a-40210352.tar.gz lab2_add.c SortedList.h SortedList.c lab2_list.c lab2_add.csv lab2_list.csv lab2_add-1.png lab2_add-2.png lab2_add-3.png lab2_add-4.png lab2_add-5.png lab2_list-1.png lab2_list-2.png lab2_list-3.png lab2_list-4.png lab2_add.gp lab2_list.gp README Makefile 

tests: tests-list tests-add

tests-list:
	rm -f lab2_list.csv
	touch lab2_list.csv
	-lab2_list --iterations=10 --threads=1 >> lab2_list.csv
	-lab2_list --iterations=100 --threads=1 >> lab2_list.csv
	-lab2_list --iterations=1000 --threads=1 >> lab2_list.csv
	-lab2_list --iterations=10000 --threads=1 >> lab2_list.csv
	-lab2_list --iterations=20000 --threads=1 >> lab2_list.csv
	-lab2_list --iterations=1 --threads=2 >> lab2_list.csv
	-lab2_list --iterations=1 --threads=4 >> lab2_list.csv
	-lab2_list --iterations=1 --threads=8 >> lab2_list.csv
	-lab2_list --iterations=1 --threads=12 >> lab2_list.csv
	-lab2_list --iterations=2 --threads=2 >> lab2_list.csv
	-lab2_list --iterations=2 --threads=4 >> lab2_list.csv
	-lab2_list --iterations=2 --threads=8 >> lab2_list.csv
	-lab2_list --iterations=2 --threads=12 >> lab2_list.csv
	-lab2_list --iterations=4 --threads=2 >> lab2_list.csv
	-lab2_list --iterations=4 --threads=4 >> lab2_list.csv
	-lab2_list --iterations=4 --threads=8 >> lab2_list.csv
	-lab2_list --iterations=4 --threads=12 >> lab2_list.csv
	-lab2_list --iterations=8 --threads=2 >> lab2_list.csv
	-lab2_list --iterations=8 --threads=4 >> lab2_list.csv
	-lab2_list --iterations=8 --threads=8 >> lab2_list.csv
	-lab2_list --iterations=8 --threads=12 >> lab2_list.csv
	-lab2_list --iterations=16 --threads=2 >> lab2_list.csv
	-lab2_list --iterations=16 --threads=4 >> lab2_list.csv
	-lab2_list --iterations=16 --threads=8 >> lab2_list.csv
	-lab2_list --iterations=16 --threads=12 >> lab2_list.csv
	-lab2_list --iterations=32 --threads=2 >> lab2_list.csv
	-lab2_list --iterations=32 --threads=4 >> lab2_list.csv
	-lab2_list --iterations=32 --threads=8 >> lab2_list.csv
	-lab2_list --iterations=32 --threads=12 >> lab2_list.csv
	-lab2_list --yield=i --iterations=1 --threads=2 >> lab2_list.csv
	-lab2_list --yield=i --iterations=1 --threads=4 >> lab2_list.csv
	-lab2_list --yield=i --iterations=1 --threads=8 >> lab2_list.csv
	-lab2_list --yield=i --iterations=1 --threads=12 >> lab2_list.csv
	-lab2_list --yield=i --iterations=2 --threads=2 >> lab2_list.csv
	-lab2_list --yield=i --iterations=2 --threads=4 >> lab2_list.csv
	-lab2_list --yield=i --iterations=2 --threads=8 >> lab2_list.csv
	-lab2_list --yield=i --iterations=2 --threads=12 >> lab2_list.csv
	-lab2_list --yield=i --iterations=4 --threads=2 >> lab2_list.csv
	-lab2_list --yield=i --iterations=4 --threads=4 >> lab2_list.csv
	-lab2_list --yield=i --iterations=4 --threads=8 >> lab2_list.csv
	-lab2_list --yield=i --iterations=4 --threads=12 >> lab2_list.csv
	-lab2_list --yield=i --iterations=8 --threads=2 >> lab2_list.csv
	-lab2_list --yield=i --iterations=8 --threads=4 >> lab2_list.csv
	-lab2_list --yield=i --iterations=8 --threads=8 >> lab2_list.csv
	-lab2_list --yield=i --iterations=8 --threads=12 >> lab2_list.csv
	-lab2_list --yield=i --iterations=16 --threads=2 >> lab2_list.csv
	-lab2_list --yield=i --iterations=16 --threads=4 >> lab2_list.csv
	-lab2_list --yield=i --iterations=16 --threads=8 >> lab2_list.csv
	-lab2_list --yield=i --iterations=16 --threads=12 >> lab2_list.csv
	-lab2_list --yield=i --iterations=32 --threads=2 >> lab2_list.csv
	-lab2_list --yield=i --iterations=32 --threads=4 >> lab2_list.csv
	-lab2_list --yield=i --iterations=32 --threads=8 >> lab2_list.csv
	-lab2_list --yield=i --iterations=32 --threads=12 >> lab2_list.csv
	-lab2_list --yield=d --iterations=1 --threads=2 >> lab2_list.csv
	-lab2_list --yield=d --iterations=1 --threads=4 >> lab2_list.csv
	-lab2_list --yield=d --iterations=1 --threads=8 >> lab2_list.csv
	-lab2_list --yield=d --iterations=1 --threads=12 >> lab2_list.csv
	-lab2_list --yield=d --iterations=2 --threads=2 >> lab2_list.csv
	-lab2_list --yield=d --iterations=2 --threads=4 >> lab2_list.csv
	-lab2_list --yield=d --iterations=2 --threads=8 >> lab2_list.csv
	-lab2_list --yield=d --iterations=2 --threads=12 >> lab2_list.csv
	-lab2_list --yield=d --iterations=4 --threads=2 >> lab2_list.csv
	-lab2_list --yield=d --iterations=4 --threads=4 >> lab2_list.csv
	-lab2_list --yield=d --iterations=4 --threads=8 >> lab2_list.csv
	-lab2_list --yield=d --iterations=4 --threads=12 >> lab2_list.csv
	-lab2_list --yield=d --iterations=8 --threads=2 >> lab2_list.csv
	-lab2_list --yield=d --iterations=8 --threads=4 >> lab2_list.csv
	-lab2_list --yield=d --iterations=8 --threads=8 >> lab2_list.csv
	-lab2_list --yield=d --iterations=8 --threads=12 >> lab2_list.csv
	-lab2_list --yield=d --iterations=16 --threads=2 >> lab2_list.csv
	-lab2_list --yield=d --iterations=16 --threads=4 >> lab2_list.csv
	-lab2_list --yield=d --iterations=16 --threads=8 >> lab2_list.csv
	-lab2_list --yield=d --iterations=16 --threads=12 >> lab2_list.csv
	-lab2_list --yield=d --iterations=32 --threads=2 >> lab2_list.csv
	-lab2_list --yield=d --iterations=32 --threads=4 >> lab2_list.csv
	-lab2_list --yield=d --iterations=32 --threads=8 >> lab2_list.csv
	-lab2_list --yield=d --iterations=32 --threads=12 >> lab2_list.csv
	-lab2_list --yield=il --iterations=1 --threads=2 >> lab2_list.csv
	-lab2_list --yield=il --iterations=1 --threads=4 >> lab2_list.csv
	-lab2_list --yield=il --iterations=1 --threads=8 >> lab2_list.csv
	-lab2_list --yield=il --iterations=1 --threads=12 >> lab2_list.csv
	-lab2_list --yield=il --iterations=2 --threads=2 >> lab2_list.csv
	-lab2_list --yield=il --iterations=2 --threads=4 >> lab2_list.csv
	-lab2_list --yield=il --iterations=2 --threads=8 >> lab2_list.csv
	-lab2_list --yield=il --iterations=2 --threads=12 >> lab2_list.csv
	-lab2_list --yield=il --iterations=4 --threads=2 >> lab2_list.csv
	-lab2_list --yield=il --iterations=4 --threads=4 >> lab2_list.csv
	-lab2_list --yield=il --iterations=4 --threads=8 >> lab2_list.csv
	-lab2_list --yield=il --iterations=4 --threads=12 >> lab2_list.csv
	-lab2_list --yield=il --iterations=8 --threads=2 >> lab2_list.csv
	-lab2_list --yield=il --iterations=8 --threads=4 >> lab2_list.csv
	-lab2_list --yield=il --iterations=8 --threads=8 >> lab2_list.csv
	-lab2_list --yield=il --iterations=8 --threads=12 >> lab2_list.csv
	-lab2_list --yield=il --iterations=16 --threads=2 >> lab2_list.csv
	-lab2_list --yield=il --iterations=16 --threads=4 >> lab2_list.csv
	-lab2_list --yield=il --iterations=16 --threads=8 >> lab2_list.csv
	-lab2_list --yield=il --iterations=16 --threads=12 >> lab2_list.csv
	-lab2_list --yield=il --iterations=32 --threads=2 >> lab2_list.csv
	-lab2_list --yield=il --iterations=32 --threads=4 >> lab2_list.csv
	-lab2_list --yield=il --iterations=32 --threads=8 >> lab2_list.csv
	-lab2_list --yield=il --iterations=32 --threads=12 >> lab2_list.csv
	-lab2_list --yield=dl --iterations=1 --threads=2 >> lab2_list.csv
	-lab2_list --yield=dl --iterations=1 --threads=4 >> lab2_list.csv
	-lab2_list --yield=dl --iterations=1 --threads=8 >> lab2_list.csv
	-lab2_list --yield=dl --iterations=1 --threads=12 >> lab2_list.csv
	-lab2_list --yield=dl --iterations=2 --threads=2 >> lab2_list.csv
	-lab2_list --yield=dl --iterations=2 --threads=4 >> lab2_list.csv
	-lab2_list --yield=dl --iterations=2 --threads=8 >> lab2_list.csv
	-lab2_list --yield=dl --iterations=2 --threads=12 >> lab2_list.csv
	-lab2_list --yield=dl --iterations=4 --threads=2 >> lab2_list.csv
	-lab2_list --yield=dl --iterations=4 --threads=4 >> lab2_list.csv
	-lab2_list --yield=dl --iterations=4 --threads=8 >> lab2_list.csv
	-lab2_list --yield=dl --iterations=4 --threads=12 >> lab2_list.csv
	-lab2_list --yield=dl --iterations=8 --threads=2 >> lab2_list.csv
	-lab2_list --yield=dl --iterations=8 --threads=4 >> lab2_list.csv
	-lab2_list --yield=dl --iterations=8 --threads=8 >> lab2_list.csv
	-lab2_list --yield=dl --iterations=8 --threads=12 >> lab2_list.csv
	-lab2_list --yield=dl --iterations=16 --threads=2 >> lab2_list.csv
	-lab2_list --yield=dl --iterations=16 --threads=4 >> lab2_list.csv
	-lab2_list --yield=dl --iterations=16 --threads=8 >> lab2_list.csv
	-lab2_list --yield=dl --iterations=16 --threads=12 >> lab2_list.csv
	-lab2_list --yield=dl --iterations=32 --threads=2 >> lab2_list.csv
	-lab2_list --yield=dl --iterations=32 --threads=4 >> lab2_list.csv
	-lab2_list --yield=dl --iterations=32 --threads=8 >> lab2_list.csv
	-lab2_list --yield=dl --iterations=32 --threads=12 >> lab2_list.csv
	-lab2_list --sync=s --iterations=32 --threads=12 >> lab2_list.csv
	-lab2_list --sync=m --iterations=32 --threads=12 >> lab2_list.csv
	-lab2_list --yield=i --sync=s --iterations=32 --threads=12 >> lab2_list.csv
	-lab2_list --yield=i --sync=m --iterations=32 --threads=12 >> lab2_list.csv
	-lab2_list --yield=d --sync=s --iterations=32 --threads=12 >> lab2_list.csv
	-lab2_list --yield=d --sync=m --iterations=32 --threads=12 >> lab2_list.csv
	-lab2_list --yield=il --sync=s --iterations=32 --threads=12 >> lab2_list.csv
	-lab2_list --yield=il --sync=m --iterations=32 --threads=12 >> lab2_list.csv
	-lab2_list --yield=dl --sync=s --iterations=32 --threads=12 >> lab2_list.csv
	-lab2_list --yield=dl --sync=m --iterations=32 --threads=12 >> lab2_list.csv
	-lab2_list --sync=s --iterations=1000 --threads=1 >> lab2_list.csv
	-lab2_list --sync=m --iterations=1000 --threads=1 >> lab2_list.csv
	-lab2_list --sync=s --iterations=1000 --threads=2 >> lab2_list.csv
	-lab2_list --sync=m --iterations=1000 --threads=2 >> lab2_list.csv
	-lab2_list --sync=s --iterations=1000 --threads=4 >> lab2_list.csv
	-lab2_list --sync=m --iterations=1000 --threads=4 >> lab2_list.csv
	-lab2_list --sync=s --iterations=1000 --threads=8 >> lab2_list.csv
	-lab2_list --sync=m --iterations=1000 --threads=8 >> lab2_list.csv
	-lab2_list --sync=s --iterations=1000 --threads=12 >> lab2_list.csv
	-lab2_list --sync=m --iterations=1000 --threads=12 >> lab2_list.csv
	-lab2_list --sync=s --iterations=1000 --threads=16 >> lab2_list.csv
	-lab2_list --sync=m --iterations=1000 --threads=16 >> lab2_list.csv
	-lab2_list --sync=s --iterations=1000 --threads=24 >> lab2_list.csv
	-lab2_list --sync=m --iterations=1000 --threads=24 >> lab2_list.csv
	-lab2_list --sync=s --iterations=1000 --threads=32 >> lab2_list.csv
	-lab2_list --sync=m --iterations=1000 --threads=32 >> lab2_list.csv
	-lab2_list --sync=s --iterations=1000 --threads=64 >> lab2_list.csv
	-lab2_list --sync=m --iterations=1000 --threads=64 >> lab2_list.csv

	
tests-add:
	rm -f lab2_add.csv
	touch lab2_add.csv
	lab2_add --iterations=10 --threads=1 >> lab2_add.csv
	lab2_add --iterations=10 --threads=2 >> lab2_add.csv
	lab2_add --iterations=10 --threads=4 >> lab2_add.csv
	lab2_add --iterations=10 --threads=8 >> lab2_add.csv
	lab2_add --iterations=10 --threads=12 >> lab2_add.csv
	lab2_add --iterations=20 --threads=1 >> lab2_add.csv
	lab2_add --iterations=20 --threads=2 >> lab2_add.csv
	lab2_add --iterations=20 --threads=4 >> lab2_add.csv
	lab2_add --iterations=20 --threads=8 >> lab2_add.csv
	lab2_add --iterations=20 --threads=12 >> lab2_add.csv
	lab2_add --iterations=40 --threads=1 >> lab2_add.csv
	lab2_add --iterations=40 --threads=2 >> lab2_add.csv
	lab2_add --iterations=40 --threads=4 >> lab2_add.csv
	lab2_add --iterations=40 --threads=8 >> lab2_add.csv
	lab2_add --iterations=40 --threads=12 >> lab2_add.csv
	lab2_add --iterations=80 --threads=1 >> lab2_add.csv
	lab2_add --iterations=80 --threads=2 >> lab2_add.csv
	lab2_add --iterations=80 --threads=4 >> lab2_add.csv
	lab2_add --iterations=80 --threads=8 >> lab2_add.csv
	lab2_add --iterations=80 --threads=12 >> lab2_add.csv
	lab2_add --iterations=100 --threads=1 >> lab2_add.csv
	lab2_add --iterations=100 --threads=2 >> lab2_add.csv
	lab2_add --iterations=100 --threads=4 >> lab2_add.csv
	lab2_add --iterations=100 --threads=8 >> lab2_add.csv
	lab2_add --iterations=100 --threads=12 >> lab2_add.csv
	lab2_add --iterations=1000 --threads=1 >> lab2_add.csv
	lab2_add --iterations=1000 --threads=2 >> lab2_add.csv
	lab2_add --iterations=1000 --threads=4 >> lab2_add.csv
	lab2_add --iterations=1000 --threads=8 >> lab2_add.csv
	lab2_add --iterations=1000 --threads=12 >> lab2_add.csv
	lab2_add --iterations=10000 --threads=1 >> lab2_add.csv
	lab2_add --iterations=10000 --threads=2 >> lab2_add.csv
	lab2_add --iterations=10000 --threads=4 >> lab2_add.csv
	lab2_add --iterations=10000 --threads=8 >> lab2_add.csv
	lab2_add --iterations=10000 --threads=12 >> lab2_add.csv
	lab2_add --iterations=100000 --threads=1 >> lab2_add.csv
	lab2_add --iterations=100000 --threads=2 >> lab2_add.csv
	lab2_add --iterations=100000 --threads=4 >> lab2_add.csv
	lab2_add --iterations=100000 --threads=8 >> lab2_add.csv
	lab2_add --iterations=100000 --threads=12 >> lab2_add.csv
	lab2_add --yield --iterations=10 --threads=1 >> lab2_add.csv
	lab2_add --yield --iterations=10 --threads=2 >> lab2_add.csv
	lab2_add --yield --iterations=10 --threads=4 >> lab2_add.csv
	lab2_add --yield --iterations=10 --threads=8 >> lab2_add.csv
	lab2_add --yield --iterations=10 --threads=12 >> lab2_add.csv
	lab2_add --yield --iterations=20 --threads=1 >> lab2_add.csv
	lab2_add --yield --iterations=20 --threads=2 >> lab2_add.csv
	lab2_add --yield --iterations=20 --threads=4 >> lab2_add.csv
	lab2_add --yield --iterations=20 --threads=8 >> lab2_add.csv
	lab2_add --yield --iterations=20 --threads=12 >> lab2_add.csv
	lab2_add --yield --iterations=40 --threads=1 >> lab2_add.csv
	lab2_add --yield --iterations=40 --threads=2 >> lab2_add.csv
	lab2_add --yield --iterations=40 --threads=4 >> lab2_add.csv
	lab2_add --yield --iterations=40 --threads=8 >> lab2_add.csv
	lab2_add --yield --iterations=40 --threads=12 >> lab2_add.csv
	lab2_add --yield --iterations=80 --threads=1 >> lab2_add.csv
	lab2_add --yield --iterations=80 --threads=2 >> lab2_add.csv
	lab2_add --yield --iterations=80 --threads=4 >> lab2_add.csv
	lab2_add --yield --iterations=80 --threads=8 >> lab2_add.csv
	lab2_add --yield --iterations=80 --threads=12 >> lab2_add.csv
	lab2_add --yield --iterations=100 --threads=1 >> lab2_add.csv
	lab2_add --yield --iterations=100 --threads=2 >> lab2_add.csv
	lab2_add --yield --iterations=100 --threads=4 >> lab2_add.csv
	lab2_add --yield --iterations=100 --threads=8 >> lab2_add.csv
	lab2_add --yield --iterations=100 --threads=12 >> lab2_add.csv
	lab2_add --yield --iterations=1000 --threads=1 >> lab2_add.csv
	lab2_add --yield --iterations=1000 --threads=2 >> lab2_add.csv
	lab2_add --yield --iterations=1000 --threads=4 >> lab2_add.csv
	lab2_add --yield --iterations=1000 --threads=8 >> lab2_add.csv
	lab2_add --yield --iterations=1000 --threads=12 >> lab2_add.csv
	lab2_add --yield --iterations=10000 --threads=1 >> lab2_add.csv
	lab2_add --yield --iterations=10000 --threads=2 >> lab2_add.csv
	lab2_add --yield --iterations=10000 --threads=4 >> lab2_add.csv
	lab2_add --yield --iterations=10000 --threads=8 >> lab2_add.csv
	lab2_add --yield --iterations=10000 --threads=12 >> lab2_add.csv
	lab2_add --yield --iterations=100000 --threads=1 >> lab2_add.csv
	lab2_add --yield --iterations=100000 --threads=2 >> lab2_add.csv
	lab2_add --yield --iterations=100000 --threads=4 >> lab2_add.csv
	lab2_add --yield --iterations=100000 --threads=8 >> lab2_add.csv
	lab2_add --yield --iterations=100000 --threads=12 >> lab2_add.csv
	lab2_add --yield --sync=m --iterations=10000 --threads=1 >> lab2_add.csv
	lab2_add --yield --sync=s --iterations=1000 --threads=1 >> lab2_add.csv
	lab2_add --yield --sync=c --iterations=10000 --threads=1 >> lab2_add.csv
	lab2_add --yield --sync=m --iterations=10000 --threads=2 >> lab2_add.csv
	lab2_add --yield --sync=s --iterations=1000 --threads=2 >> lab2_add.csv
	lab2_add --yield --sync=c --iterations=10000 --threads=2 >> lab2_add.csv
	lab2_add --yield --sync=m --iterations=10000 --threads=4 >> lab2_add.csv
	lab2_add --yield --sync=s --iterations=1000 --threads=4 >> lab2_add.csv
	lab2_add --yield --sync=c --iterations=10000 --threads=4 >> lab2_add.csv
	lab2_add --yield --sync=m --iterations=10000 --threads=8 >> lab2_add.csv
	lab2_add --yield --sync=s --iterations=1000 --threads=8 >> lab2_add.csv
	lab2_add --yield --sync=c --iterations=10000 --threads=8 >> lab2_add.csv
	lab2_add --yield --sync=m --iterations=10000 --threads=12 >> lab2_add.csv
	lab2_add --yield --sync=s --iterations=1000 --threads=12 >> lab2_add.csv
	lab2_add --yield --sync=c --iterations=10000 --threads=12 >> lab2_add.csv
	lab2_add --sync=m --iterations=10000 --threads=1 >> lab2_add.csv
	lab2_add --sync=s --iterations=10000 --threads=1 >> lab2_add.csv
	lab2_add --sync=c --iterations=10000 --threads=1 >> lab2_add.csv
	lab2_add --sync=m --iterations=10000 --threads=2 >> lab2_add.csv
	lab2_add --sync=s --iterations=10000 --threads=2 >> lab2_add.csv
	lab2_add --sync=c --iterations=10000 --threads=2 >> lab2_add.csv
	lab2_add --sync=m --iterations=10000 --threads=4 >> lab2_add.csv
	lab2_add --sync=s --iterations=10000 --threads=4 >> lab2_add.csv
	lab2_add --sync=c --iterations=10000 --threads=4 >> lab2_add.csv
	lab2_add --sync=m --iterations=10000 --threads=8 >> lab2_add.csv
	lab2_add --sync=s --iterations=10000 --threads=8 >> lab2_add.csv
	lab2_add --sync=c --iterations=10000 --threads=8 >> lab2_add.csv
	lab2_add --sync=m --iterations=10000 --threads=12 >> lab2_add.csv
	lab2_add --sync=s --iterations=10000 --threads=12 >> lab2_add.csv
	lab2_add --sync=c --iterations=10000 --threads=12 >> lab2_add.csv
