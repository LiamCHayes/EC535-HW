#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/******************************************************

  Usage:
    ./kmytimer [flag] [message]
	 
	-l read
	-s (Schedule timer)
	-m (Change number of timers)
	
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

	// Check if change timer count 
	if (argc == 3 && strcmp(argv[1], "-m") == 0) {
		char message[2];
		message[0] = '~';
		message[1] = argv[2][0];
		fputs(message, pFile);
	}

	// Check if add timer 
	else if (argc == 4 && strcmp(argv[1], "-s") == 0) {
		char message[140], *walker;
		sprintf(message, "%s;%s\n",argv[2],argv[3]);
		fputs(message , pFile);
	}
	//Check if read timers
	else if(argc == 2 && strcmp(argv[1], "-l") == 0){
		while (fgets(line, 256, pFile) != NULL) {
			printf("%s\n", line);
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
	printf(" kmytimer [-flag] [message]\n");
	printf(" -l: read from the mytimer module\n");	
	printf(" -s: write [message] to the mytimer module\n");
	printf(" -m: change number of timers to [message]\n");
}