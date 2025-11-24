#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

void sighandler(int);
void printManPage(void);
char msg[256];

int main(int argc, char **argv) {
	int pFile, oflags;
	struct sigaction action, oa;
	int ii, count = 0;
	char arg2[256];
	char arg3[256];
	char s_colon = ';';
	char sec_message[256];
	char read_buf[256];

	strcpy(msg, ""); //use memset instead
	// Opens to device file
	pFile = open("/dev/mytimer", O_RDWR);
	if (pFile < 0) {
		fprintf (stderr, "mytimer module isn't loaded\n");
		return 1;
	}

	// Setup signal handler
	memset(&action, 0, sizeof(action));
	action.sa_handler = sighandler;
	action.sa_flags = SA_SIGINFO;
	sigemptyset(&action.sa_mask);
	sigaction(SIGIO, &action, NULL);
	fcntl(pFile, F_SETOWN, getpid());
	oflags = fcntl(pFile, F_GETFL);
	fcntl(pFile, F_SETFL, oflags | FASYNC);

	// Check if in list mode
    	if (argc == 2 && strcmp(argv[1], "-l") == 0) {
		read(pFile, read_buf, 256);
		if(errno == 3){  //no timers exist, print nothing
			return 0;
		}
		else{
			printf("%s\n", read_buf);
		}
       }
	// Check if in set mode
       else if (argc == 4 && strcmp(argv[1], "-s") == 0) {
		strcpy(arg3, argv[3]);
		strcpy(sec_message, strncat(arg3, &s_colon, 1));
		strcpy(sec_message, strcat(sec_message, argv[2]));
		//first read in timer message and timer active
	  	if(write(pFile, sec_message, strlen(sec_message))==-1){ //error was thrown

				if (errno == 17){ //if errno indicates same message --> EEXIST
					printf("Timer %s has now been reset to %s seconds!\n", argv[3], argv[2]);
					return 0;
				}
				else if (errno == 16){ //if errno indicates it cannot set a new timer bc one already set --> EBUSY
					printf("A timer exists already!\n");
					return 0;
				}

				else{  //buffer overflow (29) or some other error (14)
					printf("Error - timer not set.\n");
					return 0;
				}
			}

			else{ // timer set
				strcpy(msg, argv[3]);
	    	// Waits.
	    	pause();
		

          }
	 
	}
     else if (argc == 2 && strcmp(argv[1], "-r") == 0) {
        //strcpy(arg2, strncat(argv[2], "-", 1));
				write(pFile, argv[1], 8);
			//	printf("to kernel: %s\n", arg2);
     }
     
     else {
        printManPage();
     }


	// Closes.
	close(pFile);
	return 0;

}

// SIGIO handler
void sighandler(int signo)
{

	printf("%s\n", msg);

	return;
}

void printManPage() {
    printf("Error: invalid use.\n");
    printf(" mytimer [-flag] [message]\n");
    printf(" -l: list current timers count until finished and corresponding messages\n");
    printf(" -s [SEC] [MSG]: register new timer that will expire after [SEC] and print [MSG]\n");
    printf(" -r: remove active timers\n");

}
