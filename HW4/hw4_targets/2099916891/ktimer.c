#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

void sighandler(int);
void printManPage(void);

int main(int argc, char **argv) {
	char line[2560];
	int count = 1, i = 0;
	char *flag;
	char *msg;
	char *sec;
	char *limiter = ";";
	char *cp_line;

	int pFile, oflags;
	struct sigaction action, oa;
	int ii;

	// Opens to device file
	pFile = open("/dev/mytimer", O_RDWR);
	if (pFile < 0) {
		fprintf (stderr, "fasync_example module isn't loaded\n");
		return 1;
	}
	// Setup signal handler
	memset(&action, 0, sizeof(action));
	action.sa_handler = sighandler;
	action.sa_flags = SA_SIGINFO;
	sigemptyset(&action.sa_mask);
	sigaction(SIGIO, &action, NULL);
	fcntl(pFile, F_SETOWN, getpid());
	oflags = fcntl(pFile, F_GETFL);
	fcntl(pFile, F_SETFL, oflags | FASYNC);

	// Check if list time
	if (argc == 2 && strcmp(argv[1], "-l") == 0) {
		if ((read(pFile, line, sizeof(line))) != -1) {
			printf("%s", line);
		}
	}

	// Check if register new timer
	else if (argc == 4 && strcmp(argv[1], "-s") == 0) {
		if (strtol(argv[2], NULL, 10) < 0) {
			printf("Time cannot be negative\n");
			close(pFile);
			return -1;
		}
		strcpy(line, argv[1]); // format is -s;hello;20
		strcat(line, ";");
		strcat(line, argv[3]);
		strcat(line, ";");
		strcat(line, argv[2]);
		if (write(pFile, line, sizeof(line)) == 6969) { // check if timer needs to be updated
			printf("Timer %s has now been reset to %lu seconds!\n", argv[3], strtol(argv[2], NULL, 10));
		}
		pause(); // pause and wait for timer
		printf("%s\n", argv[3]);
	}

	// Check if removing timer
	else if (argc == 2 && strcmp(argv[1], "-r") == 0) {
		strcpy(line, argv[1]); // store -r
		strcat(line, ";");
		write(pFile, line, sizeof(line));
		printf("deleting timer\n");
	}

	// if invalid
	else {
		printManPage();
	}

	close(pFile);
	return 0;
}

// SIGIO handler
void sighandler(int signo)
{
	//send signal
}

void printManPage() {
	printf("Error: invalid use.\n");
	printf(" ktimer [-flag] [message]\n");
	printf(" -l: List to stdout expiration time in seconds and message\n");
	printf(" -s [SEC] [MSG]: Start timer after [SEC] will print [MSG]\n");
	printf("                 If active timer exists, reset to [SEC]\n");
	printf(" -r: Removes all timers\n");
}
