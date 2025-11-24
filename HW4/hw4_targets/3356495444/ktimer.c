#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

void printManPage(void);
void sighandler(int);

int main(int argc, char **argv) {
	struct sigaction action;
	int feed;
	int pid = getpid();
	char line[256];
	int pFile, oflags;
	char tmp[50];
	char name[256];
	// int ii, count = 0;

	strcpy(name, __FILE__);

	// printf("NAME: %s\n", name);

	pFile = open("/dev/mytimer", O_RDWR);
	if (pFile < 0) {
		fputs("mytimer module isn't loaded\n",stderr);
		return -1;
	}
	// printf("This is %s() from %s, line %d\n",
        // __FUNCTION__, __FILE__, __LINE__);

	// Setup signal handler
	memset(&action, 0, sizeof(action));
	action.sa_handler = sighandler;
	action.sa_flags = SA_SIGINFO;
	sigemptyset(&action.sa_mask);
	sigaction(SIGIO, &action, NULL);
	fcntl(pFile, F_SETOWN, getpid());
	oflags = fcntl(pFile, F_GETFL);
	fcntl(pFile, F_SETFL, oflags | FASYNC);

	// list current timers
	if (argc == 2 && strcmp(argv[1], "-l") == 0) {
		// fputs("l", pFile); // [FLAG]
		// while ( read(pFile, line, sizeof(line)) != NULL) {
		// 	printf("%s\n", line);
		// }
		read(pFile, line, sizeof(line));
		printf("%s\n", line);

	}
	else if (argc == 4 && strcmp(argv[1], "-s") == 0) {
		strcpy(line, argv[3]); // [MSG]
		strcat(line, ";"); // delimeter
		strcat(line, argv[2]); // [SEC]
		strcat(line, ";s;"); // [FLAG]
		sprintf(tmp, "%d", pid);
		strcat(line, tmp);
		strcat(line, ";");
		strcat(line, name);
		strcat(line, ";");

		feed = write(pFile, line, sizeof(line));
		if (feed == 1) { // timer set
			pause();
			read(pFile, line, sizeof(line));
			printf("%s\n", argv[3]);
		} else if (feed == 2) {
			printf("Timer %s has now been reset to %s seconds!\n", argv[3], argv[2]);
		} else if (feed == 3) {
			printf("A timer exists already!\n");
		}
	}
  // reset timer
	else if (argc == 2 && strcmp(argv[1], "-r") == 0) {
		// printf("RESET statement\n");
		strcpy(line, "-1;-1;r;");  // [FLAG]
		// printf("line: %s\n", line);
		feed = write(pFile, line, sizeof(line)); // [FLAG]
	}
	// /////////////////////////////////////////////
	// /////////////////////////////////////////////
	// /////////////////////////////////////////////
	// // Otherwise invalid
	else {
		printManPage();
	}
	close(pFile);
	return 0;
}

void printManPage() {
	printf("Usage: ktimer [-l] [-s msg] [-m count]\n");
}

// SIGIO handler
void sighandler(int signo)
{
	// printf("Awaken!\n");
}
