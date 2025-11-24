
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <poll.h>// not sure need it or not.
#include <unistd.h>
#include <sys/types.h>

void printManPage(void);

int fd;
char line[2560];
char sMsg[128];
void mytimer_signal(int snum);

static int kill_timer = 0;

int main(int argc, char **argv) {
	int count = 1, i = 0;
	char *flag;
	char *msg;
	char *sec;
	char *limiter = ";";
	char *cp_line;
	int ret;
	int index = 0;
	char destMsg[128];
	char *destMsgDup = (char *) malloc(120);
	char *timerStr = (char *) malloc(120);
	int oflags;
	struct sigaction action, oa;
	char *token;
	char *tokenMsg;
	char *firstLn = "Module name: mytimer";
	memset(line, 0, sizeof(line));
	fd = open("/dev/mytimer", O_RDWR);
	if (fd < 0) {
		printf("mytimer module isn't loaded");
		return 0;
	}
	memset(&action, 0, sizeof(action));
	action.sa_handler = mytimer_signal;
	action.sa_flags = SA_SIGINFO;
	sigemptyset(&action.sa_mask);
	sigaction(SIGIO, &action, NULL);
	fcntl(fd, F_SETOWN, getpid());
	oflags = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, oflags | FASYNC);

	// Check if list time
	if (argc == 2 && strcmp(argv[1], "-l") == 0) {
		if(read(fd, line, sizeof(line)>=0)){
			token = strtok(line, "\n");
			if (strcmp(token, firstLn) != 0){
				printf("%s\n", token);
			}
		}
	} else if (argc == 4 && strcmp(argv[1], "-s") == 0) { // Check if register new timer
		if (strtol(argv[2], NULL, 10) < 0) {
			printf("Time cannot be negative\n");
			//fclose(pFile);
			return -1;
		}
		if (read(fd, line, sizeof(line) >= 0)) {
			token = strtok(line, "\n");
			if (strcmp(token, firstLn) != 0){
				i = 0;
				while (token[i] != '\n'){
					if(token[i] == ' '){
						index = i;
					}
					i++;
				}
				strncpy(destMsg, token, index);
				strcpy(destMsgDup, destMsg);
				token = strtok(destMsg, " ");
				i = 0;
				while(token != NULL){
					token = strtok(NULL, " ");
					i++;
				}
				index = i - 2;
				tokenMsg = strtok(destMsgDup, " ");
				strcpy(timerStr, tokenMsg);
				for (i = 0; i < index; i++){
					tokenMsg = strtok(NULL, " ");
					strcat(timerStr, " ");
					strcat(timerStr, tokenMsg);
				}
				if (strcmp(timerStr, argv[3]) != 0){
					printf("A timer exists already!\n");
					return 0;
				} else {
					//update the timer
					strcpy(line, argv[1]);
					strcat(line, ";");
					strcat(line, argv[3]);
					strcat(line, ";");
					strcat(line, argv[2]);
					ret = write(fd, line, strlen(line));
					printf("Timer %s has now been reset to %s seconds!\n", argv[3], argv[2]);
				}
			} else {
				strcpy(line, argv[1]); 
				strcat(line, ";");
				strcat(line, argv[3]);
				strcat(line, ";");
				strcat(line, argv[2]);
				ret = write(fd, line, strlen(line));
				pause();
			}
		}
	} else if (argc == 2 && strcmp(argv[1], "-r") == 0) { // Check if remove timer
		strcpy(line, argv[1]);
		ret = write(fd, line, strlen(line));
		kill_timer = 1;
	} else { // Otherwise invalid
		printManPage();
	}
	memset(line, 0, sizeof(line));
	free(timerStr);
	free(destMsgDup);
	return 0;
}

void printManPage() {
	printf("Error: invalid use.\n");
	printf(" ktimer [-flag] [message]\n");
	printf(" -l: List to stdout expiration time in seconds and message\n");
	printf(" -s [SEC] [MSG]: Start timer after [SEC] will print [MSG]\n");
	printf("                 If active timer exists, reset to [SEC]\n");
}

void mytimer_signal(int snum) {
	char *token;
	int i;
	int index = 0;
	char destMsg[128];
	char *destMsgDup = (char *) malloc(120);
	char *timerStr = (char *) malloc(120);
	char *tokenMsg;

	read(fd, line, sizeof(line));
	token = strtok(line, "\n");	
	i = 0;
	while (token[i] != '\n'){
		if(token[i] == ' '){
			index = i;
		}
		i++;
	}
	strncpy(destMsg, token, index);
	strcpy(destMsgDup, destMsg);
	token = strtok(destMsg, " ");
	i = 0;
	while(token != NULL){
		token = strtok(NULL, " ");
		i++;
	}
	index = i - 2;
	tokenMsg = strtok(destMsgDup, " ");
	strcpy(timerStr, tokenMsg);
	for (i = 0; i < index; i++){
		tokenMsg = strtok(NULL, " ");
		strcat(timerStr, " ");
		strcat(timerStr, tokenMsg);
	}
	if (kill_timer == 1){
		kill_timer = 0;
	} else {
		printf("%s\n", timerStr);
	}
	free(destMsgDup);
	free(timerStr);
}