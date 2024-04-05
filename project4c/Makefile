# NAME: Tom Lam
# EMAIL: tlam@hmc.edu
# ID: 40210352
CC = gcc
CFLAGS = -Wall -g -Wextra

SHELL = /bin/sh

default: lab4c_tcp lab4c_tls

lab4c_tcp: lab4c_tcp.c
ifeq ($(shell test -d /usr/include/rc && echo -n EMBEDDED),EMBEDDED)
	$(CC) $(CFLAGS) -o $@ $< -lrobotcontrol -lm -lcrypto -lssl
else
	$(CC) -DDUMMY $(CFLAGS) -o $@ $< -lm -lcrypto -lssl
endif

lab4c_tls: lab4c_tls.c
ifeq ($(shell test -d /usr/include/rc && echo -n EMBEDDED),EMBEDDED)
	$(CC) $(CFLAGS) -o $@ $< -lrobotcontrol -lm -lcrypto -lssl
else
	$(CC) -DDUMMY $(CFLAGS) -o $@ $< -lm -lcrypto -lssl
endif

clean:
	rm -f *tar.gz lab4c_tls lab4c_tcp

dist:
	tar czf lab4c-40210352.tar.gz lab4c_tcp.c lab4c_tls.c README Makefile


