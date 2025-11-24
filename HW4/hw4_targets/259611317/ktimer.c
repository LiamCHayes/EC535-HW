#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
/******************************************************
  Usage:
    ./fasync_ul [flag] [message]
	 
	-r (read)
	-w (write)
	
  Examples:
	./fasync_ul -r
		Print whatever message that the fasync module is holding

	./fasync_ul -w ThisIsAMessage
		Write the string "ThisIsAMessage" to the fasync module
	
******************************************************/
int myflag=0;

void printManPage(void);
void sighandler(int);

int main(int argc, char **argv) {
	char line[256];
	int mylen, pad;
	int iii;


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////modify




	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////modify
	int pFile, oflags, pFile2;
	struct sigaction action, oa;
	int ii, count = 0;
	int jj;
	  //char *cc = (char *) calloc(128, sizeof(char)); 
	char cc[4096];
	char cc2[150];
	char  arg_saved [128];
	// Opens to device file
	pFile = open("/dev/mytimer", O_RDWR);

	if (pFile < 0) {
		fprintf (stderr, "mytimer module isn't loaded\n");
		return 1;
	}


	/* Check to see if the fasync successfully has mknod run
	   Assumes that fasync is tied to /dev/fasync */


	// Check if in read mode
	if (argc == 2 && strcmp(argv[1], "-l") == 0) {
		//while (fgets(line, 256, pFile) != NULL) {
			//printf("%s\n", line);
		//}
	//pFile2 = open("/proc/mytimer", O_RDWR);
	//if (pFile < 0) {
	//	fprintf (stderr, "mytimer module isn't loaded\n");
	//	return 1;
	//}
		//printf("hello");
		read(pFile, cc, 4096);
		printf("%s\n", cc);
		
	}

	else if (argc == 2 && strcmp(argv[1], "-r") == 0) {
		char mycommand[50];
		write(pFile, "_r", 150);
		strcpy(mycommand, "killall ktimer");
		system(mycommand);
		
	}

	// Check if in write mode
	else if (argc == 4 && strcmp(argv[1], "-s") == 0) {
		// if (myflag==0){
		// myflag=1;
		strcpy(arg_saved, argv[3]);
		char mytext[150];
		strcat(mytext, argv[1]);
		strcat(mytext, "_");

		// write(pFile, argv[1], 2);
		// write(pFile, "_", 1);

	 count = 0; 
while(argv[2][++count] != '\0');
		mylen=count;
		pad=10-mylen;
		for (iii=0; iii<pad;iii++){
			// write(pFile, "0", 1);
			strcat(mytext, "0");
		}
		strcat(mytext, argv[2]);
		// write(pFile, argv[2], strlen(argv[2]));
		strcat(mytext, "_");
		// write(pFile, "_", 1);
		strcat(mytext, argv[3]);
		// write(pFile, argv[3], strlen(argv[3]));
		strcat(mytext, "_");
		strcat(mytext, "1");
		jj=write(pFile, mytext, 150);
		if (jj==3){

		}
		else if (jj==4){
			printf("A timer is already exists!\n");

		}
		else if (jj==5){
			printf("Timer has been reset to %s seconds!\n", argv[2]);

		}
		//read(pFile, cc2, 150);
		//printf("%s\n", cc2);

////////////////////////////////////////////////////////////////modify
	// Setup signal handler
	memset(&action, 0, sizeof(action));
	action.sa_handler = sighandler;
	action.sa_flags = SA_SIGINFO;
	sigemptyset(&action.sa_mask);
	sigaction(SIGIO, &action, NULL);
	fcntl(pFile, F_SETOWN, getpid());
	oflags = fcntl(pFile, F_GETFL);
	fcntl(pFile, F_SETFL, oflags | FASYNC);

	// Write to file.
	//write(pFile, "Nothing", 8);
	
	// Waits.
	//printf("Sleep!\n");

	pause();
	// myflag=0;
// }
// else{

// 		char mytext2[150];
// 		if (strcmp(argv[3], arg_saved)!=0){ //not equal
// 			printf("A timer exists already!\n");
// 		}
// else{
// 		strcat(mytext2, argv[1]);
// 		strcat(mytext2, "_");

// 		// write(pFile, argv[1], 2);
// 		// write(pFile, "_", 1);

// 	 count = 0; 
// while(argv[2][++count] != '\0');
// 		mylen=count;
// 		pad=10-mylen;
// 		for (iii=0; iii<pad;iii++){
// 			// write(pFile, "0", 1);
// 			strcat(mytext2, "0");
// 		}
// 		strcat(mytext2, argv[2]);
// 		// write(pFile, argv[2], strlen(argv[2]));
// 		strcat(mytext2, "_");
// 		// write(pFile, "_", 1);
// 		strcat(mytext2, argv[3]);
// 		// write(pFile, argv[3], strlen(argv[3]));
// 		strcat(mytext2, "_");
// 		strcat(mytext2, "2");
// 		write(pFile, mytext2, 150);}
// }
////////////////////////////////////////////////////////////////
	}

	// Otherwise invalid
	else {
		printManPage();
	}

	printf("%s\n", argv[3]);
	// Closes.
	close(pFile);
	return 0;
}

void printManPage() {
	printf("Error: invalid use.\n");
	printf(" fasync_ul [-flag] [message]\n");
	printf(" -r: read from the fasync module\n");	
	printf(" -w: write [message] to the fasync module\n");
}

// SIGIO handler
void sighandler(int signo)
{
	//printf("Awaken!\n");
}




