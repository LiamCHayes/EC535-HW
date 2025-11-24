#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

#include <errno.h>
#include <time.h>
int errno;

void sighandler(int);
char messg[256];

int main(int argc, char **argv) {
	char *str;
	char line[256];	
	int pFile, oflags;
	struct sigaction action, oa;
	
	int n, i=0;

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
		n = read(pFile, line, 256);
		if(n > 0){
			for(i=0; i<n; i++){
				printf("%c", line[i]);
			}
			printf("\n");
			/*while(line[i] != '\0'){
				printf("%c", line[i]);
				i++;
			}
			printf("\n");*/
		}							
	}

	// Check if in set mode
	else if (argc == 4 && strcmp(argv[1], "-s") == 0) {
		strcpy(messg, argv[3]);
		str = strcat(argv[3], ";");
		str = strcat(str, argv[2]);
		write(pFile, str, strlen(str));
		if(errno==17){
			printf("Timer %s has been reset to %s seconds\n", messg, argv[2]);
		}		
		else if(errno==57){
			printf("A timer exists already!\n");
		}
		else{
			pause();
			close(pFile);
		}		
	}

	// Check if in modify mode
	else if (argc == 2 && strcmp(argv[1], "-r") == 0) {
		write(pFile, "DEL", 3);
		kill(-1, 15);		
	}

	// Otherwise invalid
	else {
		printf("Invalid call\n",stderr);
	}

	close(pFile);
	return 0;
	
}

// SIGIO handler
void sighandler(int signo)
{
	printf("%s\n", messg);
}
