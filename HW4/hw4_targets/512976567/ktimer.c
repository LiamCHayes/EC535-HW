#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/syscall.h>

/******************************************************

  Usage:
    ./ktimer [flag] [message]

	-l (list)
    List to stdout expiration time in seconds and message.
	-s [SEC] [MSG]
    Register new timer after [SEC] seconds will print
    message [MSG]. If active timer with same message
    exists, reset timer to [SEC].
    -r
    remove all timers

  Examples:
	./ktimer -l
		Print whatever message that the ktimer module is holding

	./ktimer -s 50 ThisIsAMessage
		Write the string "ThisIsAMessage" to the ktimer module

******************************************************/

void printManPage(void);
void sighandler(int);
char *flag;
char *msg;
char *sec;

int main(int argc, char **argv) {
	char line[2560];
	char line_2[2560];
	int count = 1, i = 0;
	char *limiter = ";";
	char *cp_line;
	char* message;
    char *flag_start;
	int pFile, oflags;
    struct sigaction action, oa;
    int ii; //count = 0;

	/* Check to see if the ktimer successfully has mknod run
	   Assumes that mytimer is tied to /dev/mytimer */
	pFile = open("/dev/mytimer", O_RDWR);
	if (pFile==NULL) {
		write(stderr, "mytimer module isn't loaded\n", 256);
		return -1;
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
		while (read(pFile, line, sizeof(line))){
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
		strcpy(line, argv[1]); // -s;hello;20
        strcat(line, ";");
        strcat(line, argv[3]);
        strcpy(&msg, (const char *) argv[3]);
        strcat(line, ";");
        strcat(line, argv[2]);
        write(pFile, line, strlen(line));
        pause();
	}

	// Check if remove timer
	else if (argc == 2 && strcmp(argv[1], "-r") == 0) {
        pFile = open("/dev/mytimer", O_RDWR);
        strcpy(line, argv[1]);
        strcat(line, ";");
        write(pFile, line, strlen(line));
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
	printf(" -r: remove and registered timers on the system, along with offset(s) and other relevant timer info\n");
	printf("                 If active timer exists, reset to [SEC]\n");
}

// SIGIO handler
void sighandler(int signo) {
    printf("%s\n", &msg);
}






