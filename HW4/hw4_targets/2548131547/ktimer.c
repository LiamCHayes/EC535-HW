#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

/******************************************************
  Usage:
    ./ktimer [flag] [SEC] [MSG]

	-l (read)
	-s (write)
    -r (delete all)

  Examples:
	./ktimer -l
		Timer message [sec remaining]

	./ktimer -s 20 "Timer message"
		Write the string "20;Timer message" to the mytimer module

    ./ktimer -r
        [no output - all current timers deleted]

******************************************************/

void printManPage(void);
int writeToDev(char *, char);
void readFromDev(int);

int main(int argc, char **argv) {
    char sendmsg[135];

    /* Check to see if the mytimer successfully has mknod run
       Assumes that mytimer is tied to /dev/mytimer */

    // Check if in read mode
    if (argc == 2 && strcmp(argv[1], "-l") == 0) {
        readFromDev(0);
        return 0;
    }

    // Check if in write mode
    else if (argc == 4 && strcmp(argv[1], "-s") == 0) {
        strcpy(sendmsg, argv[2]);
        strcat(sendmsg, ";");
        strcat(sendmsg, argv[3]);
        return writeToDev(sendmsg, 0);
    }

    // Check if in delete all mode
    else if (argc == 2 && strcmp(argv[1], "-r") == 0) {
        strcat(sendmsg, "-r");
        return writeToDev(sendmsg, 1);
    }

    // Otherwise invalid
    else {
        printManPage();
    }

    return 0;
}

void printManPage() {
    printf("Error: invalid use.\n");
    printf(" ktimer [-flag] [SEC] [MSG]\n");
    printf(" -l: read from the mytimer module\n");
    printf(" -s: write [SEC] [MSG] to the mytimer module\n");
    printf(" -r: remove any existing timers from the mytimer module\n");
}

int writeToDev(char *s, char mode) {  // "mode"=0 means write a normal message; "mode"=1 means write delete msg
    int w_fd, w_sz, oflags;
    struct sigaction action;
    w_fd = open("/dev/mytimer", O_RDWR);
    if (w_fd == -1) {  //if (pFile==NULL) {
        printf("mytimer dev module isn't loaded\n");
        return -1;
    }
    if (mode) {
        w_sz = write(w_fd, s, strlen(s));
        close(w_fd);
        return 0;
    }

    // Setup signal handler
    memset(&action, 0, sizeof(action));
    action.sa_handler = readFromDev;
    action.sa_flags = SA_SIGINFO;
    sigemptyset(&action.sa_mask);
    sigaction(SIGIO, &action, NULL);
    fcntl(w_fd, F_SETOWN, getpid());
    oflags = fcntl(w_fd, F_GETFL);
    fcntl(w_fd, F_SETFL, oflags | FASYNC);

    // Write to file
    w_sz = write(w_fd, s, strlen(s));

    // Sleep
    pause();

    close(w_fd);
    return 0;
}

void readFromDev(int signo) {
    char line[271];
    int fd, sz;
    fd = open("/dev/mytimer", O_RDONLY);
    if (fd == -1) {
        printf("mytimer dev module isn't loaded\n");
        //return -1;
    }
    sz = read(fd, line, 270);
    line[sz] = '\0';
    if (sz > 0)
        printf("%s\n", line);
    close(fd);
}
