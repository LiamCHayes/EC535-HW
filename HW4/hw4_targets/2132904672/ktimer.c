#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #define DEV_FILE "mytimer.txt"
#define DEV_FILE "/dev/mytimer"

/******************************************************
  
  Usage:
    ./ktimer_ul [flag] [message]
	 
	-r (read)
	-w (write)
	
  Examples:
	./ktimer_ul -r
		Print whatever message that the ktimer module is holding

	./ktimer_ul -w ThisIsAMessage
		Write the string "ThisIsAMessage" to the ktimer module
	
******************************************************/

void printManPage(void);

// If the new timer is updating an old timer, then the updated version is put into the temp
// file, otherwise the old
// timer is copied. Temp and /dev/mytimer then are swapped,
// effectively updating any timers.
int updateTimers(FILE* devFile, char* newTime, char* newMessage, int count, int maxTimers) {
	char line[256];
	char* msg;
	int updated = 0;
	char newCommand[256];
	long numInput;

	FILE* tempFile;
	tempFile = fopen("/dev/tempFile.txt", "w+");


	// Check if timer delay is valid
	sscanf(newTime, "%ld", &numInput);
	if (numInput < 0) {
		fputs("Negative timer delays not allowed.\n", stderr);
		return -2;
	}

	sprintf(newCommand, "%s;%s\n", newMessage, newTime);

	// Checks is a timer needs to be updated
	while (fgets(line, 256, devFile) != NULL) {
		char temp[256];
		strcpy(temp, line);
		msg = strtok(temp, ";");
		if (strncmp(newMessage, msg, sizeof(newMessage)) != 0) {
			fputs(line, tempFile);
		} else {
			fputs(newCommand, tempFile);
			updated = 1;
		}
	}

	fclose(devFile);
	devFile = fopen(DEV_FILE, "w+");
	fseek(tempFile, 0, SEEK_SET);

	// Swaps the file contents
	while(fgets(line, 256, tempFile) != NULL) {
		fputs(line, devFile);
	}

	if(count < maxTimers) {
		if (!updated) {
			fputs(newCommand, devFile);
			count++;
		}
	} else {
		fputs("Already reached the maximum amount of timers. Consider increasing the timer count.\n",stderr);
	}

	fclose(tempFile);
	return count;
}

// [-l] list all timers from /dev/mytimer
void listTimers(FILE* devFile) {

	char line[256];
	if(fgets(line, 256, devFile) == NULL) {
		printf("");
	} else {
		do {
			int i = 0;
			while (line[i] != ';') {
				i++;
			}
			line[i] = ' ';
			printf("%s", line);
		} while (fgets(line, 256, devFile) != NULL);
	}

}

int main(int argc, char **argv) {
	int ii, count = 0;
	int maxTimers = 1;

	char formattedCommand[256];
	// int numInput;
	
	/* Check to see if the ktimer successfully has mknod run
	   Assumes that ktimer is tied to /dev/mytimer */
	FILE* pFile;
	pFile = fopen("/dev/mytimer", "r+");

	if (pFile==NULL) {
		fputs("mytimer module isn't loaded\n",stderr);
		return -1;
	}

	// Check which command is being used
	if (argc == 2 && strcmp(argv[1], "-l") == 0) {
		listTimers(pFile);
	} else if (argc == 4 && strcmp(argv[1], "-s") == 0) {
		count = updateTimers(pFile, argv[2], argv[3], count, maxTimers);
	} else if (argc == 3 && strcmp(argv[1], "-m") == 0){

		int temp;
		sscanf(argv[2], "%d", &temp);

		if (temp > 0 && temp <= 5) {
			sscanf(argv[2], "%d", &maxTimers);
		} else {
			fputs("Already reached timer count maximum. Please buy the DLC.\n", stderr);
		}

	} else {
		printManPage();
	}

	fclose(pFile);
	return 0;
}

void printManPage() {
	printf("Error: invalid use.\n");
	printf(" ktimer_ul [-flag] [message]\n");
	printf(" -l: list the active timers\n");	
	printf(" -s: create a timer with a message [msg]\n");
}
