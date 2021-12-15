/*
NAME: Jinbean Park
EMAIL: keiserssose@gmail.com
ID: 805330751
*/

#include "SortedList.h"
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

// Initialize an empty list.
SortedList_t *sharedLists;
SortedListElement_t *elmts;
int numThreads, numIterations, chkSync = 0, numElmt, numSublist = 1;
char optSync = NULL;
pthread_mutex_t *lockM;
static int *lockS;
pthread_t *tid;
int keyLen = 4;
long long totalLockTime = 0;

static struct option long_options[] = {
    {"threads", required_argument, NULL, 't'}, // Takes a parameter for the number of parallel threads
    {"iterations", required_argument, NULL, 'i'}, // Takes a parameter for the number of iterations.
    {"yield", required_argument, NULL, 'y'}, // Take a parameter to enable optional critical section yields.
    {"sync", required_argument, NULL, 's'}, // Implement three new version of your add function
    {"lists", required_argument, NULL, 'l'}, // Add a new --lists=# option
    {0, 0, 0, 0}
};

void sigHandler(int sigNum) {
    if (sigNum == SIGSEGV) {
        fprintf(stderr, "Segmentation fault happened!\n");
        exit(2);
    }
}

void freeElmts() {
    int i;
    for (i = 0; i < numElmt; i++)
        free((char *) elmts[i].key);
    free(elmts);
}

void freeThreads() {
    free(tid);
}

unsigned int getHashKey(const char *keyVal) {
    unsigned int sumIndx = 0;
    int i;
    for (i = 0; i < keyLen; i++)
        sumIndx += keyVal[i];
    return (sumIndx % numSublist);
}

void *t_function(void *index) {

    int indexThread = *((int *)index);
    int i;
    int hashKey = 0;
    struct timespec start_time, end_time;
    long long nano = 1000000000LL;

    // Inserts them all into a (single shared-by-all-threads) list
    for (i = (indexThread * numIterations); i < ((indexThread + 1) * numIterations); i++) {
        
        hashKey = getHashKey(elmts[i].key);
        clock_gettime(CLOCK_MONOTONIC, &start_time);
        if (optSync == 'm')
            pthread_mutex_lock(lockM + hashKey);
        else if (optSync == 's')
            while (__sync_lock_test_and_set(lockS + hashKey, 1));
        clock_gettime(CLOCK_MONOTONIC, &end_time);
        totalLockTime += nano * (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec);
        
        SortedList_insert(sharedLists + hashKey, &elmts[i]);
        
        if (optSync == 'm')
            pthread_mutex_unlock(lockM + hashKey);
        else if (optSync == 's')
            __sync_lock_release(lockS + hashKey);
    }
    
    // Gets the list length
    long long totalListLen = 0;
    for (i = 0; i < numSublist; i++) {
        clock_gettime(CLOCK_MONOTONIC, &start_time);
        if (optSync == 'm')
            pthread_mutex_lock(lockM + i);
        else if (optSync == 's')
            while (__sync_lock_test_and_set(lockS + i, 1));
        clock_gettime(CLOCK_MONOTONIC, &end_time);
        totalLockTime += nano * (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec);
        
        totalListLen += SortedList_length(sharedLists + i);
        if (totalListLen < 0) {
            fprintf(stderr, "Error happened while getting length of the list\n");
            exit(2);
        }
        if (optSync == 'm')
            pthread_mutex_unlock(lockM + i);
        else if (optSync == 's')
            __sync_lock_release(lockS + i);
    }
    
    // Looks up and deletes each of the keys it had previously inserted.
    for (i = (indexThread * numIterations); i < ((indexThread + 1) * numIterations); i++) {
        
        hashKey = getHashKey(elmts[i].key);
        clock_gettime(CLOCK_MONOTONIC, &start_time);
        if (optSync == 'm')
            pthread_mutex_lock(lockM + hashKey);
        else if (optSync == 's')
            while (__sync_lock_test_and_set(lockS + hashKey, 1));
        clock_gettime(CLOCK_MONOTONIC, &end_time);
        totalLockTime += nano * (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec);
        
        SortedListElement_t *toDelElm = SortedList_lookup(sharedLists + hashKey, elmts[i].key);
        if (toDelElm == NULL) {
            fprintf(stderr, "Failed to find the keys\n");
            exit(2);
        }
        if (SortedList_delete(toDelElm) == 1) {
            fprintf(stderr, "Corrupted prev/next pointers\n");
            exit(2);
        }
        if (optSync == 'm')
            pthread_mutex_unlock(lockM + hashKey);
        else if (optSync == 's')
            __sync_lock_release(lockS + hashKey);
    }
    // exits to re-join the parent thread
    return NULL;
}

