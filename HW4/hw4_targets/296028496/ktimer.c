#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

/******************************************************
  Prepared by Luxuan QI lqi25@bu.edu
  
  Usage:
    ./ktimer [flag] [message]
	 
	-l (list)
	-s (set timer)
	
  Examples:
	./ktimer -l
		Print all the timer

	./ktimer -s  settime ThisIsAMessage
		Write the string "ThisIsAMessage" to the mytimer module and set timer
        
        ./ktimer -r 
                Remove the timer
	
******************************************************/
char line[256];
char time[10];
char final[256],*fl = final;


void sighandler(int);
void printManPage(void);

void readFile(int fd) {
    int bytes_read;
    int k = 0;
    char t=0;
    while ((bytes_read = read(fd, &t, 1)) > 0)  
    {
        line[k++] = t;    
    }
    
}

int main(int argc, char **argv) {
        errno=0;
        int pFile,oflags;
        struct sigaction action, oa;
        char message[256];
 
        
	pFile = open("/dev/mytimer", O_RDWR);
	if (pFile < 0) {
		fprintf (stderr, "mytimer module isn't loaded\n");
		return 1;
	}
        
        if (argc == 2 && strcmp(argv[1], "-l") == 0) {
            readFile(pFile);
            printf("%s", line);
            if(strcmp(line,"")!=0)
                printf("\n");
	}
        
        else if (argc == 2 && strcmp(argv[1], "-r") == 0) {
            strcpy(final,argv[1]); //-r
            strcat(final,";");     //;
            
            write(pFile,fl,strlen(final));
        }
        
        else if (argc == 4 && strcmp(argv[1], "-s") == 0) {
            strcpy(time,argv[2]);
            strcpy(message,argv[3]);
            
            strcpy(final,argv[1]); //-s
            strcat(final,";");     //;
            strcat(final,argv[2]); //20
            strcat(final,";");     //;
            strcat(final,argv[3]); //hello
            
            // Setup signal handler
            memset(&action, 0, sizeof(action));
            action.sa_handler = sighandler;
            action.sa_flags = SA_SIGINFO;
            sigemptyset(&action.sa_mask);
            sigaction(SIGIO, &action, NULL);
            fcntl(pFile, F_SETOWN, getpid());
            oflags = fcntl(pFile, F_GETFL);
            fcntl(pFile, F_SETFL, oflags | FASYNC);
          
            int jj=write(pFile,fl,strlen(final));
            pause();
            
            if(jj>0){
            printf("%s\n",message);
            }
              
               	
        }
        
        // Otherwise invalid
	else {
		printManPage();
	}
        
        close(pFile);
	
	return 0;
}

// SIGIO handler
void sighandler(int signo)
{
}

void printManPage() {
	printf("Error: invalid use.\n");
	printf(" ktimer [-flag] [message]\n");
	printf(" -l: List to stdout expiration time in seconds and message\n");
	printf(" -s [SEC] [MSG]: Start timer after [SEC] will print [MSG]\n");
	printf(" -r:Remove all the timers\n");
}