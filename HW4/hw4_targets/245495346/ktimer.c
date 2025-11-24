#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/******************************************************
  
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
	int mylen, pad;
	int ii, count = 0;
	int iii;
	
	/* Check to see if the mytimer successfully has mknod run
	   Assumes that mytimer is tied to /dev/mytimer */
	FILE * pFile;
	pFile = fopen("/dev/mytimer", "r+");
	if (pFile==NULL) {
		fputs("mytimer module isn't loaded\n",stderr);
		return -1;
	}

	// Check if in read mode
	if (argc == 2 && strcmp(argv[1], "-l") == 0) {
		while (fgets(line, 256, pFile) != NULL) {
			printf("%s\n", line);
		}
	}

	// Check if in write mode
	else if (argc == 4 && strcmp(argv[1], "-s") == 0) {
		fputs(argv[1], pFile);
		fputs("_", pFile);
		// mylen = (int)( sizeof(argv[2]) / sizeof(argv[2][0]) );

	int count = 0; 
while(argv[2][++count] != '\0');
		mylen=count;
		pad=10-mylen;
		for (iii=0; iii<pad;iii++){
			fputs("0", pFile);
		}
		fputs(argv[2], pFile);
		fputs("_", pFile);
		fputs(argv[3], pFile);
		fputs("_", pFile);
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
	printf(" mytimer_ul [-flag] [message]\n");
	printf(" -r: read from the mytimer module\n");	
	printf(" -w: write [message] to the mytimer module\n");
}
