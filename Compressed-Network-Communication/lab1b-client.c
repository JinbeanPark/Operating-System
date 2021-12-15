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
#include "zlib.h"
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/wait.h>
#include <sys/stat.h>

//#define PORT 8080
#define BUF_SIZE 2048

int flagEOF = 0;
z_stream out_stream; // Compress STDIN and send to socket
z_stream in_stream; // Decompress socket data and send to client's STDOUT

// Use this variable to remember original terminal attributes.
struct termios restoreTermios;

static struct option long_options[] = {
    {"port", required_argument, NULL, 'p'},
    {"log", required_argument, NULL, 'l'},
    {"compress", no_argument, NULL, 'c'},
    {0, 0, 0, 0}
};

// reset input mode before exiting
void resetInputMode() {
    tcsetattr(STDIN_FILENO, TCSANOW, &restoreTermios);
}

// The current terminal mode.
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

int main(int argc, char **argv) {

    struct sockaddr_in server_address;
    int sockfd = 0;
    int opt, logFd, chkPort, chkLog, chkCompres, ret, i;
    chkPort = chkLog = chkCompres = 0;
    int portNum = 0;
    char outputByte[BUF_SIZE];
	char buf[BUF_SIZE];
	char rn[2];
    rn[0] = '\r';
    rn[1] = '\n';
	char interrupt[2];
	char EOT[2];
    interrupt[0] = '^';
    interrupt[1] = 'C';
    EOT[0] = '^';
    EOT[1] = 'D';
    struct hostent* server;
    char *prefixSent = "SENT ";
    char *prefixReceived = "RECEIVED ";
    char *prefixBytes = " bytes: ";
    char newLine = '\n';
	

    opt = getopt_long(argc, argv, "", long_options, NULL);
    if (opt == -1)
        fprintf(stderr, "Options was not entered.\n");

    while (opt != -1) {
        switch (opt) {
        case 'p':
            portNum = atoi(optarg);
            chkPort = 1;
            break;
        case 'l':
            if ((logFd = creat(optarg, 0644)) == -1) {
                fprintf(stderr, "Failed to create the output file %s\n", optarg);
                fprintf(stderr, "%s\n", strerror(errno));
                exit(1);
            }
            chkLog = 1;
            break;
        case 'c':
            chkCompres = 1;
            out_stream.zalloc = Z_NULL;
            out_stream.zfree = Z_NULL;
            out_stream.opaque = Z_NULL;
	        in_stream.zalloc = Z_NULL;
	        in_stream.zfree = Z_NULL;
            in_stream.opaque = Z_NULL;
			if (inflateInit(&in_stream) != Z_OK) {
                fprintf(stderr, "Unable to create decompression stream\n");
                exit(1);
            }                        
	        // The constant Z_DEFAULT_COMPRESSION provides a good
            // balance between compression and spped.
            if (deflateInit(&out_stream, Z_DEFAULT_COMPRESSION) != Z_OK) {
                fprintf(stderr, "Unable to create compression stream\n");
                exit(1);
            }
            break;
        default:
            fprintf(stderr, "Entered options and arguments in wrong format.\n");
            fprintf(stderr, "Correct usage: --port='port number' --log='log file name' --compress\n");
            exit(1);
        }
        opt = getopt_long(argc, argv, "", long_options, 0);
    }
    
    if (!chkPort) {
        fprintf(stderr, "The port argument was not entered!\n");
        exit(1);
    }

    else if (chkPort) {
        
        // Modes should be set immediately on start-up (before any characters are entered)
        setInputMode();
        
        // 1. Create a socket
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            fprintf(stderr, "Failed to create a socket in client.\n");
            exit(1);
        }

        // 2. Identify server
        memset(&server_address, '0', sizeof(server_address));
        server_address.sin_family = AF_INET;
        if (!(server = gethostbyname("localhost"))) {
            fprintf(stderr, "Failed to get structure describing an internet host\n");
            exit(1);
        }
        //server_address.sin_addr.s_addr = htons(INADDR_ANY);
        server_address.sin_port = htons(portNum);
        bcopy((char *)server->h_addr, (char *)&server_address.sin_addr.s_addr, server->h_length);
        /*
        if (inet_pton(AF_INET, hostNode->h_addr, &server_address.sin_addr) <= 0) {
            fprintf(stderr, "Invalid address\n");
            exit(1);
        }*/

        // 3. Connect to server
        if (connect(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
            fprintf(stderr, "Failed to connect socket\n");
            exit(1);
        }

        // 4. Wait for input
        // 1) poll on keyboard and socket.
        // 2) Compress and decompress if necessary.

        // Create an array of two pollfd structures
        struct pollfd fds[2];
    
        // One describing the keyboard (stdin)
        fds[0].fd = STDIN_FILENO;
        fds[1].fd = sockfd;

        // Have both of these pollfds wait for either input (POLLIN) or
        // error(POLLHUP, POLLERR) events
        fds[0].events = POLLIN | POLLHUP | POLLERR;
        fds[1].events = POLLIN | POLLHUP | POLLERR;

        fds[0].revents = 0;
        fds[1].revents = 0;

        // Write a main loop that calls poll(2) (timeout = 0)
        while (!flagEOF) {
            // Error happened after calling poll
            if ((ret = poll(fds, 2, 0)) < 0) {
                fprintf(stderr, "%s\n", strerror(errno));
                exit(1);
            }
			
			// Event happened after calling poll.
            else if (ret > 0) {
                // Case 1: keyboard
                if (fds[0].revents & POLLIN) {
					int readByte = read(fds[0].fd, buf, BUF_SIZE);
                    if (readByte == -1) {
                        fprintf(stderr, "Failed to read\n");
                        fprintf(stderr, "%s\n", strerror(errno));
                        exit(1);
                    }
                    else {
                        for (i = 0; i < readByte; i++) {
                            if (buf[i] == 'r' || buf[i] == '\n')
                                write(STDOUT_FILENO, rn, sizeof(rn));
                            // When your program reads a ^C from the keyboard
                            else if (buf[i] == 0x03) {
                                write(STDOUT_FILENO, interrupt, 2);
                                //send(sockfd, interrupt, strlen(interrupt), 0);
                            }
                            // When your program reads a ^D from the keyboard
                            else if (buf[i] == 0x04) {
                                write(STDOUT_FILENO, EOT, 2);
                                //send(sockfd, EOT, strlen(EOT), 0);
                            }
                            else {
                                write(STDOUT_FILENO, (buf + i), 1);
                                //send(sockfd, &buf[i], sizeof(char), 0);
                            }
                        }
                        // No compressing
                        if (!chkCompres)
                            write(sockfd, buf, readByte);
                        // Compressing
                        else if (chkCompres) {
                            
                            out_stream.avail_in = readByte;
                            out_stream.next_in = (unsigned char *)buf;
                            out_stream.avail_out = BUF_SIZE;
                            out_stream.next_out = (unsigned char *)outputByte;
                            
                            // In this project, I should set the value of flush to Z_SYNC_FLUSH.
                            while (out_stream.avail_in > 0)
                                deflate(&out_stream, Z_SYNC_FLUSH);
                            
                            write(sockfd, outputByte, BUF_SIZE - out_stream.avail_out);
							if (!chkLog)
								memset(outputByte, 0, sizeof(char) * BUF_SIZE);
                        }
                        if (chkLog) {
                            char sizeByte[100];
                            write(logFd, prefixSent, strlen(prefixSent));
                            if (!chkCompres)
                                sprintf(sizeByte, "%d", readByte);
                            else if (chkCompres)
                                sprintf(sizeByte, "%d", BUF_SIZE - out_stream.avail_out);
                            write(logFd, sizeByte, strlen(sizeByte));
                            write(logFd, prefixBytes, strlen(prefixBytes));
                            if (!chkCompres)
                                write(logFd, buf, readByte);
                            else if (chkCompres) {
                                write(logFd, outputByte, BUF_SIZE - out_stream.avail_out);
								memset(outputByte, 0, sizeof(char) * BUF_SIZE);
							}
                            write(logFd, &newLine, sizeof(char));
                        }
                    }
                }
                else if (fds[0].revents & POLLERR) {
                    fprintf(stderr, "%s\n", "Error happened while receiving input from the keyboard\n");
                    flagEOF = 1;
                }
                else if (fds[0].revents & POLLHUP) {
                    fprintf(stderr, "%s\n", "Connecting was broken while receiving input from the keyboard\n");
                    flagEOF = 1;
                }

                // Case 2: TCP Socket
                if (fds[1].revents & POLLIN) {
                    // Read input from the server.
					int readByte = read(sockfd, buf, BUF_SIZE);
                    if (readByte == -1) {
                        fprintf(stderr, "Failed to read\n");
                        fprintf(stderr, "%s\n", strerror(errno));
                        exit(1);
                    }
                    else if (readByte == 0)
                		flagEOF = 1;
                    else {
                        // No compressed
                        if (!chkCompres)
                            write(STDOUT_FILENO, buf, readByte);
                        
                        // Compressed
                        else if (chkCompres) {
					
                            in_stream.avail_in = readByte;
                            in_stream.next_in = (unsigned char *)buf;
                            in_stream.avail_out = BUF_SIZE;
                            in_stream.next_out = (unsigned char *)outputByte;
                            
                            // In this project, I should set the value of flush to Z_SYNC_FLUSH.
                            do {
								inflate(&in_stream, Z_SYNC_FLUSH);
							} while (in_stream.avail_in > 0);
                                
                            write(STDOUT_FILENO, outputByte, BUF_SIZE - in_stream.avail_out);
							if (!chkLog)
								memset(outputByte, 0, sizeof(char) * BUF_SIZE);
                        }
                        if (chkLog) {
                            char sizeByte[100];
                            write(logFd, prefixReceived, strlen(prefixReceived));
                            sprintf(sizeByte, "%d", readByte);
                            write(logFd, sizeByte, strlen(sizeByte));
                            write(logFd, prefixBytes, strlen(prefixBytes));
                            write(logFd, buf, readByte);
                            memset(outputByte, 0, sizeof(char) * BUF_SIZE);
                            write(logFd, &newLine, sizeof(char));
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
    }
    // 5. Shutdown() socket and log file descriptor
    if (shutdown(sockfd, SHUT_RDWR) < 0) {
        fprintf(stderr, "Failed to shutdown!\n");
        exit(1);
    }
    close(logFd);
    if (chkCompres) {
        deflateEnd(&out_stream);
        inflateEnd(&in_stream);
    }
    exit(0);
}