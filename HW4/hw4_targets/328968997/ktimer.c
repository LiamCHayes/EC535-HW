#define _GNU_SOURCE

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/******************************************************

  Usage:
    ./ktimer [flag] [message]

        -l (list)
    List to stdout expiration time in seconds and message.
        -s [SEC] [MSG]
    Register new timer after [SEC] seconds will print
    message [MSG]. If active timer with same message
    exists, reset timer to [SEC].
        -r
        Remove all timers

  Examples:
        ./ktimer -s 20 "Hello"
                Set a timer that prints "Hello" after 20 seconds

******************************************************/

void printManPage(void);
void sighandler(int);

int main(int argc, char **argv) {
  char line[2560];
  char *temp;
  char command[50];
  char *limiter = ";";
  //   char *msg;
  //   char *sec;
  unsigned long seconds;
  char pid[16];
  struct sigaction action;

  /* Check to see if the ktimer successfully has mknod run
     Assumes that mytimer is tied to /dev/mytimer */
  int pFile, oflags, procFile;
  pFile = open("/dev/mytimer", O_RDWR);
  if (pFile < 0) {
    printf("mytimer module isn't loaded\n");
    return -1;
  }

  memset(&action, 0, sizeof(action));
  action.sa_handler = sighandler;
  action.sa_flags = SA_SIGINFO;
  sigemptyset(&action.sa_mask);
  sigaction(SIGIO, &action, NULL);
  fcntl(pFile, F_SETOWN, getpid());
  oflags = fcntl(pFile, F_GETFL);
  fcntl(pFile, F_SETFL, oflags | FASYNC);

  // Check if list time
  if (argc == 2 && strcmp(argv[1], "-l") == 0) {
    read(pFile, line, 128);  // ! this cannot be strlen
    if (strlen(line) > 0) printf("%s", line);
  }

  // Check if register new timer
  else if (argc >= 4 && strcmp(argv[1], "-s") == 0) {
    if (strtol(argv[2], NULL, 10) < 0) {
      printf("Time cannot be negative\n");
      close(pFile);
      return -1;
    }
    temp = malloc(strlen(argv[1]) + strlen(argv[2]) + strlen(argv[3]) + 16 + 4);
    sprintf(pid, "%d", getpid());
    strcpy(temp, argv[1]);  // -s;hello;20;pid
    strcat(temp, ";");
    strcat(temp, argv[3]);  // message
    strcat(temp, ";");
    strcat(temp, argv[2]);  // time
    strcat(temp, ";");
    strcat(temp, pid);
    write(pFile, temp, strlen(temp));
    free(temp);

    read(pFile, line, 128);
    if (line[0] != '-') {
      pause();
      printf("%s", argv[3]);
    } else {
      printf("%s", line + 1);
    }
  } else if (argc == 2 && strcmp(argv[1], "-r") == 0) {
    strcpy(line, argv[1]);
    strcat(line, ";");
    write(pFile, line, strlen(line));  // send this to pause timers in driver
    strcpy(command, "killall ktimer");
    system(command);  // kill all ktimer instances to prevent them from hitting
  }

  // Otherwise invalid
  else {
    printManPage();
  }

  close(pFile);
  return 0;
}

void printManPage() {
  printf("Error: invalid use.\n");
  printf(" ktimer [-flag] [message]\n");
  printf(" -l: List to stdout expiration time in seconds and message\n");
  printf(" -s [SEC] [MSG]: Start timer after [SEC] will print [MSG]\n");
  printf("                 If active timer exists, reset to [SEC]\n");
  printf(" -r: Remove all timers\n");
}

// SIGIO handler
void sighandler(int signo) {}