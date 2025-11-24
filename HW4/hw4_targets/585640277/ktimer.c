#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printManPage(void);

int main(int argc, char **argv) {
	char line[256];
	int ii, count = 0;
	
	/* Check to see if the nibbler successfully has mknod run
	   Assumes that nibbler is tied to /dev/nibbler */
	FILE * pFile;
	pFile = fopen("/dev/mytimer", "r+"); //"/dev/mytimer"
	if (pFile==NULL) {
		fputs("timer module isn't loaded\n",stderr);
		return -1;
	}

	// Check if in read mode
	/*if (argc == 2 && strcmp(argv[1], "-r") == 0) {
		while (fgets(line, 256, pFile) != NULL) {
			printf("%s\n", line);
		}
	}

	// Check if in write mode
	else if (argc == 3 && strcmp(argv[1], "-w") == 0) {
		fputs(argv[2], pFile);
	}*/

	if (argc == 2 && strcmp(argv[1], "-l") == 0) {
		fgets(line, 256, pFile);
		printf("%s\n", line);
	}
	
	else if (argc == 4 && strcmp(argv[1], "-s") == 0) {
		// If using make		
		//sprintf(line, "%s;%s;%s", argv[1], argv[2], argv[3]);
		sprintf(line, "%s;%s", argv[3], argv[2]);
		fputs(line, pFile);
	}
	
	/*else if (argc == 3 && strcmp(argv[1], "-m") == 0) {
		fputs(argv[2], pFile);		
	} */
	// Otherwise invalid
	else {
		printManPage();
	}

	fclose(pFile);
	return 0;
}

void printManPage() {
	printf("Error: invalid use.\n");
	printf(" ktimer [-flag] [message] ([message])\n");
	//printf(" -r: read from the tim module\n");	
	//printf(" -w: write [message] to the nibbler module\n");
}
