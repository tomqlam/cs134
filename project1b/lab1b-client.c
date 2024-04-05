// NAME: Tom Lam
// EMAIL: tlam@hmc.edu
// ID: 40210352
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <netdb.h>
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

struct termios original_termios, new_termios;

void reset_termios() {
    if (tcsetattr(0, TCSANOW, &original_termios) == -1) {
        fprintf(stderr,
                "Resetting termios settings to default failed with error: %s\n",
                strerror(errno));
        exit(1);
    }
}

int main(int argc, char* argv[]) {
    int error;

    int sockfd, port_number;

    struct sockaddr_in serv_addr;
    struct hostent* server;
    static int compress_flag = 0;
    char* log_filename = NULL;
    z_stream infstream;
    z_stream defstream;
    FILE* log_fd;

    port_number = -1;

    static struct option long_options[] = {
        {"compress", no_argument, &compress_flag, 1},
        {"log", required_argument, 0, 'l'},
        {"port", required_argument, 0, 'p'},
        {0, 0, 0, 0}};

    int c;
    while ((c = getopt_long(argc, argv, "", long_options, NULL)) != -1) {
        switch (c) {
            case 'l':
                log_filename = malloc(strlen(optarg) + 1);
                strcpy(log_filename, optarg);
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
        fprintf(stderr, "ERROR, port was not specified!\n");
    }

    if (log_filename) {
        log_fd = fopen(log_filename, "w+");
        if (log_fd == NULL) {
            fprintf(stderr,
                    "argument --output: Error creating log file \'%s\'. "
                    "Reason: %s.\n",
                    log_filename, strerror(errno));
            exit(1);
        }
    }

    // initialize inflation and deflation streams
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

    // Get default terminal settings and save them, modify and duplicate and
    // apply them
    error = tcgetattr(0, &original_termios);
    if (error == -1) {
        fprintf(stderr, "Getting termios settings failed with error: %s\n",
                strerror(errno));
        exit(1);
    }

    new_termios = original_termios;
    new_termios.c_iflag = ISTRIP;
    new_termios.c_oflag = 0;
    new_termios.c_lflag = 0;

    error = tcsetattr(0, TCSANOW, &new_termios);
    if (error == -1) {
        fprintf(stderr, "Setting termios settings failed with error: %s\n",
                strerror(errno));
        exit(1);
    }
    // when exit is called, reset default terminal setttings
    atexit(reset_termios);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        fprintf(stderr, "Failed to open socket due to error: %s\r\n",
                strerror(errno));
        exit(1);
    }

    server = gethostbyname("localhost");
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;

    bcopy((char*)server->h_addr, (char*)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(port_number);

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        fprintf(stderr, "Failed to connect socket due to error: %s\r\n",
                strerror(errno));
        exit(1);
    }

    char buf[BUFSIZE];
    char compress_buf[BUFSIZE];
    int terminal_alive = 1;
    int chars_read;

    bzero(buf, BUFSIZE);
    struct pollfd
        pollfds[2];  // 0 = input from keyboard, 1 = response from server

    pollfds[0].fd = 0;
    pollfds[0].events = POLLIN;
    pollfds[1].fd = sockfd;
    pollfds[1].events = POLLIN;

    while (1) {
        if (poll(pollfds, 2, -1) == -1) {
            fprintf(stderr,
                    "Failed to poll file directories due to error: %s\r\n",
                    strerror(errno));
            exit(1);
        }

        if ((pollfds[0].revents & POLLIN) && terminal_alive) {
            chars_read = read(0, buf, BUFSIZE);
            if (chars_read <= 0) {
                terminal_alive = 0;
                close(sockfd);
            } else {
                // echo keyboard input to stdout
                for (int i = 0; i < chars_read; i++) {
                    if (buf[i] == '\r' || buf[i] == '\n') {
                        error = write(1, "\r\n", 2);
                        if (error == -1) {
                            fprintf(stderr, "Failed write to terminal: %s\n", strerror(errno));
                            exit(1);
                        }
                    } else {
                        error = write(1, &buf[i], 1);
                        if (error == -1) {
                            fprintf(stderr, "Failed write to terminal: %s\n", strerror(errno));
                            exit(1);
                        }
                    }
                }

                if (compress_flag) {
                    // do compression on buf and put into compress_buf
                    defstream.avail_in =
                        (uInt)chars_read;  // size of input, string + terminator
                    defstream.next_in = (Bytef*)buf;  // input char array

                    int ret;
                    int compressed_size;
                    do {
                        defstream.avail_out = (uInt)BUFSIZE;  // size of output
                        defstream.next_out =
                            (Bytef*)compress_buf;  // output char array

                        ret = deflate(&defstream, Z_SYNC_FLUSH);

                        assert(ret != Z_STREAM_ERROR); /* state not clobbered */

                        compressed_size = BUFSIZE - defstream.avail_out;
                    } while (defstream.avail_in > 0);

                    error = write(sockfd, compress_buf, compressed_size);
                    if (error == -1) {
                        fprintf(stderr, "Failed write to server through socket: %s\n", strerror(errno));
                        exit(1);
                    }

                    if (log_filename) {
                        fprintf(log_fd, "SENT %d bytes: ", compressed_size);
                        fwrite(compress_buf, 1, compressed_size, log_fd);
                        fwrite("\n", 1, 1, log_fd);
                        fflush(log_fd);
                    }
                } else {
                    error = write(sockfd, buf, chars_read);
                    if (error == -1) {
                        fprintf(stderr, "Failed write to server through socket: %s\n", strerror(errno));
                        exit(1);
                    }

                    if (log_filename) {
                        fprintf(log_fd, "SENT %d bytes: ", chars_read);
                        fwrite(buf, 1, chars_read, log_fd);
                        fwrite("\n", 1, 1, log_fd);
                        fflush(log_fd);
                    }
                }
            }
        }

        if (pollfds[1].revents & POLLHUP || pollfds[1].revents & POLLERR) {
            // cleanup on exit
            close(sockfd);
            deflateEnd(&defstream);
            inflateEnd(&infstream);
            exit(0);
        }

        if (pollfds[1].revents & POLLIN) {
            int char_count;
            if (compress_flag) {
                chars_read = read(sockfd, compress_buf, BUFSIZE);
                if (chars_read <= 0) {
                    if (chars_read == 0) {
                        // cleanup if no more to read
                        close(sockfd);
                        deflateEnd(&defstream);
                        inflateEnd(&infstream);
                        exit(0);
                    } else {
                        fprintf(
                            stderr,
                            "Failed to read from socket due to error: %s\r\n",
                            strerror(errno));
                        exit(1);
                    }
                }

                if (log_filename) {
                    fprintf(log_fd, "RECEIVED %d bytes: ", chars_read);
                    fwrite(compress_buf, 1, chars_read, log_fd);
                    fwrite("\n", 1, 1, log_fd);
                    fflush(log_fd);
                }

                // uncompress buffer into buf
                infstream.avail_in = (uInt)chars_read;
                infstream.next_in = (Bytef*)compress_buf;
                int ret;
                do {
                    infstream.avail_out = (uInt)BUFSIZE;
                    infstream.next_out = (Bytef*)buf;

                    ret = inflate(&infstream, Z_NO_FLUSH);

                    assert(ret != Z_STREAM_ERROR);

                    char_count = BUFSIZE - infstream.avail_out;
                } while (infstream.avail_in > 0);
            } else {
                chars_read = read(sockfd, buf, BUFSIZE);
                if (chars_read <= 0) {
                    fprintf(stderr,
                            "Failed to read from socket due to error: %s\r\n",
                            strerror(errno));
                    exit(1);
                }

                if (log_filename) {
                    fprintf(log_fd, "RECEIVED %d bytes: ", chars_read);
                    fwrite(buf, 1, chars_read, log_fd);
                    fwrite("\n", 1, 1, log_fd);
                    fflush(log_fd);
                }

                char_count = chars_read;
            }

            // write buffer received through socket from server to client stdout
            for (int i = 0; i < char_count; i++) {
                if (buf[i] == '\n') {
                    error = write(1, "\r\n", 2);
                    if (error == -1) {
                        fprintf(stderr, "Failed write to received data to terminal: %s\n", strerror(errno));
                        exit(1);
                    }
                } else {
                    error = write(1, &buf[i], 1);
                    if (error == -1) {
                        fprintf(stderr, "Failed write to received data to terminal: %s\n", strerror(errno));
                        exit(1);
                    }
                }
            }
        }
    }
    exit(0);
}
