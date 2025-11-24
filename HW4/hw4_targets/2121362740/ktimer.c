#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/******************************************************
  
  Usage:
    ./nibbler_ul [flag] [message]
	 
	-r (read)
	-w (write)
	
  Examples:
	./nibbler_ul -r
		Print whatever message that the nibbler module is holding

	./nibbler_ul -w ThisIsAMessage
		Write the string "ThisIsAMessage" to the nibbler module
	
******************************************************/

void printManPage(void);

int main(int argc, char **argv) {
	char line[256];
	char *msg;
	char delimiter[2] = ";"; //parses to ; to figure message and seconds
	//char timer_string[256];
	int ii, count = 0;
	
	/* Check to see if the nibbler successfully has mknod run
	   Assumes that nibbler is tied to /dev/nibbler */
	FILE * pFile;
	pFile = fopen("/dev/mytimer", "r+"); // where the timer info is loaded
	if (pFile==NULL) {
		fputs("mytimer module isn't loaded\n",stderr);
		return -1;
	}

	// Check if in list mode
	if (argc == 2 && strcmp(argv[1], "-l") == 0){
		while (fgets(line, 256, pFile)) {
			printf("%s \n", line); // will print out the message			
		}
	}

/*
	// Check if in multiple timer mode 
	else if (argc == 3 && strcmp(argv[1], "s") == 0) {
		fputs(argv[2], pFile);
	}
*/
	// ./ktimer -s [SEC] [MSG] 4 args
	// Check if in register timer mode -s
	// timer_string will hold the timer input in this form message + ; + seconds
	else if (argc == 4 && strcmp(argv[1], "-s") == 0) {
		strcpy(line, argv[3]); // message
		strcat(line, ";");   // adds ; to the message
		strcat(line, argv[2]); // reads in the seconds
		//strcat(line, ";");   // adds ; to the message
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
	printf(" ./ktimer [-flag] [message]\n");
	printf(" -l: list timer(s) from the mytimer module\n");	
	printf(" -s: create timer in mytimer module\n");
}
