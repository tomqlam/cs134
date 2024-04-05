// NAME: Tom Lam
// EMAIL: tlam@hmc.edu
// ID: 40210352
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "SortedList.h"

#define BILLION 1000000000
#define MAXSTRLEN 16

// initialize globals
long long counter = 0;
int threads = 1;
int iterations = 1;
int error;
char sync_opt[2] = "n";
int opt_yield = 0;
pthread_mutex_t lock;
volatile int exclusion = 0;
SortedListElement_t(*randomElems);
SortedList_t list = {.prev = &list, .next = &list, .key = NULL};

void* thread_run_list(void* t_arg);

void* thread_run_list_test(void* t_arg);

void* thread_run_list_mutex(void* t_arg);

void signal_handler(int signal_number);

int main(int argc, char* argv[]) {
    static struct option long_options[] = {
        {"yield", required_argument, 0, 'y'},
        {"threads", required_argument, 0, 't'},
        {"iterations", required_argument, 0, 'i'},
        {"sync", required_argument, 0, 's'},
        {0, 0, 0, 0}};

    // argument processing
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
                    fprintf(stderr,
                            "sync option only takes singular character "
                            "arguments!\n");
                    exit(1);
                } else {
                    strcpy(sync_opt, optarg);
                    if (*sync_opt != 'm' && *sync_opt != 's') {
                        fprintf(stderr,
                                "sync option only takes s and m as arguments, "
                                "not: %s\n",
                                optarg);
                        exit(1);
                    }
                }
                break;
            case 'y':
                if (strlen(optarg) > 3) {
                    fprintf(
                        stderr,
                        "yield option only at most 3 character arguments!\n");
                    exit(1);
                } else {
                    for (int i = 0; i < (int)strlen(optarg); i++) {
                        switch (optarg[i]) {
                            case 'i':
                                opt_yield = opt_yield | INSERT_YIELD;
                                break;
                            case 'd':
                                opt_yield = opt_yield | DELETE_YIELD;
                                break;
                            case 'l':
                                opt_yield = opt_yield | LOOKUP_YIELD;
                                break;
                            case '?':
                                fprintf(stderr,
                                        "bad character: %c in yield option\n",
                                        optarg[i]);
                                exit(1);
                                break;
                        }
                    }
                }
                break;
            case '?':
                exit(1);
                break;
        }
    }
    // seeding prng with current time
    srand(time(NULL));

    // malloc 2d array of Sorted list elements
    randomElems = malloc(sizeof(SortedListElement_t[threads * iterations]));
    if (randomElems == NULL) {
        fprintf(stderr, "Failed to malloc randomElems");
        exit(1);
    }

    // iterate through the 2d array and assign random string values to the keys
    // of the elements
    int r;
    char buf[MAXSTRLEN];
    for (int i = 0; i < threads; i++) {
        for (int j = 0; j < iterations; j++) {
            r = rand();
            snprintf(buf, MAXSTRLEN, "%d", r);
            randomElems[i * iterations + j].key = strndup(buf, MAXSTRLEN);
        }
    }

    // set signal handler for segfaults
    signal(SIGSEGV, signal_handler);

    // malloc array to store all of the thread_ids
    pthread_t* thread_ids = malloc(sizeof(pthread_t) * threads);
    if (thread_ids == NULL) {
        fprintf(stderr, "Failed to malloc thread_ids");
        exit(1);
    }

    // initialize the mutex
    error = pthread_mutex_init(&lock, NULL);
    if (error != 0) {
        fprintf(stderr, "Failed to initialize the mutex");
        exit(1);
    }

    // malloc a list of thread indexes to pass through to thread routines
    int* thread_indexes = malloc(sizeof(int) * threads);
    if (thread_indexes == NULL) {
        fprintf(stderr, "Failed to malloc thread_indexes");
        exit(1);
    }
    for (int i = 0; i < threads; i++) {
        thread_indexes[i] = i;
    }

    // start timer
    struct timespec start, stop;
    long long total_runtime;
    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) {
        fprintf(stderr, "clock gettime");
        exit(1);
    }

    // create the threads
    for (int i = 0; i < threads; i++) {
        // depending on the sync option specified, run a different thread
        // routine protected by various sync methods
        if (*sync_opt == 'n') {
            pthread_create(&thread_ids[i], NULL, &thread_run_list,
                           &thread_indexes[i]);
        } else if (*sync_opt == 'm') {
            pthread_create(&thread_ids[i], NULL, &thread_run_list_mutex,
                           &thread_indexes[i]);
        } else if (*sync_opt == 's') {
            pthread_create(&thread_ids[i], NULL, &thread_run_list_test,
                           &thread_indexes[i]);
        } else {
            fprintf(stderr, "sync_opt has a bad value");
            exit(1);
        }
    }

    // wait for all threads to return
    for (int i = 0; i < threads; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    // end timer
    if (clock_gettime(CLOCK_MONOTONIC, &stop) == -1) {
        fprintf(stderr, "clock gettime");
        exit(1);
    }

    // error out if the length of the sorted list length is not 0 at the end
    error = SortedList_length(&list);
    if (error == -1) {
        fprintf(stderr, "corruption found in list while finding length\n");
        exit(2);
    } else if (error > 0) {
        fprintf(stderr, "non-zero length means corrupted list\n");
        exit(2);
    }

    // processing time data for later printing
    total_runtime =
        stop.tv_nsec - start.tv_nsec + (stop.tv_sec - start.tv_sec) * BILLION;
    int total_operations = 3 * threads * iterations;
    long long avg_runtime = total_runtime / total_operations;

    printf("list-");
    if (opt_yield & INSERT_YIELD) printf("i");
    if (opt_yield & DELETE_YIELD) printf("d");
    if (opt_yield & LOOKUP_YIELD) printf("l");
    if (opt_yield == 0) printf("none");

    printf("-");

    if (*sync_opt == 'n') {
        printf("none");
    } else {
        printf(sync_opt);
    }

    printf(",%d,%d,%d,%d,%lld,%lld\n", threads, iterations, 1, total_operations,
           total_runtime, avg_runtime);

    for (int i = 0; i < threads * iterations; i++) {
        free((char*)randomElems[i].key);
    }

    // cleanup
    free(randomElems);
    free(thread_ids);
    error = pthread_mutex_destroy(&lock);
    if (error != 0) {
        fprintf(stderr, "Failed to destroy the mutex");
        exit(1);
    }
    exit(0);
}

