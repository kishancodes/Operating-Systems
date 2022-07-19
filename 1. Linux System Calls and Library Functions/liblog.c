#include <string.h>
#include <stdlib.h>
#include "liblog.h"

data_t logs[200]; 
int size = 0; //global size for memory allocation and clearence ir a

int addmsg ( const char type, const char * msg ) {
    // create data structure
    if (type == 'I' || type == 'W' || type == 'E' || type == 'F') {
        if (msg == NULL) {
            perror("driver: Error: Empty errors file.");
            return -1;
        } else {
            logs[size].type = type;
            logs[size].string = getlog();
            strcpy(logs[size].string, msg);
            logs[size].time = time(NULL);
            size++;
            //exit the program
            return 0;
        }
    }
    printf("%c", type);
    return -1;
}

void clearlog() {
    size = 0; // clear dynamic memory and empty list of log messages
}

char * getlog() {
    return malloc(sizeof(char) * MAX_MESSAGE_LENGTH); // dynamicaly allocate memory according to the message size.
}

int savelog ( char * filename ) {
    // save messages to the disk
    FILE* fp;
    int i = 0;
    fp = fopen(filename, "w"); // open the file containing the error messages
    // run loop to save the array into log file
    for (i = 0; i < size; i++) {
        char timeString[20];

        struct tm * time_info;
        time_info = localtime(&(logs[i].time));

        strftime(timeString, 9, "%H:%M:%S", time_info); // time stamp
        fprintf(fp, "%c %s %s\n", logs[i].type, logs[i].string, timeString); // save the error and time stamp into the log file.
    }
    // return to driver.c
    return -1;
}