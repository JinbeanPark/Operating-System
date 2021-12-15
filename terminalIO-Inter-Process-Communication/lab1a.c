/*
NAME: Jinbean Park
EMAIL: keiserssose@gmail.com
ID: 805330751
*/

#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <getopt.h>
#include <sys/poll.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUF_SIZE 8

int flagEOF = 0;

// Use this variable to remember original terminal attributes.
struct termios restoreTermios;

// reset input mode before exiting
void resetInputMode() {
    tcsetattr(STDIN_FILENO, TCSANOW, &restoreTermios);
}

// 1. Character-at-a-time, full duplex terminal I/O 
// Get the current terminal modes.
void setInputMode() {
    struct termios tempTer;
    
    // Check whether stdin in a terminal or not.
    if (!isatty(STDIN_FILENO)) {
        fprintf(stderr, "Not a terminal.\n");
        exit(1);
    }
    
    // Save the terminal attributs to restore it later.
    tcgetattr(STDIN_FILENO, &restoreTermios);
    // Restore the terminal attributes at progrem exit.
    atexit(resetInputMode);

    // Make a copy.
    tcgetattr(STDIN_FILENO, &tempTer);
    tempTer.c_iflag = ISTRIP;
    tempTer.c_oflag = 0;
    tempTer.c_lflag = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &tempTer);
}

void signalHandler(int sigNum) {

    if (sigNum == SIGINT)
        fprintf(stderr, "Received SIGINT!\n");
    else if (sigNum == SIGPIPE) {
        fprintf(stderr, "Received SIGPIPE!\n");
        flagEOF = 1;
    }    
}

// Support a --shell=program argument to pass input/output between the terminal and a shell
static struct option long_options[] = {
    {"shell", required_argument, NULL, 's'},
    {0, 0, 0, 0}
};

