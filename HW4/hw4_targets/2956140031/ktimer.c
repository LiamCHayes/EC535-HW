#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/******************************************************
  Usage:
    ./mytimer_ul [flag] [message]
        -s (set timer)
	-l (list)
******************************************************/

void printManPage(void);

int main(int argc, char **argv) {
        char line[256];
        int ii, count = 0;
        /* Check to see if the/mytimer successfully has mknod run
           Assumes that/mytimer is tied to /dev/mytimer */
        FILE * pFile;
        pFile = fopen("/dev/mytimer", "r+");
        if (pFile==NULL) {
                fputs("mytimer module isn't loaded\n",stderr);
                return -1;
        }                                                                           
        if (argc == 2 && strcmp(argv[1], "-l") == 0)
	{    
		while (fgets(line, 256, pFile) != NULL)
		{
			printf("%s\n",line);
		}                                                                                         
        }                                                                                                                                                                                      
	//Check if in write mode
	else if (argc == 4 && strcmp(argv[1], "-s") == 0)
	{	
		while (fgets(line, 256, pFile) != NULL)
		{
			char * message = strtok(line, " ");
			if(strcmp(message,argv[3]))
			{
				printf("A timer alredy exists\n");
			}	
		}  
                fputs(argv[2], pFile);
                fputs(argv[3], pFile);
		//printf("The timer (%s) was updated\n",argv[3]);
        }
        else if (argc == 3 && strcmp(argv[1], "-m") == 0)
	{
		printf("need to implement: adding more timers\n");
		//fputs("+", pFile);
                //fputs(argv[2], pFile);

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
        printf("Error: invalid use.\n");
        printf("/mytimer_ul [-flag] [message]\n");
        printf(" -l: read from dev/mytimer module\n");
        printf(" -s: [seconds] [message] \n");
        printf(" -m: [number of timers] \n");
} 
