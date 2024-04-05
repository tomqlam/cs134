# NAME: Tom Lam
# EMAIL: tlam@hmc.edu
# ID: 40210352
CC = gcc
CFLAGS = -Wall -g -Wextra

SHELL = /bin/sh

default: lab4b

lab4b: lab4b.c
ifeq ($(shell test -d /usr/include/rc && echo -n EMBEDDED),EMBEDDED)
	$(CC) $(CFLAGS) -o $@ $< -lrobotcontrol -lm
else
	$(CC) -DDUMMY $(CFLAGS) -o $@ $< -lm
endif

clean:
	rm -f *tar.gz lab4b

dist:
	tar czf lab4b-40210352.tar.gz lab4b.c README Makefile

check: 
	@{ sleep 2 && printf "OFF\n"; } | ./lab4b --scale=F --period=1 --log=log.txt ; \
	if [ $$? == 0 ]; then \
		echo "smoke_test: Success" ; \
	else \
		echo "smoke_test (empty case): Failure" ; \
	fi ; \
	rm -f log.txt

