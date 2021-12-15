/*
NAME: Jinbean Park
EMAIL: keiserssose@gmail.com
ID: 805330751
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <sched.h>

long long counter;
int numThreads, numIterations, opt_yield = 0, chkSync = 0;
char optSync = NULL;
pthread_mutex_t lockM;
static int lockS;
pthread_t *tid;

static struct option long_options[] = {
    {"threads", required_argument, NULL, 't'}, // Takes a parameter for the number of parallel threads
    {"iterations", required_argument, NULL, 'i'}, // Takes a parameter for the number of iterations.
    {"yield", no_argument, NULL, 'y'}, // Add a new --yield to your driver program that sets opt_yield to 1.
    {"sync", required_argument, NULL, 's'}, // Implement three new version of your add function
    {0, 0, 0, 0}
};

void add(long long *pointer, long long value) {
    
    if (chkSync) {
        if (optSync == 'm') {
            pthread_mutex_lock(&lockM);
            long long sum = *pointer + value;
            if (opt_yield)
                sched_yield();
            *pointer = sum;
            pthread_mutex_unlock(&lockM);
        }
        else if (optSync == 's') {
            while (__sync_lock_test_and_set(&lockS, 1));
            long long sum = *pointer + value;
            if (opt_yield)
                sched_yield();
            *pointer = sum;
            __sync_lock_release(&lockS);
        }
        else if (optSync == 'c') {
            long long oldVal, sum;
            do {
                oldVal = *pointer;
                sum = oldVal + value;
                // The yield check should be put between the computation
                // of the new sum and performing the compare-and-swap;
                if (opt_yield)
                    sched_yield();
            } while (__sync_val_compare_and_swap(pointer, oldVal, sum) != oldVal);
        }
    }
    else {
        long long sum = *pointer + value;
        if (opt_yield)
            sched_yield();
        *pointer = sum;
    }
}

void *t_function(void *data) {

    int i = 0;
    // add 1 to the counter the specified number of times
    // add -1 to the counter the specified number fo times
    while (i < numIterations) {
        add(&counter, 1);
        add(&counter, -1);
        i++;
    }
    // exit to re-join the parent thread.
    return data;
}

void freeThreads() {
    free(tid);
}

int main(int argc, char **argv) {

    int opt = getopt_long(argc, argv, "", long_options, NULL);
    if (opt == -1)
        fprintf(stderr, "Options was not entered.\n");

    while (opt != -1) {
        
        switch(opt) {
            case 't':
                if (optarg == NULL)
                    numThreads = 1;
                else
                    numThreads = atoi(optarg);
                break;
            case 'i':
                if (optarg == NULL)
                    numIterations = 1;
                else
                    numIterations = atoi(optarg);
                break;
            case 'y':
                opt_yield = 1;
                break;
            case 's':
                optSync = optarg[0];
                chkSync = 1;
                break;
            default:
                fprintf(stderr, "Enter the options and arguments in correct format.\n");
                exit(1);
        }
        opt = getopt_long(argc, argv, "", long_options, 0);
    }

    // Initialize a (long long) counter to zero
    counter = 0;

    // starts the specified number of threads.
    tid = (pthread_t *) malloc(sizeof(pthread_t) * numThreads);

    atexit(freeThreads);

    if (tid == NULL) {
        fprintf(stderr, "Failed to allocate memory!\n");
        exit(1);
    }

    // A mutex is initialized in the beginning of the main function.
    if (optSync == 'm') {
        if (pthread_mutex_init(&lockM, NULL) != 0) {
            fprintf(stderr, "Failed to initialize the mutext!\n");
            exit(1);
        }
    }
    
    //notes the (high resolution) starting time for the run (using clock_gettime(3))
    struct timespec startTS, endTS;
    if (clock_gettime(CLOCK_MONOTONIC, &startTS) == -1) {
        fprintf(stderr, "Failed to record starting time\n");
        exit(1);
    }

    // starts the specified number of threads, each of which will use the above and add function to
    int i = 0;
    while (i < numThreads) {
        if (pthread_create(&tid[i], NULL, &t_function, NULL) != 0) {
            fprintf(stderr, "Failed to create thread\n");
            exit(1);
        }
        i++;
    }

    // Wait for all threads to complete.
     i = 0;
    while (i < numThreads) {
        pthread_join(tid[i], NULL);
        i++;
    }

    // Notes the (high resolution) ending time for the run.
    if (clock_gettime(CLOCK_MONOTONIC, &endTS) == -1) {
        fprintf(stderr, "Faild to record ending time\n");
        exit(1);
    }

    // prints to stdout a comma-separated-value (CSV) record including:
    const long long nano = 1000000000LL;
    long long numOperation = numThreads * numIterations * 2;
    long long totalRunTime = nano * (endTS.tv_sec - startTS.tv_sec) + (endTS.tv_nsec - startTS.tv_nsec);
    long long timePerOper = totalRunTime / numOperation;

    if (opt_yield) {
        if (optSync) {
            if (optSync == 'm')
                printf("add-yield-m,");
            else if (optSync == 's')
                printf("add-yield-s,");
            else
                printf("add-yield-c,");
        }
        else {
            printf("add-yield-none,");
        }
    }
    else {
        if (optSync) {
            if (optSync == 'm')
                printf("add-m,");
            else if (optSync == 's')
                printf("add-s,");
            else
                printf("add-c,");
        }
        else {
            printf("add-none,");
        }
    }
    
    printf("%d,", numThreads);
    printf("%d,", numIterations);
    printf("%lld,", numOperation);
    printf("%lld,", totalRunTime);
    printf("%lld,", timePerOper);
    printf("%lld\n", counter);
    
    // The mutex is destroyed when the threads have completed their execution.
    if (optSync == 'm')
        pthread_mutex_destroy(&lockM);
    exit(0);
}