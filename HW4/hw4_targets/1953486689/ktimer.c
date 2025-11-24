#define _GNU_SOURCE

#include <stdlib.h>
#include <fcntl.h> // Open, write, close
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>


static	char messages[129];
static int opened, WR;

void sighandler(int);

int main(int argc, char **argv) {
    int oflags;
    struct sigaction action, oa;
    int ii, i, flag_m, count = 0;
    char line[256];

    pid_t pid = getpid();

    flag_m = 0;
    WR = 0;
    // Open the device
    opened = open("/dev/mytimer", O_RDWR);
    if (opened < 0) {
        fprintf (stderr, "mytimer module isn't loaded\n");
        return 1;
    }

    // The signal setup
    memset(&action, 0, sizeof(action));
    action.sa_handler = sighandler;
    action.sa_flags = SA_SIGINFO;
    sigemptyset(&action.sa_mask);
    sigaction(SIGIO, &action, NULL);
    fcntl(opened, F_SETOWN, getpid());
    oflags = fcntl(opened, F_GETFL);
    fcntl(opened, F_SETFL, oflags | FASYNC);


    // Start reading the inputs

    // When the flag -l is used
    if (argc == 2 && strcmp(argv[1], "-l") == 0) {
        char msg[129];
        char time2[10];
        char lbuff[2] = {'>','\0'};
        int temp1, temp2;
        temp1 = 0;
        temp2 = 0;

        write(opened, lbuff, 2);
        read(opened,line,256);

        // Return nothing if no timers are active
        if(line[0] == '-')
        {
            close(opened);
            return 0;
        }

        for(i = 0; i < 256; i++)
        {
            if(line[i] != ';' && flag_m == 0)
            {
                msg[temp1] = line[i];
                temp1++;
            }
            else if(line[i] == ';')
                flag_m = 1;
            else if (line[i] != ';')
            {
                time2[temp2] = line[i];
                temp2++;
            }
            else if(line[i] == ';' && flag_m == 1)
                break;
        }
        printf("timer1 has %s seconds left with the message: %s\n", time2, msg);

    }
    // When the flag -s
    if (argc == 4 && strcmp(argv[1], "-s") == 0) {
        // Checkes if the message is new
        if( strcmp(messages, argv[3]) != 0)
        {
            char in[200];
            char sbuff[5];
            strcpy(messages, argv[3]);
            sprintf(in, "%s;%s>%lu>", argv[2],argv[3], pid);
            write(opened,in, 200);
            read(opened, sbuff, 5);
            if(sbuff[0] == '<')
            {
                printf("Timer %s has now been reset to %s seconds!\n", argv[3], argv[2]);
                close(opened);
                return 0;
            }
            else if(sbuff[0] == '+')
            {
                printf("A timer exists already!\n");
                close(opened);
                return 0;
            }

            WR =1;
            pause();
        }

    }

    // When the flag -r
    if (argc == 2 && strcmp(argv[1], "-r") == 0) {
        char rbuff[2] = {'=','\0'};
        write(opened, rbuff, 2);
    }

    // Closes.
    close(opened);

    return 0;
}

// SIGIO handler
void sighandler(int signo)
{
    char signalbuffer[5];
    read(opened, signalbuffer, 5);
    // Two flags used, one to see if it actually made a timer
    // The other flag "~" is to see if it was killed or if it was signal to wake up
    if(WR)
    {
        if(signalbuffer[0] != '~')
            printf("%s\n", messages);

        // Reset the message
        memset(messages, '\0', strlen(messages));
    }

}
