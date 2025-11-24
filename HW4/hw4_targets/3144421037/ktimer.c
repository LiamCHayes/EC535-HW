#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#define DEVICE "/dev/mytimer"

void printManPage(void);

int main(int argc, char **argv) {
    char line[256];
    int ii, count = 0;
    char inStr[128];

    /* Check to see if the nibbler successfully has mknod run
    Assumes that nibbler is tied to /dev/nibbler */
    FILE * pFile;
    pFile = fopen(DEVICE, "r+");
    if (pFile == NULL) {
        fputs("mytimer module isn't loaded\n", stderr);
        return -1;
    }
    // Check for -l option
    if (argc == 2 && strcmp(argv[1], "-l") == 0) {
        fputs(argv[1], pFile);
    }
    // Check if is -s option
    else if (argc == 4 && strcmp(argv[1], "-s") == 0) {
        strcpy(inStr, argv[1]);
        strcat(inStr, argv[2]);
        strcat(inStr, "/");
        strcat(inStr, argv[3]);
        fputs(inStr, pFile);
    }
    // Otherwise invalid
    else {
        printManPage();
    }
    
    fclose(pFile);
    char pline[150];
    FILE *fp = popen("cat /dev/mytimer", "r");
    
    if(fp==NULL)
        printf("error popen");
    
    while (fgets(pline, sizeof pline, fp))
        printf("%s\n", pline);
    
    pclose(fp);
    return 0;
}

void printManPage() {
    printf("Error: invalid use.\n");
    printf(" kTimer [-flag] \n");
    printf(" -l: List Timer\n");
    printf(" -s [SEC] [MSG]: Register or Modify timer\n");
}