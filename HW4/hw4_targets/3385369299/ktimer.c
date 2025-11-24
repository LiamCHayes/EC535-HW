#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/******************************************************
  Prepared by Matthew Yee

  Usage:
    ./mytimer_ul [flag] [message]

	-r (read)
	-w (write)

  Examples:
	./mytimer_ul -r
		Print whatever message that the mytimer module is holding

	./mytimer_ul -w ThisIsAMessage
		Write the string "ThisIsAMessage" to the mytimer module

******************************************************/

void printManPage(void);



int main(int argc, char **argv) {
	char line[256];
	int ii, count = 0;

	/* Check to see if the mytimer successfully has mknod run
	   Assumes that mytimer is tied to /dev/mytimer */
	FILE * pFile;
	pFile = fopen("/dev/mytimer", "r+");
	if (pFile==NULL) {
		fputs("mytimer module isn't loaded\n",stderr);
		return -1;
	}

	// list current timers
	if (argc == 2 && strcmp(argv[1], "-l") == 0) {
		// fputs("l", pFile); // [FLAG]
		while (fgets(line, 256, pFile) != NULL) {
			printf("%s\n", line);
		}
	}
	/////////////////////////////////////////////
	/////////////////////////////////////////////
	/////////////////////////////////////////////
	// set timer
	else if (argc == 4 && strcmp(argv[1], "-s") == 0) {
		// "HELLO";23;s
		fputs(argv[3], pFile); // [MSG]
		fputs(";", pFile); // delimeter
		fputs(argv[2], pFile); // [SEC]
		int feed = fputs(";s;", pFile); // [FLAG]
		// printf("FPUT returned: %d\n", feed);

	}
  // change max number of timers (default=1)
	else if (argc == 3 && strcmp(argv[1], "-m") == 0) {
		fputs("max timers;", pFile); // delimeter
		fputs(argv[2], pFile); // [COUNT]
		fputs(";m;", pFile); // [FLAG]
	}
	/////////////////////////////////////////////
	/////////////////////////////////////////////
	/////////////////////////////////////////////
	// Otherwise invalid
	else {
		printManPage();
	}
	fclose(pFile);
	return 0;
}

void printManPage() {
	printf("Usage: ktimer [-l] [-s msg] [-m count]\n");
}
