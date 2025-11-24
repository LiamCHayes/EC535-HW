#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int main(int argc, char ** argv) {
    FILE *fp;

    if(strcmp("-l",argv[1]) == 0) {
        char line[256];
        fp = fopen("/dev/mytimer","r+");
        while (fgets(line, 256, fp) != NULL) {
            printf("%s\n",line);
        }
    }
    if(strcmp("-s",argv[1]) == 0) {
        fp = fopen("/dev/mytimer", "w+");
        char msg[256]; //10 for delimiter + hex. representation of seconds
        msg[0] = 's';
        msg[1] = '_';
        strcat(msg,argv[3]);
        char delimiter[] = "_";
        strcat(msg, delimiter); // "_" is the delimiter
        unsigned int secs;
        sscanf(argv[2],"%u",&secs);
        char hexa[9];
        sprintf(hexa,"%08x",secs); //by storing it in hex, I can convert it one by one in the kernel
        strcat(msg,hexa);
        fprintf(fp, msg);
    }
    if(strcmp(argv[1],"-m") == 0) {
        fp = fopen("/dev/mytimer", "w+");
        char* number = argv[2];
        char result_ [16];
        sprintf(result_,"m_%s",number);
        fprintf(fp,result_);
    }
    fclose(fp);
    return 0;
}
