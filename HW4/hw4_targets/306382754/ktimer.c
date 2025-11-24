
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/******************************************************

  Usage:
    ./ktimer [flag] [message]

	-l (list)
    List to stdout expiration time in seconds and message.
	-s [SEC] [MSG]
    Register new timer after [SEC] seconds will print
    message [MSG]. If active timer with same message
    exists, reset timer to [SEC].
  -m [COUNT]
    Change number of active timers to [COUNT]. Default
    is 1 on module load.

  Examples:
	./ktimer -l
		Print whatever message that the ktimer module is holding

	./ktimer -w ThisIsAMessage
		Write the string "ThisIsAMessage" to the ktimer module

******************************************************/

void printManPage(void);

int main(int argc, char **argv) {
	char line[2560];
	int count = 1, i = 0;
	char *flag;
	char *msg;
	char *sec;
	char *limiter = ";";
	char *cp_line;

	/* Check to see if the ktimer successfully has mknod run
	   Assumes that mytimer is tied to /dev/mytimer */
	FILE * pFile;
	pFile = fopen("/dev/mytimer", "r+");
	if (pFile==NULL) {
		fputs("mytimer module isn't loaded\n",stderr);
		return -1;
	}
	// Check if list time
	if (argc == 2 && strcmp(argv[1], "-l") == 0) {
		while ((fgets(line, sizeof(line), pFile)) != NULL) {
			printf("%s", line);
		}
	}

	// Check if register new timer
	else if (argc == 4 && strcmp(argv[1], "-s") == 0) {
		if (strtol(argv[2], NULL, 10) < 0) {
			printf("Time cannot be negative\n");
			fclose(pFile);
			return -1;
		}
		strcpy(line, argv[1]); // -s;hello;20
		strcat(line, ";");
		strcat(line, argv[3]);
		strcat(line, ";");
		strcat(line, argv[2]);
		fputs(line, pFile);
	}

	// Check if multiple timer
	else if (argc == 3 && strcmp(argv[1], "-m") == 0) {
		count = strtol(argv[2], NULL, 10);
		if (count < 1) {
			printf("Count cannot be less than 1\n");
			fclose(pFile);
			return -1;
		}
		strcpy(line, argv[1]); // store -m
		strcat(line, ";");
		strcat(line, argv[2]); // store count
		fputs(line, pFile);

	}

	// Otherwise invalid
	else {
		printManPage();
	}

	fclose(pFile);
	return 0;
}

void printManPage() {
	printf("Error: invalid use.\n");
	printf(" ktimer [-flag] [message]\n");
	printf(" -l: List to stdout expiration time in seconds and message\n");
	printf(" -s [SEC] [MSG]: Start timer after [SEC] will print [MSG]\n");
	printf("                 If active timer exists, reset to [SEC]\n");
}
