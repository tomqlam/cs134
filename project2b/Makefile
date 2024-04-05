# NAME: Tom Lam
# EMAIL: tlam@hmc.edu
# ID: 40210352
CC = gcc
CFLAGS = -Wall -g -Wextra -pthread
LDFLAGS = -g

SHELL = /bin/sh

default: lab2_list

lab2_list: lab2_list.o SortedList.o
	$(CC) $(CFLAGS) -o $@ lab2_list.o SortedList.o

clean:
	rm -f *tar.gz lab2_list *.o

clobber:
	rm -f *tar.gz lab2_list *.o *.png *.csv *.out

graphs:
	gnuplot lab2_list.gp

dist: lab2_list tests graphs
	tar czf lab2b-40210352.tar.gz SortedList.h SortedList.c lab2_list.c lab2b_list.csv lab2_list.gp profile.out lab2b_1.png lab2b_2.png lab2b_3.png lab2b_4.png lab2b_5.png README Makefile 

profile: profile.out

profile.out:
	rm -f perf.data perf.data.old
	perf record ./lab2_list --threads=12 --iterations=1000 --sync=s
	perf report > $@
	perf annotate >> $@
	rm -f perf.data

tests:
	rm -f lab2b_list.csv
	touch lab2b_list.csv
	-lab2_list --sync=s --iterations=1000 --threads=1 >> lab2b_list.csv
	-lab2_list --sync=s --iterations=1000 --threads=2 >> lab2b_list.csv
	-lab2_list --sync=s --iterations=1000 --threads=4 >> lab2b_list.csv
	-lab2_list --sync=s --iterations=1000 --threads=8 >> lab2b_list.csv
	-lab2_list --sync=s --iterations=1000 --threads=12 >> lab2b_list.csv
	-lab2_list --sync=s --iterations=1000 --threads=16 >> lab2b_list.csv
	-lab2_list --sync=s --iterations=1000 --threads=24 >> lab2b_list.csv
	-lab2_list --sync=m --iterations=1000 --threads=1 >> lab2b_list.csv
	-lab2_list --sync=m --iterations=1000 --threads=2 >> lab2b_list.csv
	-lab2_list --sync=m --iterations=1000 --threads=4 >> lab2b_list.csv
	-lab2_list --sync=m --iterations=1000 --threads=8 >> lab2b_list.csv
	-lab2_list --sync=m --iterations=1000 --threads=12 >> lab2b_list.csv
	-lab2_list --sync=m --iterations=1000 --threads=16 >> lab2b_list.csv
	-lab2_list --sync=m --iterations=1000 --threads=24 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --iterations=10 --threads=1 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --iterations=50 --threads=1 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --iterations=100 --threads=1 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --iterations=500 --threads=1 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --iterations=1000 --threads=1 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --iterations=10 --threads=4 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --iterations=50 --threads=4 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --iterations=100 --threads=4 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --iterations=500 --threads=4 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --iterations=1000 --threads=4 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --iterations=10 --threads=8 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --iterations=50 --threads=8 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --iterations=100 --threads=8 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --iterations=500 --threads=8 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --iterations=1000 --threads=8 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --iterations=10 --threads=12 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --iterations=50 --threads=12 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --iterations=100 --threads=12 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --iterations=500 --threads=12 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --iterations=1000 --threads=12 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --iterations=10 --threads=16 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --iterations=50 --threads=16 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --iterations=100 --threads=16 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --iterations=500 --threads=16 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --iterations=1000 --threads=16 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=s --iterations=30 --threads=1 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=s --iterations=150 --threads=1 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=s --iterations=300 --threads=1 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=s --iterations=1500 --threads=1 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=s --iterations=3000 --threads=1 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=s --iterations=30 --threads=4 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=s --iterations=150 --threads=4 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=s --iterations=300 --threads=4 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=s --iterations=1500 --threads=4 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=s --iterations=3000 --threads=4 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=s --iterations=30 --threads=8 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=s --iterations=150 --threads=8 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=s --iterations=300 --threads=8 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=s --iterations=1500 --threads=8 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=s --iterations=3000 --threads=8 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=s --iterations=30 --threads=12 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=s --iterations=150 --threads=12 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=s --iterations=300 --threads=12 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=s --iterations=1500 --threads=12 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=s --iterations=3000 --threads=12 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=s --iterations=30 --threads=16 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=s --iterations=150 --threads=16 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=s --iterations=300 --threads=16 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=s --iterations=1500 --threads=16 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=s --iterations=3000 --threads=16 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=m --iterations=30 --threads=1 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=m --iterations=150 --threads=1 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=m --iterations=300 --threads=1 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=m --iterations=1500 --threads=1 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=m --iterations=3000 --threads=1 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=m --iterations=30 --threads=4 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=m --iterations=150 --threads=4 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=m --iterations=300 --threads=4 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=m --iterations=1500 --threads=4 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=m --iterations=3000 --threads=4 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=m --iterations=30 --threads=8 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=m --iterations=150 --threads=8 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=m --iterations=300 --threads=8 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=m --iterations=1500 --threads=8 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=m --iterations=3000 --threads=8 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=m --iterations=30 --threads=12 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=m --iterations=150 --threads=12 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=m --iterations=300 --threads=12 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=m --iterations=1500 --threads=12 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=m --iterations=3000 --threads=12 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=m --iterations=30 --threads=16 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=m --iterations=150 --threads=16 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=m --iterations=300 --threads=16 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=m --iterations=1500 --threads=16 >> lab2b_list.csv
	-lab2_list --yield=id --lists=4 --sync=m --iterations=3000 --threads=16 >> lab2b_list.csv
	-lab2_list --sync=s --lists=4 --iterations=1000 --threads=1 >> lab2b_list.csv
	-lab2_list --sync=s --lists=8 --iterations=1000 --threads=1 >> lab2b_list.csv
	-lab2_list --sync=s --lists=16 --iterations=1000 --threads=1 >> lab2b_list.csv
	-lab2_list --sync=s --lists=4 --iterations=1000 --threads=2 >> lab2b_list.csv
	-lab2_list --sync=s --lists=8 --iterations=1000 --threads=2 >> lab2b_list.csv
	-lab2_list --sync=s --lists=16 --iterations=1000 --threads=2 >> lab2b_list.csv
	-lab2_list --sync=s --lists=4 --iterations=1000 --threads=4 >> lab2b_list.csv
	-lab2_list --sync=s --lists=8 --iterations=1000 --threads=4 >> lab2b_list.csv
	-lab2_list --sync=s --lists=16 --iterations=1000 --threads=4 >> lab2b_list.csv
	-lab2_list --sync=s --lists=4 --iterations=1000 --threads=8 >> lab2b_list.csv
	-lab2_list --sync=s --lists=8 --iterations=1000 --threads=8 >> lab2b_list.csv
	-lab2_list --sync=s --lists=16 --iterations=1000 --threads=8 >> lab2b_list.csv
	-lab2_list --sync=s --lists=4 --iterations=1000 --threads=12 >> lab2b_list.csv
	-lab2_list --sync=s --lists=8 --iterations=1000 --threads=12 >> lab2b_list.csv
	-lab2_list --sync=s --lists=16 --iterations=1000 --threads=12 >> lab2b_list.csv
	-lab2_list --sync=m --lists=4 --iterations=1000 --threads=1 >> lab2b_list.csv
	-lab2_list --sync=m --lists=8 --iterations=1000 --threads=1 >> lab2b_list.csv
	-lab2_list --sync=m --lists=16 --iterations=1000 --threads=1 >> lab2b_list.csv
	-lab2_list --sync=m --lists=4 --iterations=1000 --threads=2 >> lab2b_list.csv
	-lab2_list --sync=m --lists=8 --iterations=1000 --threads=2 >> lab2b_list.csv
	-lab2_list --sync=m --lists=16 --iterations=1000 --threads=2 >> lab2b_list.csv
	-lab2_list --sync=m --lists=4 --iterations=1000 --threads=4 >> lab2b_list.csv
	-lab2_list --sync=m --lists=8 --iterations=1000 --threads=4 >> lab2b_list.csv
	-lab2_list --sync=m --lists=16 --iterations=1000 --threads=4 >> lab2b_list.csv
	-lab2_list --sync=m --lists=4 --iterations=1000 --threads=8 >> lab2b_list.csv
	-lab2_list --sync=m --lists=8 --iterations=1000 --threads=8 >> lab2b_list.csv
	-lab2_list --sync=m --lists=16 --iterations=1000 --threads=8 >> lab2b_list.csv
	-lab2_list --sync=m --lists=4 --iterations=1000 --threads=12 >> lab2b_list.csv
	-lab2_list --sync=m --lists=8 --iterations=1000 --threads=12 >> lab2b_list.csv
	-lab2_list --sync=m --lists=16 --iterations=1000 --threads=12 >> lab2b_list.csv
