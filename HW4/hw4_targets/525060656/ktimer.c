#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/******************************************************
  Prepared by Matthew Yee
  
  Usage:
    ./ktimer [flag] [message]
	 
	-l (list)
	-s (seconds)
    -m (multiple timers)
	
  Examples:
	./ktimer -l
		List the expiration time of the current timer to stdout

	./ktimer -s [SEC]_"ThisIsAMessage"
		Register new timer that prints the string "ThisIsAMessage" after [SEC] seconds

	./ktimer -m [COUNT]
        Change the number of active timers supported by mytimer.c to [COUNT]
	
******************************************************/

void printManPage(void);

int main(int argc, char **argv) {
	char line[256];
	char* data;
	char* secs;
	int ii, count = 0;
	
	/* Check to see if the ktimer successfully has mknod run
	   Assumes that ktimer is tied to /dev/mytimer */
	FILE * pFile;
	pFile = fopen("/dev/mytimer", "r+");
	if (pFile==NULL) {
		fputs("ktimer module isn't loaded\n",stderr);
		return -1;
	}

	// Check if in list mode
	if (argc == 2 && strcmp(argv[1], "-l") == 0) {
		while (fgets(line, 256, pFile) != NULL) {
		  printf("");
		}
	}

	// Check if in timer mode
	else if (argc == 4 && strcmp(argv[1], "-s") == 0) {
	  secs = argv[2];
	  data = argv[3];
	  char *delim = "_";
	  //printf("%s\n", secs);
          //printf("%s\n", data);
          //printf("%s\n", delim);
	  strcat(data, delim);
	  strcat(data, secs);
	  fputs(data, pFile);		
	}

	/*// Check if in multiple timer mode
    else if (argc == 3 && strcmp(argv[1], "-m") == 0) {
        //fputs(argv[2], pFile);
    }*/

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
	printf(" -l: list active timers from the ktimer module\n");
	printf(" -s: make new timer with [message] to the ktimer module\n");
}
