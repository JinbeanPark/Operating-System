/*
NAME: Jinbean Park
EMAIL: keiserssose@gmail.com
ID: 805330751
*/

#include <stdio.h>
#include <mraa.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <poll.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>


static volatile int run_flag = 1;
mraa_aio_context temperSensor;
const int B = 4275;
const int R0 = 100000;;
char tempDegree = 'F';
int chkLog = 0;
int chkOff = 0;
char fileName[100];
int cntrlReport = 1;
int period = 1;
FILE *fp;
char *hostName;
int portNum = 0;
char *idNum = "";

#define BUF_SIZE 400

static struct option long_options[] = {
	
	{"period", required_argument, NULL, 'p'},
	{"scale", required_argument, NULL, 's'},
	{"log", required_argument, NULL, 'l'},
	// Accepts the following new parameters
	{"id", required_argument, NULL, 'i'},
	{"host", required_argument, NULL, 'h'},
	{0, 0, 0, 0}
};


// Use the AI0 functions of the MRAA library to get reading from your temperature sensor
float getTemp() {
		
	int a = mraa_aio_read(temperSensor);
	
	float R = 1023.0 / a - 1.0;
	R = R0 * R;
	
	// Converts the sensor value into a temperature.
	float temperature = 1.0 / (log(R / R0) / B + 1 / 298.15) -273.15;
	
	// By default, temperatures should be reported in degrees Farenheit, but this can be controlled.	
	if (tempDegree == 'F')
		return (temperature * 1.8 + 32);
	return temperature;	
}


void reportSample(int sockFd) {


	// Time of the sample (e.g. 17:25:58) in the local timezone
	time_t timeInfo;
	struct tm *current;
	time(&timeInfo);	

	current = localtime(&timeInfo);

	// Single blank/space
	// A decimal temperature in degrees and tenths (e.g. 98.6), a newline char(\n)
	char reportLine[BUF_SIZE];
	if (run_flag == 1) {
		float tmp = getTemp();
		sprintf(reportLine, "%02d:%02d:%02d %.1f\n", current->tm_hour, current->tm_min, current->tm_sec, tmp);
	}
	
	// Outputs (and longs) a final sample with the time and tring SHUTDOWN (instead of a temperature)
	// 3.as before, send (and log) newline terminated temperature reports over the connection.
	else if (run_flag == 0) {
		sprintf(reportLine, "%02d:%02d:%02d %s\n", current->tm_hour, current->tm_min, current->tm_sec, "SHUTDOWN");
		write(sockFd, reportLine, strlen(reportLine));
		if (chkLog == 1) {
			if ((fp = fopen(fileName, "a+")) == NULL) {
				fprintf(stderr, "Failed to open the file\n");
				exit(1);
			}
			if (chkOff)
				fputs("OFF\n", fp);
			fputs(reportLine, fp);
			fclose(fp);
			exit(0);
		}
	}

	// Writes that report to the sockFd
	if (cntrlReport && run_flag)
		write(sockFd, reportLine, strlen(reportLine));

	// Appends that report to a logfile if that logging has been enabled with an option.
	if (chkLog && cntrlReport && run_flag) {
		if ((fp =  fopen(fileName, "a+")) == NULL) {
			fprintf(stderr, "Failed to open the file\n");
			exit(1);
		}
		fputs(reportLine, fp);
		fclose(fp);
	}
}


void commandStdin(char *stdinLine, int sockFd) {
	

	int chkValidArg = 0;	
	char *hadPeriod = strstr(stdinLine, "PERIOD=");
	char *hadLog = strstr(stdinLine, "LOG");
	if (hadPeriod != NULL) {
		int index = 0;
		char periodVal[10];
		char *startPos = hadPeriod + 7;
		while (*startPos != '\0') {
			periodVal[index++] = *startPos;
			startPos = startPos + 1;
		}
		period = atoi(periodVal);
		chkValidArg = 1;
	}
	
	else if (hadLog != NULL) {
		chkValidArg = 1;
	}
	
	else if (strcmp(stdinLine, "SCALE=F") == 0) {
		tempDegree = 'F';
		chkValidArg = 1;
	}
	
	else if (strcmp(stdinLine, "SCALE=C") == 0) {
		tempDegree = 'C';
		chkValidArg = 1;
	}
	
	else if (strcmp(stdinLine, "STOP") == 0) {
		cntrlReport = 0;
		chkValidArg = 1;
		printf("\nEntered in STOP\n");
	}

	else if (strcmp(stdinLine, "START") == 0) {
		cntrlReport = 1;
		chkValidArg = 1;
	}

	else if (strcmp(stdinLine, "OFF") == 0) {
		chkOff = 1;
		run_flag = 0;
		reportSample(sockFd);
	}
	if (chkLog == 1 && chkValidArg == 1) {
		if ((fp = fopen(fileName, "a+")) == NULL) {
			fprintf(stderr, "Failed to open the file\n");
			exit(1);
		}
		stdinLine[strlen(stdinLine)] = '\n';
		stdinLine[strlen(stdinLine) + 1] = '\0';
		fputs(stdinLine, fp);
		fclose(fp);
	}
}


