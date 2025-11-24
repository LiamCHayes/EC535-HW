#define _GNU_SOURCE

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_SIZE 128
void sighandler(int);

int main(int argc, char **argv) {
	char line[LINE_SIZE] = "";
	char result[LINE_SIZE] = "";
	char timer_exists[LINE_SIZE] = "";
	int count = 1, i = 0;
	struct sigaction action, oa;
	int pFile, dFile, oflags;

	pFile = open("/proc/mytimer", O_RDWR);
	if (pFile < 0) {
		write (2, "mytimer module isn't loaded\n", sizeof(line));
		return 1;
	}
	dFile = open("/dev/mytimer", O_RDWR);
	if (dFile < 0) {
		write (2, "mytimer module isn't loaded\n", sizeof(line));
		close(pFile);
		return 1;
	}

	// list time
	if (argc == 2 && strcmp(argv[1], "-l") == 0) {
		char *c = (char *) calloc(1, LINE_SIZE);
		memset(c, '\0', sizeof(char)*sizeof(line));
		int sz = read(dFile, c, LINE_SIZE); 
		if (sz < 0) {
			exit(0);
		} else {
			write (1, c, LINE_SIZE);
			write (1, "\n", 2);
		}
		close(dFile);
	}

	// register new timer
	else if (argc == 4 && strcmp(argv[1], "-s") == 0) {
		if (strtol(argv[2], NULL, 10) < 0) {
			write (1, "Time cannot be negative\n", 32);
			close(pFile);
			close(dFile);
			return 1;
		}
		
		strcpy(line, argv[1]); // -s;hello;20;
		strcat(line, ";");
		strcat(line, argv[3]);
		strcat(line, ";");
		strcat(line, argv[2]);

		// Setup signal handler
		memset(&action, 0, sizeof(action));
		action.sa_handler = sighandler;
		action.sa_flags = SA_SIGINFO;

		sigemptyset(&action.sa_mask);
		sigaction(SIGIO, &action, NULL);

		// Request asynchronous notification - can only interact with dev file!
		fcntl(dFile, F_SETOWN, getpid());
		oflags = fcntl(dFile, F_GETFL);
		fcntl(dFile, F_SETFL, oflags | FASYNC);

		// Set timer
		write(dFile, line, sizeof(line));
		
		// Wait for signal
		pause();
		strcpy(result, argv[3]);
		strcat(result, "\n");
		write (1, result, sizeof(result));
	}

	else if (argc == 2 && strcmp(argv[1], "-r") == 0) {
		strcpy(line, argv[1]);
		strcat(line, ";"); // -r;
		write (dFile, line, sizeof(line));
		write (1, "Removed\n", 12);
	}
	
	// Otherwise invalid
	else {
		write (2,"Error: invalid use.\nktimer [-flag] [message]\n -l: List to stdout expiration time in seconds and message\n -s: [SEC] [MSG]: Start timer after [SEC] will print [MSG]\n  \tIf active timer exists, reset to [SEC]\n -r: Remove any timer(s) registered on the system\n", 256);
	}
	close(pFile);
	close(dFile);
	return 0;
}

// SIGIO handler
void sighandler(int signo) {

}

