
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
  /*********************************************************
  Usage:
    ./ktimer [flag] [message]
	 
	-l (list timers)
	-s (register new timer)
    -m (mount)
		
******************************************************/

void printManPage(void);

int main(int argc, char **argv) {
	char line[256];
	char temp[140];
	int ii, count = 0;

	FILE * timer_module;
	timer_module = fopen("/dev/mytimer", "w");


	/* Check to see if the module successfully has mknod run
	   Assumes that nibbler is tied to /dev/mytimer */

	if (timer_module==NULL) {
		fputs("mytimer module isn't loaded\n",stderr);
		return -1;
	}

	// Check for list (-l)
	if (argc == 2 && strncmp(argv[1], "-l",2) == 0) {
		fprintf(timer_module,"?\0");
	}else if (argc == 3 && strncmp(argv[1], "-m",2) == 0) {
		sprintf(temp,"%s~",argv[2]);
		fprintf(timer_module,"%s\0",temp);
	}else if(argc == 4) {
		sprintf(temp,"%s?%s",argv[3],argv[2]);
		fprintf(timer_module,"%s \0",temp);
	}

	// Otherwise invalid
	else {
		printManPage();
	}

	fclose(timer_module);
	return 0;
}

void printManPage() {
	printf("Error: invalid use.\n");
}
