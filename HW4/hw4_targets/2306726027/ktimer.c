#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

void printManPage(void);
void sighandler(int);

int main(int argc, char **argv) {
	char line[256];
	char msg[128];
	char s[256];
	char* token;
	unsigned long seconds;
	int ii, count = 1;
	int oflags;
	int i = 0,j = 0;
	struct sigaction action, oa;
	mode_t mode = S_IRUSR | S_IWUSR;
	int removed = 0;
	//File Open
	int file, fclose_error;
	//file=open("test.txt",O_RDWR | O_APPEND,mode);
	file=open("/dev/mytimer",O_RDWR,mode);
	if(file == -1){
		printf("Driver Module cannot be opened\n");
	}

	// Check if in read mode
	 if (argc == 2 && strcmp(argv[1], "-l") == 0) {
		read(file,line,256);
		printf("%s",line);
	}

	// Check if in write mode
	else if (argc == 4 && strcmp(argv[1], "-s") == 0) {	
		read(file,line,256);
		fclose_error=close(file);
		char* cmd_sec = argv[2];
		int c_sec;
		sscanf(cmd_sec,"%d",&c_sec);
		//printf("Seconds: %d\n",c_sec);
		if(c_sec <= 0){
			printf("Cannot have a timer set to zero seconds or below zero seconds\n");
			return -1;
		}
		file=open("/dev/mytimer",O_RDWR,mode);
		memset(&action, 0, sizeof(action));
		action.sa_handler = sighandler;
		action.sa_flags = SA_SIGINFO;
		sigemptyset(&action.sa_mask);
		sigaction(SIGIO, &action, NULL);
		fcntl(file, F_SETOWN, getpid());
		oflags = fcntl(file, F_GETFL);
		fcntl(file, F_SETFL, oflags | FASYNC);
		char* file_str = malloc(sizeof(char)*128+1);
		char* cmd_word = argv[3];
		char* secs_left = malloc(sizeof(char)*10+1);
		int delim_index=0;
		int seconds_left;
		int last_index=0;
		int last_index2=0;
		int k;

		//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
		strcat(s,argv[1]);
		strcat(s,";");
		strcat(s,argv[3]);
		strcat(s,";");
		strcat(s,argv[2]);
		strcat(s,"\n");
		i = 0;
		for(i = 0;i<strlen(line);i++){
			if(line[i] == ' '){
				last_index = i;
			}
			//printf("Index: %d\n",last_index);
		}
		
		if(line[0] != '\0'){
			i=0;
			while(i < last_index){ 
				file_str[i] = line[i];
				i++;
			}
			file_str[i] = '\0';
			//printf("String: %s\n",file_str);
		}
	
		
		if(line[0] == '\0'){
			if(write(file,(char *)s,strlen(s)) == -1){
				printf("Error\n");
				return -1;
			}
			else{
				pause();
				fclose_error=close(file);
				file=open("/dev/mytimer",O_RDWR,mode);
				read(file,line,256);
				//Find length of string
				if(line[0] == '\0'){
					printf("%s\n",argv[3]);
				}
				else{
					k=0;
					for(k = 0;k<strlen(line);k++){
						if(line[k] == ' '){
							last_index2 = k;
						}
					}
					last_index2 = last_index2 + 1;
					//printf("Last index: %d\n",last_index2);

					/* while(line[delim_index] != ' '){
						delim_index++;
					} */
					//delim_index++;
					j=0;
					//printf("Last: %d\n",delim_index);
					while(line[last_index2] != '\0'){
						secs_left[j] = line[last_index2];
						j++;
						last_index2++;
					}
					sscanf(secs_left,"%d",&seconds_left);
					if(seconds_left == 0){
						printf("%s\n",argv[3]);
					}
				}
				
				
			}
		}
		else if(strcmp(argv[3],file_str)==0){
			write(file,(char *)s,strlen(s));
			//printf("Update\n");
		}
		else{
			printf("A timer exists already!\n");
		}
		free(file_str);
		free(secs_left);
    }

    else if (argc == 2 && strcmp(argv[1], "-r") == 0) {
		strcat(s,argv[1]);
		strcat(s,";");
		strcat(s,"\n");
		write(file,s,strlen(s)); //Puts -r in the driver file
    } 

	else if(argc = 3 && strcmp(argv[1], "-m") == 0){
		count = strtol(argv[2],NULL,10);
		if(count < 1) {
			printf("Count must be greater than 0\n");
			fclose_error=close(file);
			return -1;
		}
		else{
			strcat(s,argv[1]);
			strcat(s,";");
			strcat(s,argv[2]);
			strcat(s,"\n");
			write(file,s,strlen(s));
		}
	}
	// Otherwise invalid
	else {
		printManPage();
	}

	fclose_error=close(file);
	if(fclose_error == -1){
		printf("The file cannot be closed\n");
	}
	return 0;
}

void printManPage() {
	printf("Error: invalid use.\n");
	printf(" ktimer [-flag]\n");
	printf(" -l: List current queue\n");
	printf(" -s: [Seconds] [message] Store timer and message\n");
	printf(" -r: Remove timer(s)\n");
}

void sighandler(int signo)
{
	//printf("Awaken!\n");
}