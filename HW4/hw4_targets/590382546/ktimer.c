#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>


void printManPage(void);
void sighandler(int);
char *msg;

int main(int argc, char **argv) {
	char line[2560];
	int count = 1, i = 0;
	char *flag;
	
	char *sec;
	char *limiter = ";";
	char *cp_line;
	int duration;
	struct sigaction action, oa;

	/* Check to see if the ktimer successfully has mknod run
	   Assumes that mytimer is tied to /dev/mytimer */
	int pFile, oflags;
	pFile = open("/dev/mytimer", O_RDWR);
	if (pFile<0) {
		fputs("mytimer module isn't loaded\n",stderr);
		return -1;
	}
	
	// fasync functions
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
		while ((read(pFile, line, sizeof(line))) != 0) { 
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
		msg = argv[3];
		strcpy(line, argv[1]); // -s;hello;20
		strcat(line, ";");
		strcat(line, argv[3]);
		strcat(line, ";");
		strcat(line, argv[2]);
		//duration = (long)argv[2];
		sscanf (argv[2], "%d", &duration);
		if (write(pFile, line, strlen(line)) == 27) {
			printf("Timer %s has now been reset to %d seconds!\n", argv[3], duration);
		}
		
		pause();
		
	}
	// sends command to write, then kills ktimer process
	else if (argc == 2 && strcmp(argv[1], "-r") == 0) {
		strcpy(line, argv[1]);
		strcat(line, ";");
		write(pFile, line, 4);
		system("killall ktimer");
	}

	// Otherwise invalid
	else {
		printManPage();
	}

	close(pFile);
	return 0;
}

void printManPage() {
	printf("Error: invalid use.\n");
	printf(" ktimer [-flag] [message]\n");
	printf(" -l: List to stdout expiration time in seconds and message\n");
	printf(" -s [SEC] [MSG]: Start timer after [SEC] will print [MSG]\n");
	printf("                 If active timer exists, reset to [SEC]\n");
	printf(" -r: remove any active timer\n");
}

// Prints message once signal from km is received
void sighandler(int signo)
{
	printf("%s\n", msg);
}
