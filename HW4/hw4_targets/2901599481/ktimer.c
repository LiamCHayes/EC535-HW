#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

/******************************************************
  Usage:
    ./ktimer [flag] [message]

	-l (read)
	-s (write)
	-m change amount of timers allowed in module

  Examples:
	./ktimer -s [secs] [message]
		Create timer and include message

	./ktimer -l
		see timers

******************************************************/

void sighandler(int);

void printManPage(void);

char msg[256];

int main(int argc, char **argv) {
    char line[256];
    int ii, count = 0;
    int i = 0;
    char check[10];
    /* Lab 3 */
    int pFile, oflags;
    struct sigaction action, oa, noprint;  

    /* Creating timer variables */
    char timer_var[256];
    memset(timer_var, 0 , 256);

    /* Check to see if the nibbler successfully has mknod run
       Assumes that nibbler is tied to /dev/nibbler */
    pFile = open("/dev/mytimer", O_RDWR);
    if (pFile < 0) {
        fprintf(stderr, "timer module isn't loaded\n");
        return 1;
    }

    // Check if in list mode
    if (argc == 2 && strcmp(argv[1], "-l") == 0) {
        read(pFile, line, 256);
    }

    //Deletes all current timers
    else if (argc == 2 && strcmp(argv[1],"-r") == 0) {
        timer_var[0] = 'r';
	memset(msg, 0 , 256);
	memset(line, 0 , 256);
	write(pFile, timer_var, 256);
	system("pkill -SIGKILL ktimer");
        
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
	    msg[i - k] = argv[3][i - k];
        }
        timer_var[i] = '\0';	
	
	
	// Setup signal handler
	memset(&action, 0, sizeof(action));
	action.sa_handler = sighandler;
	action.sa_flags = SA_SIGINFO;
	sigemptyset(&action.sa_mask);
	sigaction(SIGIO, &action, NULL);
	
        fcntl(pFile, F_SETOWN, getpid());
	oflags = fcntl(pFile, F_GETFL);
	fcntl(pFile, F_SETFL, oflags | FASYNC);

	write(pFile, timer_var, strlen(timer_var)+1);
	read(pFile, check , 10);
	printf("%s \n" , check);
	if(check[0] == 0){
		pause();
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
	printf("%s \n" , msg);

}

void printManPage() {
    printf("Error: invalid use.\n");
    printf(" ktimer [-flag] [message]\n");
    printf(" -l: read from list of timers\n");
    printf(" -s: write [message] and assign it to timers\n");
    printf(" -m: mode [number] assign how many timers can be active\n");

}
