# NAME: Tom Lam, Serena Mao
# EMAIL: tlam@hmc.edu, smao@hmc.edu
# ID: 40210352, 40210158

default: lab3b

lab3b: 
	rm -f lab3b
	touch lab3b
	echo "python3.10 lab3b.py \$$1" > lab3b
	chmod +x lab3b

clean:
	rm -f *~ *.o *.gz *.txt lab3b

dist:
	tar czf lab3b-40210352.tar.gz lab3b.py README Makefile 