// NAME: Tom Lam
// EMAIL: tlam@hmc.edu
// ID: 40210352
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE 4096

void initiate_segfault();

void signal_handler(int signal_number);

int main(int argc, char *argv[]) {
    // argument parsing code adapted from
    // https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Option-Example.html
    char *input_filename = NULL;
    char *output_filename = NULL;
    static int segfault_flag = 0;
    static int catch_flag = 0;

    static struct option long_options[] = {
        {"segfault", no_argument, &segfault_flag, 1},
        {"catch", no_argument, &catch_flag, 1},
        {"input", required_argument, 0, 'i'},
        {"output", required_argument, 0, 'o'},
        {0, 0, 0, 0}};

    int c;
    while ((c = getopt_long(argc, argv, "", long_options, NULL)) != -1) {
        switch (c) {
            case 'i':
                input_filename = malloc(strlen(optarg) + 1);
                strcpy(input_filename, optarg);
                break;
            case 'o':
                output_filename = malloc(strlen(optarg) + 1);
                strcpy(output_filename, optarg);
                break;
            case '?':
                exit(1);
                break;
        }
    }

    if (input_filename) {
        int input_fd = open(input_filename, O_RDONLY);
        if (input_fd >= 0) {
            close(0);
            dup(input_fd);
            close(input_fd);
        } else {
            fprintf(stderr,
                    "argument --input: Error opening input file \'%s\'. "
                    "Reason: %s.\n",
                    input_filename, strerror(errno));
            exit(2);
        }
    }

    if (output_filename) {
        int output_fd = creat(output_filename, 0666);
        if (output_fd >= 0) {
            close(1);
            dup(output_fd);
            close(output_fd);
        } else {
            fprintf(stderr,
                    "argument --output: Error creating output file \'%s\'. "
                    "Reason: %s.\n",
                    output_filename, strerror(errno));
            exit(3);
        }
    }

    if (catch_flag) {
        signal(SIGSEGV, signal_handler);
    }

    if (segfault_flag) {
        initiate_segfault();
    }

    char input_buf[4096];
    ssize_t chars_read;

    while ((chars_read = read(0, input_buf, BUFSIZE)) > 0) {
        if (write(1, input_buf, chars_read) == -1) {
            fprintf(stderr,
                    "argument --output: Error writing to output file \'%s\'. "
                    "Reason: %s.\n",
                    output_filename, strerror(errno));
            exit(3);
        }
    }

    if (chars_read == -1) {
        fprintf(stderr,
                "argument --input: Error reading input file \'%s\'. "
                "Reason: %s.\n",
                input_filename, strerror(errno));
        exit(2);
    }

    free(input_filename);
    free(output_filename);
    exit(0);
}

void initiate_segfault() {
    char *bad_ptr = NULL;
    bad_ptr[0] = '\0';
}

void signal_handler(int signal_number) {
    fprintf(stderr,
            "signal: %s handled from executable being called with \'--catch\' "
            "argument.\n",
            strsignal(signal_number));
    exit(4);
}