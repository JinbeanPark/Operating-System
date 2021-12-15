/*
NAME: Jinbean Park
EMAIL: keiserssose@gmail.com
ID: 805330751
*/

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <zlib.h>
#include <getopt.h>
#include <poll.h>
#include <unistd.h>
#include <termios.h>
#include <netdb.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include <assert.h>
#include <stdbool.h>

#define BUF_SIZE 256
    
int flagEOF = 0;
int sockfd, newSock;
int fdPtoC[2], fdCtoP[2];
pid_t childpid;
z_stream out_stream; // Compress child shell's STDOUT and send to socket
z_stream in_stream; // Decompress socket data and send to child shell's STDIN


void childExit() {

    close(sockfd);
    close(newSock);
    close(fdPtoC[1]);
    close(fdCtoP[0]);
    int status;
    waitpid(childpid, &status, 0);
    fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", WIFSIGNALED(status), WEXITSTATUS(status));
    exit(0);
}

void signalHandler(int sigNum) {

    if (sigNum == SIGPIPE) {
        close(fdPtoC[1]);
        close(fdCtoP[0]);
        kill(childpid, SIGKILL);
        int status;
        waitpid(childpid, &status, 0);
        fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", WIFSIGNALED(status), WEXITSTATUS(status));
        exit(0);
    }
}

static struct option long_options[] = {
    {"port", required_argument, NULL, 'p'},
    {"shell", required_argument, NULL, 's'},
    {"compress", no_argument, NULL, 'c'},
    {0, 0, 0, 0}
};

