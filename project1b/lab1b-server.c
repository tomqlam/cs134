// NAME: Tom Lam
// EMAIL: tlam@hmc.edu
// ID: 40210352
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <netinet/in.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>
#include <zlib.h>

#define BUFSIZE 262144
#define INTERRUPT_VALUE 3
#define ESCAPE_VALUE 4

int main(int argc, char* argv[]) {
    int error;
    char* shell_filename = NULL;
    int port_number = -1;

    int sockfd, newsockfd;

    struct sockaddr_in serv_addr, cli_addr;
    static int compress_flag = 0;
    z_stream infstream;
    z_stream defstream;

    static struct option long_options[] = {
        {"compress", no_argument, &compress_flag, 1},
        {"shell", required_argument, 0, 's'},
        {"port", required_argument, 0, 'p'},
        {0, 0, 0, 0}};

    int c;
    while ((c = getopt_long(argc, argv, "", long_options, NULL)) != -1) {
        switch (c) {
            case 's':
                shell_filename = malloc(strlen(optarg) + 1);
                strcpy(shell_filename, optarg);
                break;
            case 'p':
                port_number = atoi(optarg);
                break;
            case '?':
                exit(1);
                fprintf(stderr, "Invalid option specified");
                break;
        }
    }

    if (port_number == -1) {
        fprintf(stderr, "Port was not specified!\n");
        exit(1);
    }

    // if compression flag set, initialize inflation and deflation streams
    if (compress_flag) {
        infstream.zalloc = Z_NULL;
        infstream.zfree = Z_NULL;
        infstream.opaque = Z_NULL;
        infstream.avail_in = (uInt)0;
        infstream.next_in = Z_NULL;
        inflateInit(&infstream);

        defstream.zalloc = Z_NULL;
        defstream.zfree = Z_NULL;
        defstream.opaque = Z_NULL;
        deflateInit(&defstream, Z_DEFAULT_COMPRESSION);
    }

    // initialize socket connection
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port_number);

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        fprintf(stderr, "Binding port number failed due to error: %s\n",
                strerror(errno));
        exit(1);
    }

    // wait for client connection
    listen(sockfd, 5);

    socklen_t clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);
    if (newsockfd < 0) {
        fprintf(stderr, "Failed to accept client connection due to error: %s\n",
                strerror(errno));
        exit(1);
    }

    char buf[BUFSIZE];
    char compress_buf[BUFSIZE];
    bzero(buf, BUFSIZE);
    bzero(compress_buf, BUFSIZE);

    int chars_read;

    // if there is a shell
    if (shell_filename) {
        // With a pipe, the fd in index 1 is source and the fd in index 0 is
        // receiver
        int ptoc_fd[2];
        int ctop_fd[2];

        if (pipe(ptoc_fd) == -1) {
            fprintf(stderr,
                    "Creating parent to child pipe failed with error: %s\n",
                    strerror(errno));
            exit(1);
        }

        if (pipe(ctop_fd) == -1) {
            fprintf(stderr,
                    "Creating child to parent pipe failed with error: %s\n",
                    strerror(errno));
            exit(1);
        }

        pid_t p = fork();

        if (p == -1) {
            fprintf(stderr, "Failed to fork process, error: %s\n",
                    strerror(errno));
            exit(1);
        } else if (p == 0) {
            // Redirect child stdin to receiving end of terminal process parent
            // to child pipe
            close(ptoc_fd[1]);  // Child doesn't need the source end of the
                                // parent to child pipe
            close(0);
            dup(ptoc_fd[0]);
            close(ptoc_fd[0]);

            // Redirect child stdout to source end of terminal process child to
            // parent pipe
            close(ctop_fd[0]);  // Child doesn't need the receiving end of the
                                // child to parent pipe
            close(1);
            dup(ctop_fd[1]);  // Redirect child stdout
            close(2);
            dup(ctop_fd[1]);  // Redirect child stderr
            close(ctop_fd[1]);

            char* arg_list[] = {shell_filename, NULL};
            error = execvp(shell_filename, arg_list);
            if (error == -1) {
                exit(1);
            }
        } else {
            close(ptoc_fd[0]);  // Parent doesn't need the receiving end of the
                                // parent to child pipe
            close(ctop_fd[1]);  // Parent doesn't need the source end of the
                                // child to parent pipe

            struct pollfd
                pollfds[2];  // 0 = input from client, 1 = response from child

            pollfds[0].fd = newsockfd;
            pollfds[0].events = POLLIN;
            pollfds[1].fd = ctop_fd[0];
            pollfds[1].events = POLLIN;

            int terminal_alive = 1;

            // Continuously poll file descriptors to check if we should read
            // from them.
            while (1) {
                if (poll(pollfds, 2, -1) == -1) {
                    fprintf(
                        stderr,
                        "Failed to poll file directories due to error: %s\n",
                        strerror(errno));
                    exit(1);
                }

                if ((pollfds[0].revents & POLLIN) && terminal_alive) {
                    int char_count;
                    if (compress_flag) {
                        chars_read = read(newsockfd, compress_buf, BUFSIZE);
                        if (chars_read <= 0) {
                            fprintf(
                                stderr,
                                "Failed to read from socket due to error: %s\n",
                                strerror(errno));
                            terminal_alive = 0;
                            close(ptoc_fd[1]);
                            continue;
                        }

                        infstream.avail_in = (uInt)chars_read;
                        infstream.next_in = (Bytef*)compress_buf;
                        int ret;
                        do {
                            infstream.avail_out = BUFSIZE;
                            infstream.next_out = (Bytef*)buf;
                            ret = inflate(&infstream, Z_NO_FLUSH);

                            assert(ret !=
                                   Z_STREAM_ERROR); /* state not clobbered */

                            char_count = BUFSIZE - infstream.avail_out;
                        } while (infstream.avail_in > 0);
                    } else {
                        chars_read = read(newsockfd, buf, BUFSIZE);
                        if (chars_read <= 0) {
                            fprintf(
                                stderr,
                                "Failed to read from socket due to error: %s\n",
                                strerror(errno));
                            terminal_alive = 0;
                            close(ptoc_fd[1]);
                            continue;
                        }

                        char_count = chars_read;
                    }

                    // write plaintext buf to shell 
                    for (int i = 0; i < char_count; i++) {
                        if (buf[i] == ESCAPE_VALUE) {
                            terminal_alive = 0;
                            close(ptoc_fd[1]);
                        } else if (buf[i] == INTERRUPT_VALUE) {
                            kill(p, SIGINT);
                        } else if (buf[i] == '\r') {
                            error = write(ptoc_fd[1], "\n", 1);
                            if (error == -1) {
                                fprintf(stderr, "Failed write to shell: %s\n", strerror(errno));
                                exit(1);
                            }
                        } else {
                            error = write(ptoc_fd[1], &buf[i], 1);
                            if (error == -1) {
                                fprintf(stderr, "Failed write to shell: %s\n", strerror(errno));
                                exit(1);
                            }
                        }
                    }
                }

                if (pollfds[1].revents & POLLHUP ||
                    pollfds[1].revents & POLLERR) {
                    int shell_signal;
                    waitpid(p, &shell_signal, 0);
                    fprintf(stderr, "SHELL EXIT SIGNAL=%d, STATUS=%d\r\n",
                            shell_signal & 0x007f,
                            (shell_signal & 0xff00) >> 0x8);
                    close(newsockfd);
                    deflateEnd(&defstream);
                    inflateEnd(&infstream);
                    exit(0);
                }

                if (pollfds[1].revents & POLLIN) {
                    chars_read = read(ctop_fd[0], buf, BUFSIZE);
                    if (chars_read <= 0) {
                        fprintf(stderr,
                                "Failed to read from child process due to "
                                "error: %s\n",
                                strerror(errno));
                        exit(1);
                    }

                    if (compress_flag) {
                        defstream.avail_in = (uInt)
                            chars_read;  // size of input, string + terminator
                        defstream.next_in = (Bytef*)buf;  // input char array

                        int ret;
                        int compressed_size;
                        do {
                            defstream.avail_out =
                                (uInt)BUFSIZE;  // size of output
                            defstream.next_out =
                                (Bytef*)compress_buf;  // output char array

                            ret = deflate(&defstream, Z_SYNC_FLUSH);

                            assert(ret !=
                                   Z_STREAM_ERROR); /* state not clobbered */

                            compressed_size = BUFSIZE - defstream.avail_out;
                        } while (defstream.avail_in > 0);

                        error = write(newsockfd, compress_buf, compressed_size);
                        if (error == -1) {
                            fprintf(stderr, "Failed write to client through socket: %s\n", strerror(errno));
                            exit(1);
                        }
                    } else {
                        error = write(newsockfd, buf, chars_read);
                        if (error == -1) {
                            fprintf(stderr, "Failed write to client through socket: %s\n", strerror(errno));
                            exit(1);
                        }
                    }
                }
            }
        }
    } else {
        // If no shell is specified, just echo keyboard input
        while (1) {
            chars_read = read(newsockfd, buf, BUFSIZE);
            if (chars_read == -1) {
                fprintf(stderr,
                        "Failed to read from terminal due to error: %s\n",
                        strerror(errno));
                exit(1);
            }

            for (int i = 0; i < chars_read; i++) {
                if (buf[i] == ESCAPE_VALUE || buf[i] == INTERRUPT_VALUE) {
                    exit(0);
                } else if (buf[i] == '\n' || buf[i] == '\r') {
                    error = write(1, "\r\n", 2);
                    if (error == -1) {
                        fprintf(
                            stderr,
                            "Failed to write to terminal due to error: %s\n",
                            strerror(errno));
                        exit(1);
                    }
                } else {
                    error = write(1, &buf[i], 1);
                    if (error == -1) {
                        fprintf(
                            stderr,
                            "Failed to write to terminal due to error: %s\n",
                            strerror(errno));
                        exit(1);
                    }
                }
            }
        }
    }
    exit(0);
}
