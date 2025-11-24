
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

#define LINE_LENGTH 256
/******************************************************

  Usage:
    ./ktimer [flag] [message]

	-l (list)
    List to stdout expiration time in seconds and message.
	-s [SEC] [MSG]
    Register new timer after [SEC] seconds will print
    message [MSG]. If active timer with same message
    exists, reset timer to [SEC].
  -m [COUNT]
    Change number of active timers to [COUNT]. Default
    is 1 on module load.

  Examples:
	./ktimer -l
		Print whatever message that the ktimer module is holding

	./ktimer -w ThisIsAMessage
		Write the string "ThisIsAMessage" to the ktimer module

******************************************************/

static char *message;


void sighandler(int signo);

void printManPage(void);

int main(int argc, char **argv) {
	char line[LINE_LENGTH];
	//char write_to_kernel_line[LINE_LENGTH];
	char temp_message[LINE_LENGTH];
	//char temp_flag[2];
	int num;
	
	int count = 1, i = 0;
	char *flag;
	char *msg;
	char *sec;
	char *limiter = ";";
	char *cp_line;
	int pFile, oflags;
	struct sigaction action, oa;

	message = malloc(sizeof(char) * strlen(argv[3]));
	//memset(message,0,sizeof(char) * strlen(argv[2]));
	/* Check to see if the ktimer successfully has mknod run
	   Assumes that mytimer is tied to /dev/mytimer */

	/* pFile = open("/dev/mytimer", O_RDWR);
	if (pFile < 0) {
		fprintf (stderr, "mytimer module isn't loaded\n");
		return 1;
	} */

	// Check if list time
	if (argc == 2 && strcmp(argv[1], "-l") == 0) {
		pFile = open("/dev/mytimer", O_RDWR);
		if (pFile < 0) {
			fprintf (stderr, "mytimer module isn't loaded\n");
			return 1;
		}
		
		if(read(pFile, line, LINE_LENGTH)){
			//lseek(pFile, 0, SEEK_SET); // sets the file pointer back to the beginning
			printf("%s", line);
		}
		//close(pFile);
	}

	else if (argc == 2 && strcmp(argv[1], "-r") == 0) {
		pFile = open("/dev/mytimer", O_RDWR);
		if (pFile < 0) {
			fprintf (stderr, "mytimer module isn't loaded\n");
			return 1;
		}
		strcpy(line, argv[1]); // -s;hello;20
		strcat(line, ";");
		write(pFile, line, strlen(line));
		//lseek(pFile, 0, SEEK_SET);

		system("killall ktimer");
		//close(pFile);
	} 

	 // Check if register new timer
	else if (argc == 4 && strcmp(argv[1], "-s") == 0) {

		memset(message,0,sizeof(char) * strlen(argv[2]));

		pFile = open("/dev/mytimer", O_RDWR);
		if (pFile < 0) {
			fprintf (stderr, "mytimer module isn't loaded\n");
			return 1;
		}

		memcpy(message, argv[3], sizeof(char) * strlen(argv[3]));
		//message = argv[3];
		
		if (strtol(argv[2], NULL, 10) < 0) {
			printf("Time cannot be negative\n");
			close(pFile);
			return -1;
		}
		// Checks if it is already in /dev/mytimer AKA if the timer already exists
		if(read(pFile, line, LINE_LENGTH)){
			
			close(pFile); 
			pFile = open("/dev/mytimer", O_RDWR);
				if (pFile < 0) {
					fprintf (stderr, "mytimer module isn't loaded\n");
					return 1;
				}
				//			sscanf(line, "%s %s %s", temp_flag, temp_message, num);

			/* sscanf(line, "%s %s", temp_message, num);
			printf("num = %s \n", num);
			printf("temp_message = %s \n", temp_message); */

			memset(&action, 0, sizeof(action));
			action.sa_handler = sighandler;
			action.sa_flags = SA_SIGINFO;
			sigemptyset(&action.sa_mask);
			sigaction(SIGIO, &action, NULL);
			fcntl(pFile, F_SETOWN, getpid());
			oflags = fcntl(pFile, F_GETFL);
			fcntl(pFile, F_SETFL, oflags | FASYNC); 
			// if the message and line in dev are the same AKA timer exists, only update
			
			//printf("line = %s \n", line);
			//printf("message = %s \n", message);
			
			// if pFile is empty AKA first timer and you read null char
			if (line[0] == '\0'){
				memcpy(message, argv[3], sizeof(char) * strlen(argv[3]));
				strcpy(line, argv[1]); // -s;hello;20
				strcat(line, ";");
				strcat(line, argv[3]);
				strcat(line, ";");
				strcat(line, argv[2]);
				write(pFile, line, strlen(line));
				
				//lseek(pFile, 0, SEEK_SET);
				//printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA \n");
				//printf("line = %s \n", line);
				//printf("message = %s \n", message);

				pause();
				//close(pFile);
				
				//printf("I am unpaused \n");
			}
			else if (line[0] != '\0'){
				sscanf(line, "%s %d", temp_message, &num);
				//printf("num = %d \n", num);
				//printf("temp_message = %s \n", temp_message);

				if(strcmp(temp_message, message) == 0){
					//printf("I ALREADY HAVE THIS MESSAGE \n");
					strcpy(line, argv[1]); // -s;hello;20
					strcat(line, ";");
					strcat(line, argv[3]);
					strcat(line, ";");
					strcat(line, argv[2]);
					write(pFile, line, strlen(line));
					//close(pFile);
					//lseek(pFile, 0, SEEK_SET);
					}
				else{
						printf("A timer exists already! \n");
				}
			}
			else{
				printf("else cond \n");
				printf("line 2 = %s \n", line);
				/* sscanf(line, "%s %d", temp_message, &num);
				printf("num = %d \n", num);
				printf("temp_message = %s \n", temp_message); */
			}
			/* else if (strcmp(line, message) != 0){
				pFile = open("/dev/mytimer", O_RDWR);
				if (pFile < 0) {
					fprintf (stderr, "mytimer module isn't loaded\n");
					return 1;
				}
				close(pFile);
				printf("Too many timers \n");
			} */
			
		}
			
		
	}


// -r flag should just do system(killall)
	// Otherwise invalid
	else {
		printManPage();
	} 

	close(pFile);
	free(message);
	return 0;
}

void sighandler(int signo)
{
	printf("%s \n", message);
}

void printManPage() {
	printf("Error: invalid use.\n");
	printf(" ktimer [-flag] [message]\n");
	printf(" -l: List to stdout expiration time in seconds and message\n");
	printf(" -s [SEC] [MSG]: Start timer after [SEC] will print [MSG]\n");
	printf("                 If active timer exists, reset to [SEC]\n");
}