int main(int argc, char **argv) {

    // Simply experience a segmentation fault. Catch these, and log and return an error.
    signal(SIGSEGV, sigHandler);

    opt_yield = 0;
    unsigned int u;

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
                for (u = 0; u < strlen(optarg); u++) {
                    if (optarg[u] == 'i')
                        opt_yield |= INSERT_YIELD;
                    else if (optarg[u] == 'd')
                        opt_yield |= DELETE_YIELD;
                    else if (optarg[u] == 'l')
                        opt_yield |= LOOKUP_YIELD;
                }
                break;
            case 's':
                optSync = optarg[0];
                chkSync = 1;
                break;
            case 'l':
                numSublist = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Enter the options and arguments in correct format.\n");
                exit(1);
        }
        opt = getopt_long(argc, argv, "", long_options, 0);
    }
    
    // Creates and initializes (with random keys) the required numbers of list elements.
    sharedLists = (SortedList_t*) malloc(sizeof(SortedList_t) * numSublist);
    if (sharedLists == NULL) {
        fprintf(stderr, "Failed to create sharedLists\n");
        exit(1);
    }

    numElmt = numThreads * numIterations;
    elmts = (SortedListElement_t*) malloc(sizeof(SortedListElement_t) * numElmt);
    if (elmts == NULL) {
        fprintf(stderr, "Failed to allocate memory!\n");
        exit(1);
    }
    srand((unsigned int)time(0));
    int i, j;
    for (i = 0; i < numElmt; i++) {
        char *keyVal = (char*) malloc(sizeof(char) * (keyLen + 1));
        if (keyVal == NULL) {
            fprintf(stderr, "Failed to allocate memory!\n");
            exit(1);
        }
        for (j = 0; j < keyLen; j++)
            keyVal[j] = 'a' + (rand() % 26);
        keyVal[keyLen] = '\0';
        elmts[i].key = keyVal;
    }
    // Free elements at the end of the test.
    atexit(freeElmts);

    // A mutex is initialized in the beginning of the main function.
    if (optSync == 'm') {
        lockM = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t) * numSublist);
        if (lockM == NULL) {
            fprintf(stderr, "Failed to generate the memory for mutex\n");
            exit(1);
        }
        for (i = 0; i < numSublist; i++) {
            if (pthread_mutex_init(lockM + i, NULL) != 0) {
                fprintf(stderr, "Failed to initialize the mutex!\n");
                exit(1);
            }
        }
    }
    else if (optSync == 's') {
        lockS = (int *) malloc(sizeof(int) * numSublist);
        if (lockS == NULL) {
            fprintf(stderr, "Failed to generate the memory for spin-locks\n");
            exit(1);
        }
        for (i = 0; i < numSublist; i++)
            lockS[i] = 0;
    }

    // Notes the (high resolution) starting time for the run.
    struct timespec startTS, endTS;
    if (clock_gettime(CLOCK_MONOTONIC, &startTS) == -1) {
        fprintf(stderr, "Failed to record starting time\n");
        exit(1);
    }

    // starts the specified number of threads.
    tid = (pthread_t *) malloc(sizeof(pthread_t) * numThreads);
    atexit(freeThreads);
    if (tid == NULL) {
        fprintf(stderr, "Failed to allocate memory!\n");
        exit(1);
    }

    int *index = (int *) malloc(sizeof(int) * numThreads);
    i = 0;
    while (i < numThreads) {
        index[i] = i;
        if (pthread_create(&tid[i], NULL, &t_function, (void *)(index + i)) != 0) {
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

    // Checks the length of the list to confirm that it is zero.
    unsigned long long totalListLen = 0;
    for (i = 0; i < numSublist; i++)
        totalListLen += SortedList_length(sharedLists + i);
    if (totalListLen != 0) {
        fprintf(stderr, "The length of the list is not zero\n");
        exit(2);
    }
    
    // Prints to stdout a comma-separated-value (CSV) record.
    // The name of the test, which is of the form: list-yieldopts-syncopts
    printf("list-");
    switch (opt_yield) {
        case 0:
            printf("none-");
            break;
        case 1:
            printf("i-");
            break;
        case 2:
            printf("d-");
            break;
        case 3:
            printf("id-");
            break;
        case 4:
            printf("l-");
            break;
        case 5:
            printf("il-");
            break;
        case 6:
            printf("dl-");
            break;
        case 7:
            printf("idl-");
            break;
        default:
            break;
    }
    if (chkSync) {
        if (optSync == 's')
            printf("s,");
        else
            printf("m,");
    }
    else
        printf("none,");
    
    // The number of threads (from --threads=)
    printf("%d,", numThreads);
    
    // The number of iterations (from --iterations=)
    printf("%d,", numIterations);
    
    // Include the number of lists as the fourth number in the output statistics report.
    printf("%d,", numSublist);
    
    // The total number of operations performed: threads x iterations x 3 (insert + look + delete)
    long long numOperations = numThreads * numIterations * 3;
    printf("%lld,", numOperations);
    
    // The total run time (in nanoseconds) for all threads
    const long long nano = 1000000000LL;
    long long totalRunTime = nano * (endTS.tv_sec - startTS.tv_sec) + (endTS.tv_nsec - startTS.tv_nsec);
    printf("%lld,", totalRunTime);

    // The average time per operations (in nanoseconds)
    long long timePerOper = totalRunTime / numOperations;
    printf("%lld,", timePerOper);

    // The average wait-for-lock (in nanoseconds)
    long long waitForLock = totalLockTime / numOperations;
    printf("%lld\n", waitForLock);
    
    if (optSync == 'm') {
        for (i = 0; i < numSublist; i++)
            pthread_mutex_destroy(lockM + 1);
        free(lockM);
    }
    else if (optSync == 's')
        free(lockS);
    exit(0);
}