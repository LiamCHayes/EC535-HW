#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

#define LINE_LENGTH 256
#define MSG_LENGTH 129

char* timerMessage;

void sighandler(int);

void printManPage(void);

// [-l] list all timers from /dev/mytimer
void listTimers(int devFile) {

	char line[LINE_LENGTH];
	int i = 0;

	if(read(devFile, line, LINE_LENGTH) == 0) {
		printf("");
	} else {
		while (line[i] != ';') {
				i++;
		}
		line[i] = ' ';
		printf("%s", line);
	}
}

// [-s] make/update timer
void updateTimer(int devFile, char* seconds, char* message) {
	char line[LINE_LENGTH];
	char* msg;
	char newTimer[LINE_LENGTH];
	long numInput;

	// check if timer delay is valid
	sscanf(seconds, "%ld", &numInput);
	if (numInput < 0) {
		printf("Negative timer delays not allowed.\n");
		exit(-1);
	}

	// format and write to the /dev/ file
	sprintf(newTimer, "%s;%s\n\0", message, seconds);
	write(devFile, newTimer, strlen(newTimer));
}

int main(int argc, char **argv) {
	char line[LINE_LENGTH];
	char msg[MSG_LENGTH];
	int msgIndex = 0;
	int pFile, oflags;
	struct sigaction action, oa;
	mode_t mode = S_IRUSR | S_IWUSR;
	int devFile;
	
	/* Check to see if the ktimer successfully has mknod run
	   Assumes that ktimer is tied to /dev/mytimer */
	devFile = open("/dev/mytimer", O_RDWR | O_TRUNC, mode);
	if (devFile < 0) {
		printf("ktimer module isn't loaded\n");
		exit(-1);
	}

	// Check if in read mode
	if (argc == 2 && strcmp(argv[1], "-l") == 0) {
		listTimers(devFile);
	}

	// Check if in write mode
	else if (argc == 4 && strcmp(argv[1], "-s") == 0) {

		// conditions for async
		memset(&action, 0, sizeof(action));
		action.sa_handler = sighandler;
		action.sa_flags = SA_SIGINFO;
		sigemptyset(&action.sa_mask);
		sigaction(SIGIO, &action, NULL);
		fcntl(devFile, F_SETOWN, getpid());
		oflags = fcntl(devFile, F_GETFL);
		fcntl(devFile, F_SETFL, oflags | FASYNC);

		timerMessage = argv[3];

		if (read(devFile, line, LINE_LENGTH) != 0) {
			while (line[msgIndex] != ';') {
				msg[msgIndex] = line[msgIndex];
				msgIndex++;
			}
			msg[msgIndex] = '\0';

			if (strcmp(msg, argv[3]) == 0) {
				updateTimer(devFile, argv[2], argv[3]);
				printf("Timer %s has now been reset to %s seconds!\n", argv[3], argv[2]);
			} else {
				printf("A timer exists already!\n");
			}
		} else {
			updateTimer(devFile, argv[2], argv[3]);
			pause();
		}
	} else if (argc == 2 && strcmp(argv[1], "-r") == 0) {
		// make sure /dev/ contains nothing and kill all ktimers
		close(devFile);
		devFile = open("/dev/mytimer", O_RDWR | O_TRUNC, mode);
		write(devFile, "\0", strlen("\0"));
		system("killall ktimer");
	} else {
		printManPage();
	}

	close(devFile);
	return 0;
}

void sighandler(int signo) {
	printf("%s\n", timerMessage);
}

void printManPage() {
	printf(" Error: invalid use.\n");
	printf(" ktimer_ul -s [time] [message]\n");
	printf(" ktimer_ul -l\n");
	printf(" ktimer_ul -r\n");
	printf(" -l: print out the current timer.\n");	
	printf(" -s: make a new timer.");
	printf(" -r: remove all timers.");
}

