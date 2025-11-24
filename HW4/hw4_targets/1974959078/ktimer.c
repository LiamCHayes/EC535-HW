#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
  char line[256];
  unsigned int time;
  int i;
  char in[256];
  int print, print2;
  print = 0;
  print2 = 0;
  FILE * pFile;
   pFile = fopen("/dev/mytimer", "r+");
  if (pFile == NULL) {
    fputs("mytimer module isn't loaded\n", stderr);
    return -1;
    } 

  if(argc == 2 && strcmp( argv[1], "-l") == 0) {
    // Add a signal to know later what type of read it needs
    sprintf(line, "<");
    fputs(line, pFile);
    print2 = 1;
  }
  else if (argc == 4 && strcmp( argv[1],"-s") == 0) {
    time = (unsigned int) argv[2]; // Saves the time
    // Line is used a buffer to stated the correct input
    sprintf(line,"%s;%s",argv[3],argv[2]);
	  fputs(line, pFile);
    print = 1;
    }
  else if (argc == 3 && strcmp( argv[1], "-m") == 0) {
    // "?" is used to signal what type of input it is
    sprintf(line,"?%s", argv[2]);
    fputs(line, pFile);
      }
  else
    // To know if it was typed incorrectly
  	printf("Fail\n");

  fclose(pFile);
  for(i = 0; i < 5; i++) {
  pFile = fopen("/dev/mytimer", "r+");
  if(print2)
  {
      fgets(in, 256, pFile);
      if(in[0] != '>' && in[0] != '\0')
       printf("%s\n", in);
     }
  fclose(pFile);
  }

  pFile = fopen("/dev/mytimer", "r+");
  if(print)
  {
    fgets(in,256, pFile);
    if(in[0] == 'T' || in[0] == 'A') //Only certain strings should be printed
     printf("%s", in);
    print = 0;
  }

  fclose(pFile);

	return 0;
}
