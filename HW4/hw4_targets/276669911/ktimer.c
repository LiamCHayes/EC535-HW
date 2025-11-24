#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/******************************************************
  Usage:
    ./ktimer [flag] [message]
	 
	-l (list)
	-s (set timer)
	
  Examples:
	./ktimer -l
		Print all the timer

	./ktimer -s  settime ThisIsAMessage
		Write the string "ThisIsAMessage" to the mytimer module and set timer
        
        ./ktimer -m count
	
******************************************************/


int main(int argc, char **argv) {
	char line[256];
	int ii, count = 0;
        char buffer[100];
        char sep =';'; 
        char *s = &sep;
        int num = 0;
	
	/* Check to see if the nibbler successfully has mknod run
	   Assumes that nibbler is tied to /dev/mytimer */
	FILE * pFile;
	pFile = fopen("/dev/mytimer", "a+");
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
        
        if (argc == 3 && strcmp(argv[1], "-m") == 0) {
            printf("%s\n",argv[2]);
	}
	// Check if in write mode
	else if (argc == 4 && strcmp(argv[1], "-s") == 0) {
               
            fputs(argv[2], pFile);
            fputs(s,pFile);
            fputs(argv[3], pFile);
                
                 
               	
		}
                
	
	fclose(pFile);
	return 0;
}
