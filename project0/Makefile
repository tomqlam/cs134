# NAME: Tom Lam
# EMAIL: tlam@hmc.edu
# ID: 40210352
CC = gcc
CFLAGS = -Wall -g -Wextra
LDFLAGS = -g

SHELL = /bin/sh

all: lab0

lab0: lab0.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f *tar.gz lab0

dist:
	tar czf lab0-40210352.tar.gz lab0.c README Makefile backtrace.png breakpoint.png

check: test_invalid_argument test_stdin_stdout test_input_success test_input_to_output test_input_failure test_output_failure test_segfault test_segfault_catch test_normal_catch

test_invalid_argument:
	@lab0 -z; \
	if [ $$? == 1 ]; then \
		echo "test_invalid_argument (short case): Success"; \
	else \
		echo "test_invalid_argument (short case): Failure"; \
	fi; \
	lab0 --zzz; \
	if [ $$? == 1 ]; then \
		echo "test_invalid_argument (long case): Success"; \
	else \
		echo "test_invalid_argument (long case): Failure"; \
	fi; \
	lab0 --input; \
	if [ $$? == 1 ]; then \
		echo "test_invalid_argument (no arg case): Success"; \
	else \
		echo "test_invalid_argument (no arg case): Failure"; \
	fi;

test_stdin_stdout:
	@lab0 < <(echo "Test Input"); \
	if [ $$? == 0 ]; then \
		echo "test_stdin_stdout: Success"; \
	else \
		echo "test_stdin_stdout: Failure"; \
	fi

test_input_success:
	@touch input.txt ; \
	lab0 --input input.txt ; \
	if [ $$? == 0 ]; then \
		echo "test_input_success (empty case): Success" ; \
	else \
		echo "test_input_success (empty case): Failure" ; \
	fi ; \
	echo "Sample Text" >> input.txt ; \
	lab0 --input input.txt ; \
	if [ $$? == 0 ]; then \
		echo "test_input_success (text case): Success" ; \
	else \
		echo "test_input_success (text case): Failure" ; \
	fi; \
	rm input.txt

test_input_to_output:
	@rm -f input.txt ; \
	rm -f output.txt ; \
	touch input.txt ; \
	echo "Sample Text" >> input.txt ; \
	lab0 --input input.txt --output output.txt ; \
	if [ $$? == 0 ]; then \
		test output.txt ; \
		if [ $$? == 1 ]; then \
			echo "test_input_to_output: Failure | lab0 failed to create output.txt" ; \
		else \
			cmp -s input.txt output.txt ; \
			if [ $$? == 1 ]; then \
				echo "test_input_to_output: Failure | input.txt and output.txt are different" ; \
			else \
				echo "test_input_to_output: Success" ;\
			fi; \
		fi; \
	else \
		echo "test_input_to_output: Failure | lab0 exited with non-0 value" ; \
	fi ; \
	rm -f input.txt ; \
	rm -f output.txt ; \

test_input_failure:
	@rm -f gone.txt ; \
	lab0 --input gone.txt ; \
	if [ $$? == 2 ]; then \
		echo "test_input_failure: Success" ; \
	else \
		echo "test_input_failure: Failure" ; \
	fi

test_output_failure:
	@lab0 --output lab0 ; \
	if [ $$? == 3 ]; then \
		echo "test_output_failure: Success" ; \
	else \
		echo "test_output_failure: Failure" ; \
	fi

test_segfault:
	@lab0 --segfault ; \
	if [ $$? == 139 ]; then \
		echo "test_segfault: Success" ; \
	else \
		echo "test_segfault: Failure" ; \
	fi

test_segfault_catch:
	@lab0 --segfault --catch ; \
	if [ $$? == 4 ]; then \
		echo "test_segfault_catch: Success" ; \
	else \
		echo "test_segfault_catch: Failure" ; \
	fi

test_normal_catch:
	@lab0 --catch < <(echo "Test Input") ; \
	if [ $$? == 0 ]; then \
		echo "test_normal_catch: Success" ; \
	else \
		echo "test_normal_catch: Failure" ; \
	fi