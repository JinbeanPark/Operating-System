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

extern int errno;

void sigHandler(int sigNum) {
    if (sigNum == SIGSEGV) {
        fprintf(stderr, "Received SIGSEGV\n");
        exit(4);
    }
}

// 1. Process all arguments and store the results in variables.
static struct option long_options[] = {
    {"input", required_argument, NULL, 'i'}, // return val = 'i' because flag set to NULL.
    {"output", required_argument, NULL, 'o'}, // return val = 'o'
    {"segfault", no_argument, NULL, 's'}, // return val = 's'
    {"catch", no_argument, NULL, 'c'}, // return val = 'c'
    {0, 0, 0, 0} // Resetting to 0.
};

int main(int argc, char **argv) {
    
    int opt, fdi, fdo;
    int chkSegOpt = 0;
    char buf;
    int readn;
    char *nullPointer = NULL;

    opt = getopt_long(argc, argv, "", long_options, NULL);
    if (opt == -1)
        fprintf(stderr, "Options was not entered.\n");

    while (opt != -1) {
        
        switch(opt) {
            case 'i':
                if ((fdi = open(optarg, O_RDONLY)) == -1) {
                    fprintf(stderr, "Argument --input caused the problem\n");
                    fprintf(stderr, "Failed to open the input file '%s'\n", optarg);
                    fprintf(stderr, "%s\n", strerror(errno));
                    //perror("The following error occurred\n");
                    exit(2);
                }
                // 2. Do any file redirection
                else if (fdi >= 0) {
                    close(0);
                    dup(fdi);
                    close(fdi);
                }
                break;
            case 'o':
                // Mode 0644: Owner only can read/write, others only can read. 
                if ((fdo = creat(optarg, 0644)) == -1) {
                    fprintf(stderr, "Argument --output caused the problem\n");
                    fprintf(stderr, "Failed to create the output file '%s'\n", optarg);
                    fprintf(stderr, "%s\n", strerror(errno));
                    //perror("The following error occurred\n");
                    exit(3);
                }
                // 2. Do any file redirection
                else if (fdo >= 0) {
                    close(1);
                    dup(fdo);
                    close(fdo);
                }
                break;
            case 's':
                chkSegOpt = 1;
                break;
            case 'c':
            // 3. Register the signal handler.
                if (signal(SIGSEGV, sigHandler) == SIG_ERR)
                    fprintf(stderr, "Fail to catch SIGSEGV\n");
                break;
            default:
                fprintf(stderr, "Entered options and arguments in wrong format.\n");
                fprintf(stderr, "Correct usage: --input=filename --output=filename --segfault --catch\n");
                exit(1);
        }
        opt = getopt_long(argc, argv, "", long_options, 0);
    }

    // 4. Cause the segfault.
    if (chkSegOpt == 1)
        // Tried storing the char 'c' on NULL pointer to cause the segfault.
        memset(nullPointer, 'c', sizeof(char));

    // 5. If no segfault was caused, copy stdin to stdout.
    else {
        while ((readn = read(0, &buf, sizeof(buf))) > 0) {
            if (readn == -1) {
                fprintf(stderr, "Failed to read.\n");
                fprintf(stderr, "%s\n", strerror(errno));
                exit(1);
            }
            if (write(1, &buf, sizeof(buf)) == -1) {
                fprintf(stderr, "Failed to write.\n");
                fprintf(stderr, "%s\n", strerror(errno));
                exit(1);
            }
        }
    }
    exit(0);
}