int main(int argc, char **argv) {

	int ret;
	
	int opt = getopt_long(argc, argv, "", long_options, NULL);
	if (opt == -1)
		fprintf(stderr, "Options was not entered.\n");

	while (opt != -1) {
	
		switch(opt) {
			// samples a temperature sensor at a configurable rate		
			case 'p':
				period = atoi(optarg);
			break;
			case 's':
				if (optarg[0] == 'C')
					tempDegree = 'C';
			break;
			case 'l':
				chkLog = 1;
				strcpy(fileName, optarg);
			break;
			case 'h':
				hostName = optarg;
			break;
			case 'i':
				idNum = optarg;
			break;
			default:
				fprintf(stderr, "Enter the options and arguments in correct format.\n");
				exit(1);				
		}
		opt = getopt_long(argc, argv, "", long_options, 0);
	}
	
	// Get port number.		
	if ((portNum = atoi(argv[optind])) < 0) {
		fprintf(stderr, "port number is wrong\n");
		exit(1);
	}
	
	// Check the length of ID number
	if (strlen(idNum) != 9) {
		fprintf(stderr, "The length of ID number is wrong\n");
		exit(1);
	}

	// Check if the option of log was entered or not
	if (chkLog == 0) {
		fprintf(stderr, "Enter the log option\n");
		exit(1);
	}
		
	// Use the AIO functions of the MRAA library to get reading from your temperature sensor
	temperSensor = mraa_aio_init(1);
    	
	// 1. Open a TCP connection to the server at the specified address and port
	// (1) Create a socket with the socket() system call
	int sockFd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockFd < 0) {
		fprintf(stderr, "Failed to create a socket\n");
		exit(1);
	}
	// (2) Connect the socket to the address of the server using the connect() system call
	// Need two pieces of information that address of the server and port that the server process is listening to connect.
	struct hostent *serverInfo;
	if (!(serverInfo = gethostbyname(hostName))) {
		fprintf(stderr, "Failed to find host\n");
		exit(1);
	}
	struct sockaddr_in server_address;
	memset(&server_address, '0', sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(portNum);
	bcopy((char *)serverInfo->h_addr, (char *)&server_address.sin_addr.s_addr, serverInfo->h_length);
	if (connect(sockFd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
		fprintf(stderr, "Failed to connect to server\n");
		exit(1);
	}

	// 2. Immediately send (and log) an ID terminated with a newline: ID=ID-number
	char idNumber[30];
	sprintf(idNumber, "ID=%s\n", idNum);
	if (write(sockFd, idNumber, strlen(idNumber)) < 0) {
		fprintf(stderr, "Failed to send an ID\n");
		exit(1);
	}
	if ((fp = fopen(fileName, "a+")) == NULL) {
		fprintf(stderr, "Failed to open the file\n");
		exit(1);
	}
	fputs(idNumber, fp);
	fclose(fp);
	

	struct pollfd pollSockFd;
	pollSockFd.fd = sockFd;
	pollSockFd.events = POLLIN;
	pollSockFd.revents = 0;	
	
	
	// Use the GPIO functions of the MRAA library.
	while (run_flag) {
		
		// 4. as before, process (and log) newline_terminated commands received over the connection.
		if ((ret = poll(&pollSockFd, 1, 0)) < 0) {
			fprintf(stderr, "%s\n", strerror(errno));
			exit(1);
		}
		else if (ret > 0) {
			if (pollSockFd.revents & POLLIN) {
				char inputLine[BUF_SIZE];
				int readByte = read(sockFd, inputLine, BUF_SIZE);
				int i,j = 0;
				char commandStr[BUF_SIZE];
				for (i = 0; i < readByte; i++ ) {
					if (inputLine[i] == '\n') {
						commandStr[i] = '\0';
						commandStdin(commandStr, sockFd);
						memset(commandStr, 0, BUF_SIZE);
						j = 0;
					}
					else
						commandStr[j++] = inputLine[i];
				}
			}
		}
		reportSample(sockFd);
		if (run_flag)
			sleep(period);
	}
	mraa_aio_close(temperSensor);
	
	return 0;
}
