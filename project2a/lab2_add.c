// NAME: Tom Lam
// EMAIL: tlam@hmc.edu
// ID: 40210352
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define BILLION 1000000000;

// initialize all globals
long long counter = 0;
int threads = 1;
int iterations = 1;
int error;
char sync_opt[2] = "n";
static int yield_flag = 0;
pthread_mutex_t lock;
volatile int exclusion = 0;

void add(long long *pointer, long long value);

void add_mutex(long long *pointer, long long value);

void add_test(long long *pointer, long long value);

void add_cas(long long *pointer, long long value);

void *thread_run();

void *thread_run_mutex();

void *thread_run_test();

void *thread_run_cas();

int main(int argc, char *argv[]) {
    static struct option long_options[] = {
        {"yield", no_argument, &yield_flag, 1},
        {"threads", required_argument, 0, 't'},
        {"iterations", required_argument, 0, 'i'},
        {"sync", required_argument, 0, 's'},
        {0, 0, 0, 0}};

    // process arguments
    int c;
    while ((c = getopt_long(argc, argv, "", long_options, NULL)) != -1) {
        switch (c) {
            case 'i':
                iterations = atoi(optarg);
                break;
            case 't':
                threads = atoi(optarg);
                break;
            case 's':
                if (strlen(optarg) != 1) {
                    fprintf(
                        stderr,
                        "sync option only takes singular character arguments!");
                    exit(1);
                } else {
                    strcpy(sync_opt, optarg);
                    if (*sync_opt != 'm' && *sync_opt != 's' &&
                        *sync_opt != 'c') {
                        fprintf(stderr,
                                "sync option only m, s, and c as arguments, "
                                "not: %s",
                                optarg);
                        exit(1);
                    }
                }
                break;
            case '?':
                exit(1);
                break;
        }
    }

    // malloc space to store the thread_ids
    pthread_t *thread_ids = malloc(sizeof(pthread_t) * threads);
    if (thread_ids == NULL) {
        fprintf(stderr, "Failed to malloc thread_ids");
        exit(1);
    }

    error = pthread_mutex_init(&lock, NULL);
    if (error != 0) {
        fprintf(stderr, "Failed to initialize the mutex");
        exit(1);
    }

    struct timespec start, stop;
    long long total_runtime;

    // start timer before spawning threads
    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) {
        fprintf(stderr, "clock gettime");
        exit(1);
    }

    // spawn threads number of threads
    for (int i = 0; i < threads; i++) {
        // depending on the sync option specified, will tun a different thread
        // routine which use different synchronization mechanisms
        if (*sync_opt == 'n') {
            pthread_create(&thread_ids[i], NULL, &thread_run, NULL);
        } else if (*sync_opt == 'm') {
            pthread_create(&thread_ids[i], NULL, &thread_run_mutex, NULL);
        } else if (*sync_opt == 's') {
            pthread_create(&thread_ids[i], NULL, &thread_run_test, NULL);
        } else if (*sync_opt == 'c') {
            pthread_create(&thread_ids[i], NULL, &thread_run_cas, NULL);
        } else {
            fprintf(stderr, "sync_opt has a bad value");
            exit(1);
        }
    }

    // wait for all threads to exit
    for (int i = 0; i < threads; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    // stop timing
    if (clock_gettime(CLOCK_MONOTONIC, &stop) == -1) {
        fprintf(stderr, "clock gettime");
        exit(1);
    }

    // calculate values for eventual output
    total_runtime =
        stop.tv_nsec - start.tv_nsec + (stop.tv_sec - start.tv_sec) * BILLION;
    int total_operations = 2 * threads * iterations;
    long long avg_runtime = total_runtime / total_operations;

    printf("add-");
    if (yield_flag) {
        printf("yield-");
    }
    if (*sync_opt == 'n') {
        printf("none");
    } else {
        printf(sync_opt);
    }
    printf(",%d,%d,%d,%lld,%lld,%lld\n", threads, iterations, total_operations,
           total_runtime, avg_runtime, counter);

    error = pthread_mutex_destroy(&lock);
    if (error != 0) {
        fprintf(stderr, "Failed to destroy the mutex");
        exit(1);
    }
    free(thread_ids);
    exit(0);
}

// unprotected add
void add(long long *pointer, long long value) {
    long long sum = *pointer + value;
    if (yield_flag) {
        sched_yield();
    }
    *pointer = sum;
}

// add protected by a mutex
void add_mutex(long long *pointer, long long value) {
    pthread_mutex_lock(&lock);
    long long sum = *pointer + value;
    if (yield_flag) {
        sched_yield();
    }
    *pointer = sum;
    pthread_mutex_unlock(&lock);
}

// add protected by a test and set spinlock
void add_test(long long *pointer, long long value) {
    while (__sync_lock_test_and_set(&exclusion, 1)) {
    }
    long long sum = *pointer + value;
    if (yield_flag) {
        sched_yield();
    }
    *pointer = sum;
    __sync_lock_release(&exclusion);
}

// add protected by compare and swap
void add_cas(long long *pointer, long long value) {
    long long sum;
    long long prev;
    do {
        prev = *pointer;
        sum = prev + value;
        if (yield_flag) {
            sched_yield();
        }
    } while (__sync_val_compare_and_swap(pointer, prev, sum) != prev);
}

// thread routine that calls unprotected add
void *thread_run() {
    for (int i = 0; i < iterations; i++) {
        add(&counter, 1);
        add(&counter, -1);
    }
    pthread_exit(NULL);
}

// thread routine that calls mutex protected add
void *thread_run_mutex() {
    for (int i = 0; i < iterations; i++) {
        add_mutex(&counter, 1);
        add_mutex(&counter, -1);
    }
    pthread_exit(NULL);
}

// thread routine that calls spin-lock protected add
void *thread_run_test() {
    for (int i = 0; i < iterations; i++) {
        add_test(&counter, 1);
        add_test(&counter, -1);
    }
    pthread_exit(NULL);
}

// thread routine that calls compare and swap protected add
void *thread_run_cas() {
    for (int i = 0; i < iterations; i++) {
        add_cas(&counter, 1);
        add_cas(&counter, -1);
    }
    pthread_exit(NULL);
}