int main(int argc, char **argv) {
    
    int readn, i, readByte, opt, ret;
    int hasShellOpt = 0;
    int fdPtoC[2], fdCtoP[2];
    pid_t childpid;
    char *argName = "/bin/bash";
    char readBuffer[256];
    char buf[BUF_SIZE];
    char rn[2];
    char EOT[2];
    char interrupt[2];
    rn[0] = '\r';
    rn[1] = '\n';
    EOT[0] = '^';
    EOT[1] = 'D';
    interrupt[0] = '^';
    interrupt[1] = 'C';


    opt = getopt_long(argc, argv, "", long_options, NULL);
    if (opt == -1)
        fprintf(stderr, "Options was not entered.\n");
    while (opt != -1) {
        switch (opt) {
            case 's':
                hasShellOpt = 1;
                break;
            default:
                fprintf(stderr, "Wrong argument was entered\n");
                exit(1);
        }
        opt = getopt_long(argc, argv, "", long_options, NULL);
    }
    
    // Modes should be set immediately on start-up(before any characteres are entered).
    setInputMode();

    // Execute the signalHandler if program receives SIGINT or SIGPIPE signal.
    signal(SIGINT, signalHandler);
    signal(SIGPIPE, signalHandler);

    // 1. Non shell mode
    if (hasShellOpt == 0) {
        readn = read(STDIN_FILENO, buf, BUF_SIZE);
        if (readn == -1) {
            fprintf(stderr, "Failed to read.\n");
            fprintf(stderr, "%s\n", strerror(errno));
            exit(1);
        }
        while (readn > 0) {
            for (i = 0; i < readn; i++) {
                // map received <cr> or <lf> into <cr><lf>
                if (buf[i] == '\r' || buf[i] == '\n')
                    write(STDOUT_FILENO, rn, sizeof(rn));
                // Upon detecting a pre-defined escape sequence (^D), restore normal
                //terminal modes and exit.
                else if (buf[i] == 0x04) {
                    write(STDOUT_FILENO, EOT, 2);
                    exit(0);
                }
                // write the received characters back out to the display,
                else
                    write(STDOUT_FILENO, &buf[i], 1);
            }
            readn = read(STDIN_FILENO, buf, BUF_SIZE);
            if (readn == -1) {
                fprintf(stderr, "Failed to read.\n");
                fprintf(stderr, "%s\n", strerror(errno));
                exit(1);
            }
        }
    }

    // 2. Shell mode (Passing input and output between two processes)
    else if (hasShellOpt == 1) {
        
        // Create two pipes, pipes are unidirectional (Child -> Parent & Parent -> Child)
        if (pipe(fdCtoP) != 0) {
            fprintf(stderr, "%s\n", strerror(errno));
            exit(1);
        }
        if (pipe(fdPtoC) != 0) {
            fprintf(stderr, "%s\n", strerror(errno));
            exit(1);
        }

        // fork to create a new process. 
        if ((childpid = fork()) == -1) {
            fprintf(stderr, "Creation of a child process was uncessful.\n");
            fprintf(stderr, "%s\n", strerror(errno));
            exit(1);
        }

        // Case of child process (Shell)
        else if (childpid == 0) {
            // Close the end of the pipe that they are not concerned with child process.
            close(fdCtoP[0]);
            close(fdPtoC[1]);

            // Standard input is a pipe from the terminal process,
            close(STDIN_FILENO);
            dup(fdPtoC[0]);
            close(fdPtoC[0]);

            // Standard output and standard error are a pipe to the terminal process. 
            close(STDOUT_FILENO);
            dup(fdCtoP[1]);
            close(STDERR_FILENO);
            dup(fdCtoP[1]);
            close(fdCtoP[1]);
            
            // exec the specified program (with no arguments other than its name)
            char *ar[] = {argName, NULL};
            if (execvp(argName, ar) == -1) {
                fprintf(stderr, "Failed to execute the specified program on child process\n");
                exit(1);
            }
        }

        // Case of parent process (Terminal)
        else {
            // Close the end of the pipe that they are not concerned with child process.
            close(fdCtoP[1]);
            close(fdPtoC[0]);

            // Create an array of two pollfd structures,
            struct pollfd fds[2];

            // One describing the keyboard (stdin)
            fds[0].fd = STDIN_FILENO;
            // One describing the pipe that returns output from the shell.
            fds[1].fd = fdCtoP[0];

            // Have both of these pollfds wait for either input (POLLIN) or
            // error(POLLHUP, POLLERR) events
            fds[0].events = POLLIN | POLLHUP | POLLERR;
            fds[1].events = POLLIN | POLLHUP | POLLERR;

            fds[0].revents = 0;
            fds[1].revents = 0;

            // Write a main loop that calls poll(2) (timeout = 0)
            while ((!flagEOF)) {
                // Error happened after calling poll.
                if ((ret = poll(fds, 2, 0)) < 0) {
                    fprintf(stderr, "%s\n", strerror(errno));
                    exit(1);
                }
                
                // Event happened after calling poll.
                else if (ret > 0) {
                    // Only reads from a file descriptor if it has pending input.
                    // Case of keyboard
                    if (fds[0].revents & POLLIN) {
                        if ((readByte = read(fds[0].fd, &buf, BUF_SIZE)) == - 1) {
                            fprintf(stderr, "Failed to read\n");
                            fprintf(stderr, "%s\n", strerror(errno));
                            exit(1);
                        }
                        else {
                            for (i = 0; i < readByte; i++) {
                                // Read input from the keyboard, echo it to stdout,
                                // and forward it to the shell.
                                if (buf[i] == '\r' || buf[i] == '\n') {
                                    write(STDOUT_FILENO, rn, sizeof(rn));
                                    write(fdPtoC[1], &rn[1], sizeof(char));
                                }
                                // When your program reads a ^C from the keyboard,
                                // it should use kill to send a SIGINT to the shell process.
                                else if (buf[i] == 0x03) {
                                    write(STDOUT_FILENO, interrupt, 2);
                                    if (kill(childpid, SIGINT) != 0) {
                                        fprintf(stderr, "%s\n", strerror(errno));
                                        exit(1);
                                    }
                                }
                                // Upon receiving an EOF from the terminal,
                                // close the pipe to the shell.
                                else if (buf[i] == 0x04) {
                                    write(STDOUT_FILENO, EOT, 2);
                                    close(fdPtoC[1]);
                                }
                                else {
                                // each read from the keyboard is likely to return only one character.
                                    write(STDOUT_FILENO, &buf[i], sizeof(char));
                                    write(fdPtoC[1], &buf[i], sizeof(char));
                                }
                            }
                        }
                    }
                    else if (fds[0].revents & POLLERR) {
                        fprintf(stderr, "%s\n", "Error happened while receiving input from the keyboard\n");
                        // Always process all availalbe input before processing the shut-down indication.
                        flagEOF = 1;
                    }
                    else if (fds[0].revents & POLLHUP) {
                        fprintf(stderr, "%s\n", "Connecting was broken while receiving input from the keyboard\n");
                        flagEOF = 1;
                    }

                    // Case of shell
                    if (fds[1].revents & POLLIN) {
                        // Read input from the shell pipe.
                        if ((readByte = read(fds[1].fd, readBuffer, sizeof(readBuffer))) == -1) {
                            fprintf(stderr, "Faild to read\n");
                            fprintf(stderr, "%s\n", strerror(errno));
                            exit(1);
                        }
                        else {
                            for (i = 0; i < readByte; i++) {
                                // Write it to stdout.
                                // If it receives an <lf> from the shell, it should print it to
                                // the screen as <cr><lf>
                                if (readBuffer[i] == '\n')
                                    write(STDOUT_FILENO, rn, sizeof(rn));
                                else if (readBuffer[i] == 0x04) {
                                    write(STDOUT_FILENO, EOT, 2);
                                    flagEOF = 1;
                                }
                                else
                                    write(STDOUT_FILENO, &readBuffer[i], sizeof(char));
                            }
                        }
                    }
                    else if (fds[1].revents & POLLERR) {
                        fprintf(stderr, "%s\n", "Error happened while receiving input from the shell pipe\n");
                        flagEOF = 1;
                    }
                    else if (fds[1].revents & POLLHUP) {
                        fprintf(stderr, "%s\n", "Connecting was broken while receiving input from the shell pipe\n");
                        flagEOF = 1;
                    }
                }
            }
            // After you have closed the write pipe to the shell
            close(fdPtoC[1]);
            int status;
            // Processed the final output returned from the shell,
            ret = waitpid(childpid, &status, 0);
            // You should collect the shell's exit status.
            fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", WTERMSIG(status), WEXITSTATUS(status));
        }
    }
    exit(0);
}