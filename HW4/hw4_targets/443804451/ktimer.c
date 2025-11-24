#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

char buf[128];
char str[128];
char prev[128];

void sighandler(int);

int main(int argc, char **argv) {

	int oflags;
	struct sigaction action;

	int pFile = open("/dev/mytimer", O_RDWR);
	int rFile = open("/proc/mytimer", O_RDONLY);

	char line[140];

	// Setup signal handler
	memset(&action, 0, sizeof(action));
	action.sa_handler = sighandler;
	action.sa_flags = SA_SIGINFO;
	sigemptyset(&action.sa_mask);
	sigaction(SIGIO, &action, NULL);
	fcntl(pFile, F_SETOWN, getpid());
	oflags = fcntl(pFile, F_GETFL);
	fcntl(pFile, F_SETFL, oflags | FASYNC);


	if (pFile < 0) {
		fprintf (stderr, "mytimer module isn't loaded\n");
		return -1;
	}

	// read
	if (argc == 2 && strcmp(argv[1], "-l") == 0) {
		read(rFile,str,128);
	}

	//write
	if (argc > 3 && strcmp(argv[1], "-s") == 0) {
		strcpy(line, argv[2]);
		strcpy(buf, argv[3]);
		for (int i=4; i<argc; i++) {
			strcat(buf, " ");
			strcat(buf, argv[i]);
		}
		strcat(line, " ");
		strcat(line, buf);
		if (strcmp(prev,buf)==0) {
			printf("A timer exists already!\n");
		} else {
			write(pFile, line, 140);
			strcpy(prev,buf);
			pause();
		}
	}

	close(pFile);
	close(rFile);

	return 0;
}

void sighandler(int signo)
{
	printf("%s\n", buf);
}
