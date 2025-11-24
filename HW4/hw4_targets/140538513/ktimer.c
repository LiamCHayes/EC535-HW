#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printManPage(void);

int main(int argc, char **argv) {
	char line[256];

	FILE * pFile;
	pFile = fopen("/dev/mytimer", "r+");
	if (pFile==NULL) {
		fputs("mytimer module isn't loaded\n",stderr);
		return -1;
	}

	// list current timers
	if (argc == 2 && strcmp(argv[1], "-l") == 0)
	{
		while (fgets(line, 256, pFile) != NULL) {
			printf("%s\n", line);
		}
	}

	// -s option
	// -s [SEC] [MSG]
	else if (argc == 4 && strcmp(argv[1], "-s") == 0)
	{
		fputs(argv[3], pFile);
		fputs(";", pFile);
		fputs(argv[2], pFile);
		fputs(";", pFile);
	}
	// default: -s [MSG]
	else if (argc == 3 && strcmp(argv[1], "-s") == 0)
	{
		fputs(argv[2], pFile);
		fputs(";", pFile);
		fputs("1", pFile);
		fputs(";", pFile);
	}
	// Otherwise invalid
	else
	{
		printManPage();
	}

	fclose(pFile);
	return 0;
}

void printManPage() {
	printf("Error:invalid use.\n");
	printf(" ktimer [-flag] [params]\n");
	printf(" -l: List to stdout the expiration time of the currently registered timer(s) and the message.\n");
	printf(" -s [SEC] [MSG]: Register a new timer that after [SEC] seconds will print the message [MSG]\n");
	printf(" -m [COUNT]: Change the number of active timers to [COUNT]\n.");
}
