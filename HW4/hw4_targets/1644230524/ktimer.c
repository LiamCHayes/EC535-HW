#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//need to change this comment
/******************************************************

  Usage:
    ./mytimer_ul [flag] [message]

	-l (list)
	-s [SEC] [MSG] (set)
	-m [count] (change number of supported timers)

  Examples:
	./mytimer_ul -l
		Print all timers and times before expiring

	./mytimer_ul -s 10 This is a message
		Print this is a message after 10 seconds

******************************************************/

void printManPage(void);

int main(int argc, char **argv) {
    char line[256];
    int ii, count = 0;
    int sec; //number of seconds left before timer finished -> need to actually grab this value from kernel!!! (T->expired)

    /* Check to see if the mytimer successfully has mknod run
       Assumes that mytimer is tied to /dev/mytimer */
    FILE * pFile;
    pFile = fopen("/dev/mytimer", "r+");
    if (pFile==NULL) {
        fputs("mytimer module isn't loaded\n",stderr);
        return -1;
    }

    // Check if in list mode
    if (argc == 2 && strcmp(argv[1], "-l") == 0) {
        fgets(line, 256, pFile); // triggers read function in kernel
    }
    /*while (fgets(line, 256, pFile) != NULL) { //reads msg line by line from kernel mod //get time left in sec here?
    	printf("%s ; %d\n", line, sec); //line = msg to print when time expires, sec = num secs left before timer finishes
    }
    }*/

    // Check if in set mode
    else if (argc == 4 && strcmp(argv[1], "-s") == 0) {
        fputs(argv[3], pFile); //trigger write function
        fputs(";", pFile);
        fputs(argv[2], pFile);
        fputs("\n", pFile);
    }

    // Check if in num active timers mode
    else if (argc == 3 && strcmp(argv[1], "-m") == 0) {
        fputs("-", pFile);
        fputs(argv[2], pFile);
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
    printf(" mytimer [-flag] [message]\n");
    printf(" -l: list current timers count until finished and corresponding messages\n");
    printf(" -s [SEC] [MSG]: register new timer that will expire after [SEC] and print [MSG]\n");
    printf(" -m [COUNT]: change number of supported active timers to [COUNT] (defaults to 1 upon module load)\n");

}