int main(int argc, char** argv) {

    struct sockaddr_in address;
    int opt, portNum, chkPort, chkShell, chkCompres, ret, i;
    unsigned int j;
    chkShell = chkPort = chkCompres = 0;
    char rn[2];
    rn[0] = '\r';
    rn[1] = '\n';
    char buf[BUF_SIZE] = {0};
    char readBuffer[256];
    struct hostent* server;
    char *argName = "/bin/bash";

    opt = getopt_long(argc, argv, "", long_options, NULL);
    if (opt == -1)
        fprintf(stderr, "Options was not entered.\n");

    while (opt != -1) {
        switch (opt) {
        case 'p':
            portNum = atoi(optarg);
            chkPort = 1;
            break;
        case 's':
            chkShell = 1;
            break;
        case 'c':
            chkCompres = 1;
            in_stream.zalloc = Z_NULL;
            in_stream.zfree = Z_NULL;
            in_stream.opaque = Z_NULL;
            if (deflateInit(&out_stream, Z_DEFAULT_COMPRESSION) != Z_OK) {
                fprintf(stderr, "Unable to create compression stream\n");
                exit(1);
            }
            out_stream.zalloc = Z_NULL;
            out_stream.zfree = Z_NULL;
            out_stream.opaque = Z_NULL;
            if (inflateInit(&in_stream) != Z_OK) {
                fprintf(stderr, "Unable to create decompression stream\n");
                exit(1);
            }
            break;
        default:
            fprintf(stderr, "Entered options and arguments in wrong format.\n");
            fprintf(stderr, "Correct usage: --port='port number' --shell='/bin/bash' --compress\n");
            exit(1);
        }
        opt = getopt_long(argc, argv, "", long_options, 0);
    }

    if (!chkPort) {
        fprintf(stderr, "The port argument was not entered!\n");
        exit(1);
    }
    
    else if (chkPort) {

        // 1. Create a socket
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            fprintf(stderr, "Failed to create a socket\n");
            exit(1);
        }
    
        // 2. Fill server's sockaddr_in struct.
        memset(&address, '0', sizeof(address));
        address.sin_family = AF_INET;
        if (!(server = gethostbyname("localhost"))) {
            fprintf(stderr, "Failed to get structure describing an internet host\n");
            exit(1);
        }

        //address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(portNum);
        bcopy((char *)server->h_addr, (char *)&address.sin_addr.s_addr, server->h_length);

        // 3. Bind() socket to name (= Register the sock I made to sever socket)
        if (bind(sockfd, (struct sockaddr *) &address, sizeof(address)) < 0) {
            fprintf(stderr, "Error happened while binding socket!\n");
            exit(1);
        }
    
        // 4. Establish connection with client by using listen and accept.
        // 1) Start listening for incoming connections
        if (listen(sockfd, 5) < 0) {
            fprintf(stderr, "Error happened while listening!\n");
            exit(1);
        }
        // 2) Accept a connection on a socket (blocking until a connection is accepted)
        int sizeSockAddr = sizeof(address);
        if ((newSock = accept(sockfd, (struct sockaddr *) &address, (socklen_t *) &(sizeSockAddr))) < 0) {
            fprintf(stderr, "Error happened while accepting!\n");
            exit(1);
        }
        
        // 5. Input / output forwarding
        // 1) poll on socket and shell
        // 2) Decompress if necessary

        // fork a child process, which will exec a shell to process commands
        // received; the server process should communicate with the shell via pipes.
        
        // Create two pipes, pipes are unidirectional (Shell -> Server & Server -> Shell)        
        if (pipe(fdCtoP) != 0) {
            fprintf(stderr, "%s\n", strerror(errno));
            exit(1);
        }
        if (pipe(fdPtoC) != 0) {
            fprintf(stderr, "%s\n", strerror(errno));
            exit(1);
        }

        // Executes the signalHandler if program receives SIGPIPE signal.
        signal(SIGPIPE, signalHandler);

        // fork a child process. 
        if ((childpid = fork()) == -1) {
            fprintf(stderr, "Creation of a child process was uncessful.\n");
            fprintf(stderr, "%s\n", strerror(errno));
            exit(1);
        }

        // Case of child process (Shell)
        else if (!childpid) {
            // Close the end of the pipe that they are not concerned with child process.
            close(fdCtoP[0]);
            close(fdPtoC[1]);

            // Redirect the shell process's stdin/stdout/stderr
            // to the appropriate pipe ends
            // Standard input is a pipe from the server,
            close(STDIN_FILENO);
            dup(fdPtoC[0]);
            close(fdPtoC[0]);

            // Standard output and standard error are a pipe to the server. 
            close(STDOUT_FILENO);
            dup(fdCtoP[1]);
            close(STDERR_FILENO);
            dup(fdCtoP[1]);
            close(fdCtoP[1]);
            
            // exec the specified program (with no arguments other than its name)
            if (chkShell) {
                char *ar[] = {argName, NULL};
                if (execvp(argName, ar) == -1) {
                    fprintf(stderr, "Failed to execute the specified program on child process\n");
                    exit(1);
                }
            }
        }

        // Case of parent process (Server)
        else {
            // Close the end of the pipe that they are not concerned with child process.
            close(fdCtoP[1]);
            close(fdPtoC[0]);

            // Create an array of two pollfd structures,
            struct pollfd fds[2];

            // One describing the network socket from client.
            fds[0].fd = newSock;
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
                
                atexit(childExit);

                // Error happened after calling poll.
                if ((ret = poll(fds, 2, 0)) < 0) {
                    fprintf(stderr, "%s\n", strerror(errno));
                    exit(1);
                }
                
                // Event happened after calling poll.
                else if (ret > 0) {
                    // Only reads from a file descriptor if it has pending input.
                    // Case of network socket
                    if (fds[0].revents & POLLIN) {
                        int readByte = read(fds[0].fd, buf, BUF_SIZE);
                        if (readByte == -1) {
                            fprintf(stderr, "Failed to read\n");
                            fprintf(stderr, "%s\n", strerror(errno));
                            exit(1);
                        }
                        else {
                            // No compressed input from the network socket
                            if (!chkCompres) {
                                for (i = 0; i < readByte; i++) {
                                    // Read input from the keyboard, echo it to stdout,
                                    // and forward it to the shell.
                                    if (buf[i] == '\r' || buf[i] == '\n') {
                                        //write(STDOUT_FILENO, rn, sizeof(rn));
                                        write(fdPtoC[1], &rn[1], 1);
                                    }
                                    // When your program reads a ^C from the keyboard,
                                    // it should use kill to send a SIGINT to the shell process.
                                    else if (buf[i] == 0x03) {
                                        //write(STDOUT_FILENO, interrupt, 2);
                                        if (kill(childpid, SIGINT) != 0) {
                                            fprintf(stderr, "%s\n", strerror(errno));
                                            exit(1);
                                        }
                                    }
                                    // Upon receiving an EOF from the terminal,
                                    // close the pipe to the shell.
                                    else if (buf[i] == 0x04) {
                                        // write(STDOUT_FILENO, EOT, 2);
                                        close(fdPtoC[1]);
                                    }
                                    else {
                                        // each read from the keyboard is likely to return only one character.
                                        //write(STDOUT_FILENO, &buf[i], sizeof(char));
                                        write(fdPtoC[1], &buf[i], sizeof(char));
                                    }
                                }
                            }
                            // Compressed input from the network socket
                            // Decompress socket data and send to child shell's STDIN
                            else if (chkCompres) {

                                char outputByte[BUF_SIZE];
                                in_stream.avail_in = readByte;
                                in_stream.next_in = (unsigned char *)buf;
                                in_stream.avail_out = BUF_SIZE;
                                in_stream.next_out = (unsigned char *)outputByte;

                                // In this project, I should set the value of flush to Z_SYNC_FLUSH.
                                do {
                                    inflate(&in_stream, Z_SYNC_FLUSH);
                                } while (in_stream.avail_in > 0);

                                for (j = 0; j < (BUF_SIZE - in_stream.avail_out); j++) {
                                    if (outputByte[j] == '\r' || outputByte[j] == '\n')
                                        write(fdPtoC[1], &rn[1], sizeof(char));
                                    // When your program reads a ^C from the socket data,
                                    // it should use kill to send a SIGINT to the shell process.
                                    else if (outputByte[j] == 0x03) {
                                        if (kill(childpid, SIGINT) != 0) {
                                            fprintf(stderr, "%s\n", strerror(errno));
                                            exit(1);
                                        }
                                    }
                                    // close the pipe to the shell.
                                    else if (outputByte[j] == 0x04)
                                        close(fdPtoC[1]);
                                    else
                                        write(fdPtoC[1], &outputByte[j], sizeof(char));
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
                        int readByte = read(fds[1].fd, &readBuffer, sizeof(readBuffer));
                        if (readByte == -1) {
                            fprintf(stderr, "Failed to read\n");
                            fprintf(stderr, "%s\n", strerror(errno));
                            exit(1);
                        }
                        else {
                            for (i = 0; i < readByte; i++) {
                                if (readBuffer[i] == 0x04)
                                    flagEOF = 1;
                                else {
                                    if (!chkCompres)
                                        write(newSock, &readBuffer[i], sizeof(char));
                                    else if (chkCompres == 1) {
                                        char outputByte[BUF_SIZE];
                                        out_stream.avail_in = readByte;
                                        out_stream.next_in = (unsigned char *)readBuffer;
                                        out_stream.avail_out = BUF_SIZE;
                                        out_stream.next_out = (unsigned char *)outputByte;
                                        do {
                                            deflate(&out_stream, Z_SYNC_FLUSH);
                                        } while (out_stream.avail_in > 0);
                                        write (newSock, outputByte, BUF_SIZE - out_stream.avail_out);
                                    }
                                }
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
            childExit();
        }
        if (chkCompres) {
            deflateEnd(&out_stream);
            inflateEnd(&in_stream);
        }
    }
    exit(0);
}