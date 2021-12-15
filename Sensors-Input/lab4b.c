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


static volatile int run_flag = 1;
mraa_aio_context temperSensor;
mraa_gpio_context Button;
const int B = 4275;
const int R0 = 100000;;
char tempDegree = 'F';
int chkLog = 0;
int chkOff = 0;
char fileName[100];
int cntrlReport = 1;
int period = 1;
FILE *fp;

#define BUF_SIZE 400

static struct option long_options[] = {
	
	{"period", required_argument, NULL, 'p'},
	{"scale", required_argument, NULL, 's'},
	{"log", required_argument, NULL, 'l'},
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


void reportSample() {
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
	else if (run_flag == 0) {
		sprintf(reportLine, "%02d:%02d:%02d %s\n", current->tm_hour, current->tm_min, current->tm_sec, "SHUTDOWN");
		write(1, reportLine, strlen(reportLine));
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

	// Writes that report to the stdout (fd 1)
	if (cntrlReport && run_flag)
		write(1, reportLine, strlen(reportLine));
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


void do_when_interrupted() {
	run_flag = 0;
	reportSample();
	exit(0);
}


void commandStdin(char *stdinLine) {
	
	
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
	}

	//char *chkLog = strstr(stdinLine, "LOG");
	else if (hadLog != NULL) {

	}
	
	else if (strcmp(stdinLine, "SCALE=F") == 0)
		tempDegree = 'F';
	
	else if (strcmp(stdinLine, "SCALE=C") == 0)
		tempDegree = 'C';
	
	else if (strcmp(stdinLine, "STOP") == 0)
		cntrlReport = 0;

	else if (strcmp(stdinLine, "START") == 0)
		cntrlReport = 1;

	else if (strcmp(stdinLine, "OFF") == 0) {
		chkOff = 1;
		run_flag = 0;
		reportSample();
	}
	if (chkLog == 1) {
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
			default:
				fprintf(stderr, "Enter the options and arguments in correct format.\n");
				exit(1);				
		}
		opt = getopt_long(argc, argv, "", long_options, 0);
	}
		

	// Use the AIO functions of the MRAA library to get reading from your temperature sensor
	temperSensor = mraa_aio_init(1);
	Button = mraa_gpio_init(60);
	
	// configure button GPIO interface to be an input pin.
	mraa_gpio_dir(Button, MRAA_GPIO_IN);
	// when the button is pressed, call do_when_interrupted.
	mraa_gpio_isr(Button, MRAA_GPIO_EDGE_RISING, &do_when_interrupted, NULL);
	
	signal(SIGINT, do_when_interrupted);
	
	struct pollfd stdinFd;
	stdinFd.fd = STDIN_FILENO;
	stdinFd.events = POLLIN;
	stdinFd.revents = 0;	

	// Use the GPIO functions of the MRAA library to samples the state of the button and when it is pushed.
	while (run_flag) {
		reportSample();
		if ((ret = poll(&stdinFd, 1, 0)) < 0) {
			fprintf(stderr, "%s\n", strerror(errno));
			exit(1);
		}
		else if (ret > 0) {
			if (stdinFd.revents & POLLIN) {
				char inputLine[BUF_SIZE];
				fgets(inputLine, BUF_SIZE, stdin);
				inputLine[strlen(inputLine) - 1] = '\0';
				commandStdin(inputLine);				
			}
		}
		if (run_flag)
			sleep(period);
	}
	
	mraa_aio_close(temperSensor);
	mraa_gpio_close(Button);

	return 0;
}
