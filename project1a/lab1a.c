// NAME: Tom Lam
// EMAIL: tlam@hmc.edu
// ID: 40210352
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

#define BUFSIZE 4096
#define ESCAPE_VALUE 4
#define INTERRUPT_VALUE 3

struct termios original_termios, new_termios;

void reset_termios()
{
    if (tcsetattr(0, TCSANOW, &original_termios) == -1) {
        fprintf(stderr, "Resettubg termios settings to default failed with error: %s\n", strerror(errno));
        exit(1);
    }
}

int main(int argc, char* argv[])
{
    int error;
    char* shell_filename = NULL;
    static int debug_flag = 0;

    static struct option long_options[] = {
        { "segfault", no_argument, &debug_flag, 1 },
        { "shell", required_argument, 0, 's' },
        { 0, 0, 0, 0 }
    };

    int c;
    while ((c = getopt_long(argc, argv, "", long_options, NULL)) != -1) {
        switch (c) {
        case 's':
            shell_filename = malloc(strlen(optarg) + 1);
            strcpy(shell_filename, optarg);
            break;
        case '?':
            exit(1);
            fprintf(stderr, "Invalid option specified");
            break;
        }
    }

    // Get default terminal settings and save them, modify and duplicate and apply them
    error = tcgetattr(0, &original_termios);
    if (error == -1) {
        fprintf(stderr, "Getting termios settings failed with error: %s\n", strerror(errno));
        exit(1);
    }

    new_termios = original_termios;
    new_termios.c_iflag = ISTRIP;
    new_termios.c_oflag = 0;
    new_termios.c_lflag = 0;

    error = tcsetattr(0, TCSANOW, &new_termios);
    if (error == -1) {
        fprintf(stderr, "Setting termios settings failed with error: %s\n", strerror(errno));
        exit(1);
    }
    // when exit is called, reset default terminal setttings
    atexit(reset_termios);

    char buf[BUFSIZE];
    ssize_t chars_read;

    // if there is a shell
    if (shell_filename) {
        // With a pipe, the fd in index 1 is source and the fd in index 0 is receiver
        int ptoc_fd[2];
        int ctop_fd[2];

        if (pipe(ptoc_fd) == -1) {
            fprintf(stderr, "Creating parent to child pipe failed with error: %s\n", strerror(errno));
            exit(1);
        }

        if (pipe(ctop_fd) == -1) {
            fprintf(stderr, "Creating child to parent pipe failed with error: %s\n", strerror(errno));
            exit(1);
        }

        pid_t p = fork();

        if (p == -1) {
            fprintf(stderr, "Failed to fork process, error: %s\n", strerror(errno));
            exit(1);
        } else if (p == 0) {
            // Redirect child stdin to receiving end of terminal process parent to child pipe
            close(ptoc_fd[1]); // Child doesn't need the source end of the parent to child pipe
            close(0);
            dup(ptoc_fd[0]);
            close(ptoc_fd[0]);

            // Redirect child stdout to source end of terminal process child to parent pipe
            close(ctop_fd[0]); // Child doesn't need the receiving end of the child to parent pipe
            close(1);
            dup(ctop_fd[1]); // Redirect child stdout
            close(2);
            dup(ctop_fd[1]); // Redirect child stderr
            close(ctop_fd[1]);

            char* arg_list[] = { shell_filename, NULL };
            error = execvp(shell_filename, arg_list);
            if (error == -1) {
                exit(1);
            }
        } else {
            close(ptoc_fd[0]); // Parent doesn't need the receiving end of the parent to child pipe
            close(ctop_fd[1]); // Parent doesn't need the source end of the child to parent pipe

            struct pollfd pollfds[2]; // 0 = keyboard, 1 = child

            pollfds[0].fd = 0;
            pollfds[0].events = POLLIN;
            pollfds[1].fd = ctop_fd[0];
            pollfds[1].events = POLLIN;

            char buf[BUFSIZE];
            ssize_t chars_read;
            int terminal_alive = 1;

            // Continuouslly poll file descriptors to check if we should read from them.
            while (1) {
                if (poll(pollfds, 2, -1) == -1) {
                    fprintf(stderr, "Failed to poll file directories due to error: %s", strerror(errno));
                    exit(1);
                }

                if ((pollfds[0].revents & POLLIN) && terminal_alive) {
                    chars_read = read(0, buf, BUFSIZE);
                    if (chars_read == -1) {
                        fprintf(stderr, "Failed to read from terminal due to error: %s", strerror(errno));
                        exit(1);
                    }

                    for (int i = 0; i < chars_read; i++) {
                        if (buf[i] == ESCAPE_VALUE) {
                            write(1, "^D", 2);
                            terminal_alive = 0;
                            close(ptoc_fd[1]);
                            break;
                        } else if (buf[i] == INTERRUPT_VALUE) {
                            write(1, "^C", 2);
                            kill(p, SIGINT);
                        } else if (buf[i] == '\r' || buf[i] == '\n') {
                            write(1, "\r\n", 2);
                            write(ptoc_fd[1], "\n", 1);
                        } else {
                            write(1, &buf[i], 1);
                            write(ptoc_fd[1], &buf[i], 1);
                        }
                    }
                }

                if (pollfds[1].revents & POLLIN) {
                    chars_read = read(ctop_fd[0], buf, BUFSIZE);
                    if (chars_read <= 0) {
                        fprintf(stderr, "Failed to read from child process due to error: %s", strerror(errno));
                        exit(1);
                    }

                    for (int i = 0; i < chars_read; i++) {
                        if (buf[i] == '\n') {
                            write(1, "\r\n", 2);
                        } else {
                            write(1, &buf[i], 1);
                        }
                    }
                }

                if (pollfds[1].revents & POLLHUP || pollfds[1].revents & POLLERR) {
                    int shell_signal;
                    waitpid(p, &shell_signal, 0);
                    fprintf(stderr, "SHELL EXIT SIGNAL=%d, STATUS=%d\r\n", shell_signal & 0x007f, (shell_signal & 0xff00) >> 0x8);
                    exit(0);
                }
            }
        }
    } else {
        // If no shell is specified, just echo keyboard input
        while (1) {
            chars_read = read(0, buf, BUFSIZE);
            if (chars_read == -1) {
                fprintf(stderr, "Failed to read from terminal due to error: %s", strerror(errno));
                exit(1);
            }

            for (int i = 0; i < chars_read; i++) {
                if (buf[i] == ESCAPE_VALUE) {
                    exit(0);
                } else if (buf[i] == '\r' || buf[i] == '\n') {
                    error = write(1, "\r\n", 2);
                    if (error == -1) {
                        fprintf(stderr, "Failed to write to terminal due to error: %s", strerror(errno));
                        exit(1);
                    }
                } else {
                    error = write(1, &buf[i], 1);
                    if (error == -1) {
                        fprintf(stderr, "Failed to write to terminal due to error: %s", strerror(errno));
                        exit(1);
                    }
                }
            }
        }
    }
    exit(0);
}
