#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

void sighandler(int signo, siginfo_t *info, void *ucontext);
static char *message;

int main(int argc, char **argv) {
	int pFile, oflags;
	struct sigaction action;

	// Opens to device file
	pFile = open("/dev/mytimer", O_RDWR);
	if (pFile < 0) {
		fprintf (stderr, "mytimer module isn't loaded\n");
		return 1;
	}

	// Setup signal handler
	memset(&action, 0, sizeof(action));
	action.sa_sigaction = sighandler;
	action.sa_flags = SA_SIGINFO;
	sigemptyset(&action.sa_mask);
	sigaction(SIGIO, &action, NULL);
	fcntl(pFile, F_SETOWN, getpid());
	oflags = fcntl(pFile, F_GETFL);
	fcntl(pFile, F_SETFL, oflags | FASYNC);

	// if -l, print out the required info
	if (argc == 2 && strcmp(argv[1], "-l") == 0) {
		char buf[256];
		read(pFile, buf, sizeof(buf));
		printf("%s\n", buf);
	}
	
	// if starting a timer, store message and write to the file
	if (argc == 4 && strcmp(argv[1], "-s") == 0) {
		message = argv[3];

		// Write to the device to store the data
		char flag[2] = "-w";
		char input0[132];
		sprintf(input0, "%s %s", flag, argv[3]);
		write(pFile, input0, strlen(input0)+1);

		// Write to the device to start the timer
		char input[128];
		sprintf(input, "%s %s", argv[1], argv[2]);
		ssize_t count = write(pFile, input, strlen(input)+1);
		if (count == -1) {
			if (errno == EINVAL) { 
				printf("Cannot add another timer!\n");
			} else if (errno == EIO) {
				printf("The timer %s was updated!\n", message);
			}
		} else {
			pause();
		}
	}

	// if -r, remove all timers
	if (argc == 2 && strcmp(argv[1], "-r") == 0) {
		write(pFile, argv[1], strlen(argv[1])+1);
		message = NULL;
	}

	// Sleep until we get the sigio signal
	close(pFile);
	return 0;
}

void sighandler(int signo, siginfo_t *info, void *ucontext)
{
	if (info->si_int == 0 && message) {
		printf("%s\n", message);
	}
}
