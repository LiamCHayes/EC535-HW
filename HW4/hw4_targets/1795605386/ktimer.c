#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/******************************************************

  Usage:
    ./ktimer [flag] <additional options>

	-l (list current timers and time remaining)
	-s <sec> <msg> (make new timer of duration sec and message msg)
	-m <count> (number of timers)
  Examples:
	./ktimer -l
		List the current active timers

	./ktimer -s 10 HelloWorld!
		Create a new timer with message HelloWorld!

******************************************************/

void printManPage(void);

int main(int argc, char **argv) {
    char line[256];
    int ii, count = 0;
    char strptr[140];
    unsigned int num;
    char* ptr;

    /* Check to see if the nibbler successfully has mknod run
       Assumes that nibbler is tied to /dev/nibbler */
    FILE * pFile;
    pFile = fopen("/dev/mytimer", "r+");
    if (pFile==NULL) {
        fputs("mytimer module isn't loaded\n",stderr);
        return -1;
    }

    // Check if in list timer mode
    if (argc == 2 && strcmp(argv[1], "-l") == 0) {
        // while (fgets(line, 140, pFile) != NULL) {
        // 	printf("%s\n", line);
        // }
        fputs("_",pFile);
    }
    // Check if in add timer mode
    else if (argc == 3 && strcmp(argv[1], "-m") == 0) {
        num = strtoul(argv[2],&ptr,10);
        sprintf(strptr,";%d\n", num);
        fputs(strptr, pFile);
    }
    // Check if in set timer mode
    else if (argc == 4 && strcmp(argv[1], "-s") == 0) {
        num = strtoul(argv[2],&ptr,10);
        sprintf(strptr,"%s;%d\n", argv[3], num);
        fputs(strptr, pFile);
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
    printf(" ktimer [-flag] <options>\n");
    printf(" -l: list current active timers\n");
    printf(" -s <sec> <msg>: create a new timer\n");
    printf(" -m <count>: create multiple timers\n");
}
