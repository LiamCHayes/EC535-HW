#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

// Check for message duplication
static char msg[128], *mptr = msg;

void sighandler(int);

int main(int argc, char *argv[]) {
	int pFile, oflags;
	struct sigaction action, oa;
	int ii, count = 0;

	//For file write
	char string[144];

	// Opens to device file
	pFile = open("/dev/mytimer", O_RDWR);
	if (pFile < 0) {
		fprintf (stderr, "mytimer module isn't loaded\n");
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

	//Read the parameters.
	if(strcmp(argv[1], "-s") == 0 && argc == 4)
	{

		sprintf(string, "s;%s;%s", argv[2], argv[3]);
		//printf("Userland Write to Kernel: %s\n", string);
		write(pFile, string, 144);
		if(strcmp(argv[3], mptr) == 0)
		{
			printf("Timer %s has now been reset to %s seconds!", argv[3], argv[2]);
		}
		else
		{
			strcpy(mptr, argv[3]);
		}

		// Waits.
		printf("Sleep!\n");
		pause();

		printf("%s\n", argv[3]);
		//printf("Closing out!\n");
		// Closes.

		close(pFile);
		return 0;
	}
	else if(strcmp(argv[1], "-r") == 0 && argc == 2)
	{
		//printf("Userland Write to Kernel: r\n");
		write(pFile, "r", 144);
		memset(msg, 0, sizeof(msg));
		close(pFile);
		return 0;
	}
	else if(strcmp(argv[1], "-l") == 0 && argc == 2)
	{
		//printf("Userland Write to Kernel: l\n");
		write(pFile, "l", 144);
		close(pFile);
		return 0;		
	}
	else	//Bad input format
	{
		printf("Bad input format.\n");
		close(pFile);
		return 1;
	}

	// Write to file.
	//write(pFile, "s;3", 144);

	

	
}

// SIGIO handler
void sighandler(int signo)
{
	memset(msg, 0, sizeof(msg));
}