// unprotected list testing thread routine
void* thread_run_list(void* t_arg) {
    int* index = t_arg;
    for (int j = 0; j < iterations; j++) {
        SortedList_insert(&list, &randomElems[(*index) * iterations + j]);
    }

    error = SortedList_length(&list);
    if (error == -1) {
        fprintf(stderr, "corruption found in list while finding length\n");
        exit(2);
    }

    for (int j = 0; j < iterations; j++) {
        if (SortedList_lookup(
                &list, randomElems[(*index) * iterations + j].key) == NULL) {
            fprintf(stderr, "could not find key in list\n");
            exit(2);
        }

        if (SortedList_delete(&randomElems[(*index) * iterations + j])) {
            fprintf(stderr, "could not delete key in list, found corruption\n");
            exit(2);
        }
    }

    pthread_exit(NULL);
}

// spin lock protected list testing thread routine
void* thread_run_list_test(void* t_arg) {
    int* index = t_arg;
    for (int j = 0; j < iterations; j++) {
        while (__sync_lock_test_and_set(&exclusion, 1)) {
        }
        SortedList_insert(&list, &randomElems[(*index) * iterations + j]);
        __sync_lock_release(&exclusion);
    }

    while (__sync_lock_test_and_set(&exclusion, 1)) {
    }
    error = SortedList_length(&list);
    if (error == -1) {
        fprintf(stderr, "corruption found in list while finding length\n");
        __sync_lock_release(&exclusion);
        exit(2);
    }
    __sync_lock_release(&exclusion);

    for (int j = 0; j < iterations; j++) {
        while (__sync_lock_test_and_set(&exclusion, 1)) {
        }
        if (SortedList_lookup(
                &list, randomElems[(*index) * iterations + j].key) == NULL) {
            fprintf(stderr, "could not find key in list\n");
            __sync_lock_release(&exclusion);
            exit(2);
        }
        __sync_lock_release(&exclusion);

        while (__sync_lock_test_and_set(&exclusion, 1)) {
        }
        if (SortedList_delete(&randomElems[(*index) * iterations + j])) {
            fprintf(stderr, "could not delete key in list, found corruption\n");
            __sync_lock_release(&exclusion);
            exit(2);
        }
        __sync_lock_release(&exclusion);
    }

    pthread_exit(NULL);
}

// mutex protected list testing thread routine
void* thread_run_list_mutex(void* t_arg) {
    int* index = t_arg;
    for (int j = 0; j < iterations; j++) {
        pthread_mutex_lock(&lock);
        SortedList_insert(&list, &randomElems[(*index) * iterations + j]);
        pthread_mutex_unlock(&lock);
    }

    pthread_mutex_lock(&lock);
    error = SortedList_length(&list);
    if (error == -1) {
        fprintf(stderr, "corruption found in list while finding length\n");
        pthread_mutex_unlock(&lock);
        exit(2);
    }
    pthread_mutex_unlock(&lock);

    for (int j = 0; j < iterations; j++) {
        pthread_mutex_lock(&lock);
        if (SortedList_lookup(
                &list, randomElems[(*index) * iterations + j].key) == NULL) {
            fprintf(stderr, "could not find key in list\n");
            pthread_mutex_unlock(&lock);
            exit(2);
        }
        pthread_mutex_unlock(&lock);

        pthread_mutex_lock(&lock);
        if (SortedList_delete(&randomElems[(*index) * iterations + j])) {
            fprintf(stderr, "could not delete key in list, found corruption\n");
            pthread_mutex_unlock(&lock);
            exit(2);
        }
        pthread_mutex_unlock(&lock);
    }

    pthread_exit(NULL);
}

// handles signal for segfaults
void signal_handler(int signal_number) {
    fprintf(stderr, "signal: %s handled\n", strsignal(signal_number));
    exit(2);
}