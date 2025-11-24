#define _GNU_SOURCE

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

void wake_up(int);
static int fp;
static char message[256];
static int removed = 0;

int main(int argc, char ** argv) {
    int oflags;
    struct sigaction action, oa;
    if(strcmp("-l",argv[1]) == 0) {
        char line[256] = "";
        int foo = 0;
        char check[20] = "";
        char msg[256] = "";
        fp = open("/proc/mytimer",O_RDONLY);
        read(fp, line, 256);
        sscanf(line, "%s %s %*s", check, msg);
        if(strcmp(check,"message:") == 0) {
            sscanf(line, "%*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %d", &foo);
            printf("%d %s\n", foo, msg);
        }
        close(fp);
    }
    if(strcmp("-s",argv[1]) == 0) {
        fp = open("/dev/mytimer", O_RDWR);
        char msg[256] = ""; //10 for delimiter + hex. representation of seconds

        //setting up signal handler
        char line[256] = "";
        int foo;
        memset(&action, 0, sizeof(action));
        action.sa_handler = wake_up;
        action.sa_flags = SA_SIGINFO;
        sigemptyset(&action.sa_mask);
        sigaction(SIGIO, &action, NULL);
        fcntl(fp, F_SETOWN, getpid());
        oflags = fcntl(fp, F_GETFL);
        fcntl(fp, F_SETFL, oflags | FASYNC);


        strcat(msg, "s_");
        strcat(msg, argv[3]);
        strcpy(message, argv[3]);
        strcat(msg, "_");
        unsigned int secs;
        sscanf(argv[2],"%u",&secs);
        char hexa[9];
        sprintf(hexa,"%08x",secs); //by storing it in hex, I can convert it one by one in the kernel
        strcat(msg, hexa);
        char pid[10];
        sprintf(pid,"%08x",getpid());
        strcat(msg,pid);
        foo = write(fp, msg, 256);
        if(foo == 1) {
            printf("too many Timers!\n");
            close(fp);
            return 0;
        } else if(foo == 6) {
            printf("Timer %s has been reset to %u!\n",message, secs);
            return 0;
        }
        pause();
        close(fp);
    }
    if(strcmp(argv[1],"-r") == 0) {
        char command[50];
        fp = open("/dev/mytimer",  O_RDWR);
        write(fp,"r_",18);
        close(fp);
        strcpy(command, "killall ktimer");
        system(command);
    }
    return 0;
}
void wake_up(int signo)
{
    char line[256];
    fp = open("/dev/mytimer",O_RDONLY);
    read(fp, line, 256);
    if(strcmp(line,"0") == 0) printf("%s\n",message);
    close(fp);
}
