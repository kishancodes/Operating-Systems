#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "liblog.h"

#define MAX_FILE_NAME 300 // set limit to filename character counts
//#define RAND_MAX 2 // time between 0 to 2 seconds

int main(int argc, char** argv) {
    int goptio; // to get the following commands from the cli input
    int secs = 1; // default but will 
    char filename[MAX_FILE_NAME]; // allocate the filename to the variable

    while((goptio = getopt(argc, argv, ":ht:x")) != -1) {
        switch(goptio){
        case 'h':
            printf("Synopsis: \n\n./driver [-h] [-t sec] [logfile]\n\n./driver [option]");
            //continue to exit and let user try again
            return 1;
        case 't':
            secs = atoi(optarg); //return int value for those that accept argument, elese 0
            break;
        }
    }

    if (optind < argc) {  // indexing
        strcpy(filename, argv[optind]);
    } else {
        strcpy(filename, "errors.log");
    }

    while (1) { // true else break the loop
        char type;
        char message[MAX_MESSAGE_LENGTH]; // allocate message length
        int result = scanf("%c", &type); // message type
        char* str = fgets(message, MAX_MESSAGE_LENGTH, stdin); 

        addmsg(type, message); // direct to liblog.c

        if (type == 'F') {
            savelog(filename); // invoke liblog.c savelog
            clearlog(); // invoke liblog.c clearlog
            break;
        }

        int limit = 1000 * 1000 * secs * 2; 
        int delay = rand() % limit;
        usleep(delay); // try to suspend currnet ongoing process for certain period
    }
    //exit the program
    return 0;

}