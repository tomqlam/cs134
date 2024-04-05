// NAME: Tom Lam
// EMAIL: tlam@hmc.edu
// ID: 40210352
#ifdef DUMMY
/*
 * dummy declarations for I/O functions
 *	enable program to be tested on any Linux system
 */

int rc_adc_init() { return 0; }
int rc_adc_read_raw(int ch) {
    (void)(ch);
    return 2602;
}
int rc_adc_cleanup() { return 0; }
#else
#include <rc/adc.h>
#endif

#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <math.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define BUFSIZE 65536

const int B = 4275;     // B value of the thermistor
const int R0 = 100000;  // R0 = 100k
char* log_filename = NULL;
int period_seconds = 1;
char scale = 'F';
int error;
FILE* log_fd;
int should_report = 1;
int port_number = 0;
int id_number = 0;
char * host_name;
int sockfd, port_number;

struct sockaddr_in serv_addr;
struct hostent* server;

int main(int argc, char* argv[]) {
    static struct option long_options[] = {
        {"period", required_argument, 0, 'p'},
        {"scale", required_argument, 0, 's'},
        {"log", required_argument, 0, 'l'},
        {"id", required_argument, 0, 'i'},
        {"host", required_argument, 0, 'h'},
        {0, 0, 0, 0}};

    // argument processing
    int c;
    while ((c = getopt_long(argc, argv, "", long_options, NULL)) != -1) {
        switch (c) {
            case 'p':
                period_seconds = atoi(optarg);
                break;
            case 's':
                if (strlen(optarg) != 1) {
                    fprintf(stderr,
                            "scale option only takes singular character "
                            "arguments!\n");
                    exit(1);
                } else {
                    scale = optarg[0];
                    if (scale != 'C' && scale != 'F') {
                        fprintf(stderr,
                                "scale option only takes C and F as arguments, "
                                "not: %s\n",
                                optarg);
                        exit(1);
                    }
                }
                break;
            case 'l':
                log_filename = malloc(strlen(optarg) + 1);
                strcpy(log_filename, optarg);
                break;
            case 'i':
                id_number = atoi(optarg);
                break;
            case 'h':
                host_name = malloc(strlen(optarg) + 1);
                strcpy(host_name, optarg);
                break;
            case '?':
                exit(1);
                break;
        }
    }

    if (argc - optind != 1) {
        fprintf(stderr, "There should be one non option argument for the port, instead there are %d args", argc - optind - 1);
        exit(1);
    }

    port_number = atoi(argv[optind]);
    
    // creating log file for reporting
    if (log_filename) {
        log_fd = fopen(log_filename, "a+");
        if (log_fd == NULL) {
            fprintf(stderr, "Failed to open logfile: %s\r\n",
                    strerror(errno));
            exit(2);
        }
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        fprintf(stderr, "Failed to open socket due to error: %s\r\n",
                strerror(errno));
        exit(1);
    }

    server = gethostbyname(host_name);
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

    char write_buf[BUFSIZE];
    int chars_to_write = snprintf(write_buf, BUFSIZE, "ID=%d\n", id_number);
    error = write(sockfd, write_buf, chars_to_write);
    if (error == -1) {
        fprintf(stderr, "Failed write to server through socket: %s\n", strerror(errno));
        exit(1);
    }

    // buffer for polling
    char buf[BUFSIZE];
    int buf_len = 0;

    // initialize polling structs
    struct pollfd commands_pollfd;
    commands_pollfd.fd = sockfd;
    commands_pollfd.events = POLLIN;

    // initialize needed time variables
    time_t prev_time, curr_time;
    struct tm* time_info;

    time(&prev_time);
    long long prev_time_seconds, curr_time_seconds;
    prev_time_seconds = prev_time;

    // initialize the temperature sensor
    error = rc_adc_init();
    if (error == -1) {
        fprintf(stderr, "Failed to initialize analog input: %s\r\n",
                strerror(errno));
        exit(2);
    }

    // main program reporting loop
    while (1) {
        time(&curr_time);
        curr_time_seconds = curr_time;

        // only report gather and report temp data if reporting is on and enough time has passed
        if (curr_time_seconds - prev_time_seconds >= period_seconds && should_report) {
            prev_time_seconds = curr_time_seconds;

            int raw_analog = rc_adc_read_raw(0);
            if (raw_analog == -1) {
                fprintf(stderr, "Failed to read temperature input: %s\r\n",
                        strerror(errno));
                exit(2);
            }
            
            // following code from temperature sensor documentation
            double R = 4095.0 / raw_analog - 1.0;
            R = R0 * R;

            double temperature = 1.0 / (log(R / R0) / B + 1 / 298.15) - 273.15;

            if (scale == 'F') {
                temperature = (temperature * 1.8) + 32;
            }

            time_info = localtime(&curr_time);
            chars_to_write = snprintf(write_buf, BUFSIZE, "%02d:%02d:%02d %.1f\n", time_info->tm_hour, time_info->tm_min, time_info->tm_sec, temperature);
            error = write(sockfd, write_buf, chars_to_write);
            if (error == -1) {
                fprintf(stderr, "Failed write to server through socket: %s\n", strerror(errno));
                exit(1);
            }
            // printf("%02d:%02d:%02d %.1f\n", time_info->tm_hour, time_info->tm_min, time_info->tm_sec, temperature);

            if (log_filename) {
                fprintf(log_fd, "%02d:%02d:%02d %.1f\n", time_info->tm_hour, time_info->tm_min, time_info->tm_sec, temperature);
                fflush(log_fd);
            }
        }

        if (poll(&commands_pollfd, 1, 0) == -1) {
            fprintf(stderr,
                    "Failed to poll commands due to error: %s\r\n",
                    strerror(errno));
            exit(1);
        }

        // if poll found stuff to read, process the data
        if (commands_pollfd.revents & POLLIN) {
            // read data into buffer. start at buf + buf_len because buf to buf + buflen - 1 contains leftover data from incomplete commands
            int chars_read = read(sockfd, buf + buf_len, BUFSIZE - buf_len - 1);
            if (chars_read == -1) {
                fprintf(stderr,
                        "Failed to read from STDIN due to error: %s\r\n",
                        strerror(errno));
                exit(1);
            }

            // update buf_len and make character after input null for strtok_r to stop at
            buf_len += chars_read;
            buf[buf_len] = '\0';

            char* full_command;
            char* rest = buf;

            full_command = strtok_r(buf, "\n", &rest);

            // tokenizes buffer with "\n" as the delimiter, check if each command matches any of the predefined commands
            while (full_command != NULL) {
                if (strcmp(full_command, "SCALE=F") == 0) {
                    scale = 'F';
                } else if (strcmp(full_command, "SCALE=C") == 0) {
                    scale = 'C';
                } else if (strncmp(full_command, "PERIOD=", 7) == 0) {
                    period_seconds = atoi(full_command + 7);
                } else if (strcmp(full_command, "STOP") == 0) {
                    if (should_report) {
                        should_report = 0;
                    }
                } else if (strcmp(full_command, "START") == 0) {
                    if (!should_report) {
                        should_report = 1;
                    }
                } else if (strncmp(full_command, "LOG ", 4) == 0) {
                    ;  // placeholder for lab 4C
                } else if (strcmp(full_command, "OFF") == 0) {
                    time(&curr_time);
                    time_info = localtime(&curr_time);
                    
                    chars_to_write = snprintf(write_buf, BUFSIZE, "%02d:%02d:%02d SHUTDOWN\n", time_info->tm_hour, time_info->tm_min, time_info->tm_sec);
                    error = write(sockfd, write_buf, chars_to_write);
                    if (error == -1) {
                        fprintf(stderr, "Failed write to server through socket: %s\n", strerror(errno));
                        exit(1);
                    }
                    // printf("%02d:%02d:%02d SHUTDOWN\n", time_info->tm_hour, time_info->tm_min, time_info->tm_sec);

                    if (log_filename) {
                        fprintf(log_fd, "%s\n", full_command);
                        fprintf(log_fd, "%02d:%02d:%02d SHUTDOWN\n", time_info->tm_hour, time_info->tm_min, time_info->tm_sec);
                        fflush(log_fd);
                    }

                    goto end;
                } else {
                    ;
                    // printf("unrecognized command: %s\n", full_command);
                }
                if (log_filename) {
                    fprintf(log_fd, "%s\n", full_command);
                    fflush(log_fd);
                }
                
                // take next possible command
                full_command = strtok_r(rest, "\n", &rest);
            }

            // after processing all "\n" ending runs, move remaining characters to the beginning, update buf_len to remember this on the next run
            int remaining = buf + buf_len - rest;
            if (remaining > 0) {
                memmove(buf, rest, remaining);
            }
            buf_len = remaining;
        }
    }

end:
    // cleanup everything
    if (log_filename) {
        fflush(log_fd);
        fclose(log_fd);
    }

    rc_adc_cleanup();
    free(log_filename);
    exit(0);
}
