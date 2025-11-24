#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/******************************************************
In example:
  Usage:
    ./nibbler_ul [flag] [message]
	 
	-r (read)
	-w (write)
	
  Examples:
	./nibbler_ul -r
		Print whatever message that the nibbler module is holding

	./nibbler_ul -w ThisIsAMessage
		Write the string "ThisIsAMessage" to the nibbler module
	
After change:
  Usage:
    ./nibbler_ul [flag] [message]
	 
	-l 
	-s [SEC] [MSG]
        -m [COUNT]
******************************************************/

void printManPage(void);

int main(int argc, char **argv) {
	char line[256];
	//int ii, count = 0;

	FILE * pFile;

	/*
	if (pFile==NULL) {
		fputs("mytimer module isn't loaded\n",stderr);
		return -1;
	}	
	if (argc == 2 && strcmp(argv[1], "-r") == 0) {
		while (fgets(line, 256, pFile) != NULL) {
			printf("%s\n", line);
		}
	}

	else if (argc == 3 && strcmp(argv[1], "-w") == 0) {
		fputs(argv[2], pFile);
	}
	*/
	if (argc == 2 && strcmp(argv[1], "-l") == 0) {
                pFile = fopen("/dev/mytimer", "r+");
                if(pFile == NULL){
                  return 0;
		}
		while (fgets(line, 256, pFile) != NULL) {printf("%s\n",line);}
                fclose(pFile);
	}

	else if (argc == 4 && strcmp(argv[1], "-s") == 0)  {
          pFile = fopen("/dev/mytimer", "a+");
	  fprintf(pFile,"%s\t",argv[2]);
	  fprintf(pFile,"%s;",argv[3]);
          fclose(pFile);
	}

	else if (argc == 3 && strcmp(argv[1], "-m") == 0) {
                pFile = fopen("/dev/mytimer", "a+");
                fputs("-", pFile);
		fputs(argv[2], pFile);
                fclose(pFile);
	}
	// Otherwise invalid
	else {
		printManPage();
	}

	return 0;
}

void printManPage() {
	printf("Error: invalid use.\n");
	printf(" ./ktimer [-flag] [message]\n");
	printf(" -l: list to stdout\n");	
	printf(" -s [SEC] [MSG]: Register a new timer\n");
        printf(" -m [COUNT]: Change the number of timer supported\n");
}

