#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>


/******************************************************
  Usage:
    ./mytimer_ul [flag] [message]
        -s (set timer)
	-l (list)
******************************************************/

void printManPage(void);
void sighandler(int);

int main(int argc, char **argv) {

    char line[256];
    char line_read[256];
    int oflags;
    int s_flag;
    struct sigaction action;
    int pFile = 0;
    int id;

    pFile = open("/dev/mytimer", O_RDWR);
    if (pFile==0) {
        printf("mytimer module isn't loaded\n");
        return -1;
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

    if (argc == 2)
    {
        if(strcmp(argv[1], "-l") == 0)
        {
            write(pFile, "-l", 2);
            read(pFile, line, 256);
            printf("%s\n",line);
        }
        else if(strcmp(argv[1], "-r") == 0)
        {
            write(pFile, "-r", 2);
            read(pFile, line, sizeof(line));
            id = atoi(line);
            kill(id,SIGQUIT);
            printf("All timers have been removed\n");
        }
        else
        {
            printManPage();
        }
    }
    else if (argc == 4 && strcmp(argv[1], "-s") == 0)
    {
        strcpy(line,argv[2]);
        strcat(line,argv[3]);
        s_flag = write(pFile, line, sizeof(line));
        if(s_flag == 0) // already exists
        {
            printf("timer[%s] updated to [%s]secs!\n",argv[3],argv[2]);
            return 0;
        }
        else if(s_flag == -1) //trying to set new timer...
        {
          printf("timer alredy exists!\n");
          return 0;
        }
        else
        {
            printf("Sleep!\n");
            pause();
            read(pFile, line_read, sizeof(line_read));
            printf("%s\n",line_read);
        }
    }
    else
    {
        printManPage();
    }
    close(pFile);
    return 0;
}

// SIGIO handler
void sighandler(int signo)
{
    printf("Awaken!\n");
}
void printManPage() {
    printf("Error: invalid use.\n");
    printf("/mytimer_ul [-flag] [message]\n");
    printf(" -l: read from dev/mytimer module\n");
    printf(" -s: [seconds] [message] \n");
    //printf(" -m: [number of timers] \n");
}
