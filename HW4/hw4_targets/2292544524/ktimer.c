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
	char msg[128];
	char s[256];
	char* token;
	unsigned long seconds;
	int ii, count = 1;
	//int counter = 0;
	/* Check to see if the nibbler successfully has mknod run
	   Assumes that nibbler is tied to /dev/nibbler */
	FILE * pFile;
	pFile = fopen("/dev/mytimer", "r+");
	if (pFile==NULL) {
		fputs("Mytimer module isn't loaded\n",stderr);
		return -1;
	}

	// Check if in read mode
	if (argc == 2 && strcmp(argv[1], "-l") == 0) {

		while (fgets(line, sizeof(line), pFile) != NULL) {
			// token = strtok(line,";");
			// printf("%s ", token);
			// token = strtok(NULL,";");
			// seconds = strtoul(token,NULL,10);
			// printf("%lu\n", seconds);
			printf("%s",line);
		}
	
	}

	// Check if in write mode
	else if (argc == 4 && strcmp(argv[1], "-s") == 0) {
		//printf("IN S\n");
		strcat(s,argv[1]);
		strcat(s,";");
		strcat(s,argv[3]);
		strcat(s,";");
		strcat(s,argv[2]);
		fputs(s, pFile);
		//counter = counter + 1;
        }

	else if(argc = 3 && strcmp(argv[1], "-m") == 0){
		//printf("In M\n");
		count = strtol(argv[2],NULL,10);
		if(count < 1) {
			printf("Count must be greater than 0\n");
			fclose(pFile);
			return -1;
		}
		else{
			strcat(s,argv[1]);
			strcat(s,";");
			strcat(s,argv[2]);
			fputs(s,pFile);
			//count = argv[2];
		}
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
	printf(" ktimer [-flag]\n");
	printf(" -l: List current queue\n");
	printf(" -s: [Seconds] [message] Store timer and message\n");
	printf(" -m: [Count] Number of timers\n");
}
