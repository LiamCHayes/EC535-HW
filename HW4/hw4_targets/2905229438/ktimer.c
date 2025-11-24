#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/******************************************************
  Usage:
    ./ktimer [flag] [message]

	-l (read)
	-s (write)
	-m change count of module

  Examples:
	./ktimer -s [secs] [message]
		Create timer and include message

	./ktimer -l
		see timers

******************************************************/

void printManPage(void);

int main(int argc, char **argv) {
    char line[256];
    int ii, count = 0;
    int i = 0;

    /* Creating a timer variables */
    char timer_var[256];

    /* Check to see if the nibbler successfully has mknod run
       Assumes that nibbler is tied to /dev/nibbler */
    FILE * pFile;
    pFile = fopen("/dev/mytimer", "r+");
    if (pFile==NULL) {
        fputs("timer module isn't loaded\n",stderr);
        return -1;
    }

    // Check if in list mode
    if (argc == 2 && strcmp(argv[1], "-l") == 1) {
        while (fgets(line, 256, pFile) != NULL) {
            printf("%s\n", line);
        }
    }

    //check if in modify timer amount mode
    else if (argc == 3 && strcmp(argv[1],"-m") == 1) {
        timer_var[0] = 'm';
        for(i = 1; i < strlen(argv[2]) + 1; i++)
        {
            timer_var[i] = argv[2][i-1];
        }
        timer_var[i] = '!';
        fputs(timer_var,pFile);
    }


    // Check if in create timer mode
    else if (argc == 4 && strcmp(argv[1], "-s") == 0) {
        // parse the string and add identifiers so kernel can quickly buffer in the
        // inputs
        i = 0;
        for(i = 0; i < strlen(argv[2]); i++)
        {
            timer_var[i] = argv[2][i];
        }
        timer_var[i] = '!';
        i++;
        int k = i;

        for(i ; i - k < strlen(argv[3]); i++)
        {
            timer_var[i] = argv[3][i - k];
        }
        timer_var[i] = '\0';
        fputs(timer_var, pFile);
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
    printf(" ktimer [-flag] [message]\n");
    printf(" -l: read from list of timers\n");
    printf(" -s: write [message] and assign it to timers\n");
    printf(" -m: mode [number] assign how many timers can be active\n");

}
