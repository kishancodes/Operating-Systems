#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

// link custom external headeer files
#include "licenseobj.h"
#include "config.h"

// Declare *license(pointer) to hold license object
struct License *license;

int getlicense(){//Blocks until a license is available
        if (license->nlicenses > 0){
                (license->nlicenses)--;
        }
        else{
                wait(&license->nlicenses);
                //printf("Blocking for license....\n");
                return 0;
        }return 0;
}

int returnlicense(){
        //Increments the number of available licenses
        (license->nlicenses)++;
        return 0;
}

int initlicense(int number){
        //Performs any needed initialization of the license object
        license->nlicenses = number;
        return 0;
}

int addtolicenses(int n){
        //Adds n licenses to the number available
        license->nlicenses += n;
        return 0;
}

int removelicenses(int n){
        //Decrements the number of licenses by n
        license->nlicenses -= n;
        return 0;
}

// This function writes the specifed message to the log file. There is inly one log file. This function
// will treat the log file as a critical resource. It will open the file to append the message and close the file after
// appending the message.
int logmsg(const char *msg){
        int i;
        char *timestamp = (char *)malloc(sizeof(char) * 20);
        time_t now;
        struct tm *t;
        char *timeformat = "%Y-%m-%d %H:%M:%S";

        // Get the current time
        now = time(NULL);
        t = localtime(&now);
        strftime(timestamp, 20, timeformat, t);
        for (i = 0; i < 20; i++)
        {
                if (license->choosing[i] == false)
                {
                        license->choosing[i] = true;
                        break;
                }
        }
        if (i == 20)
        {
                for (i = 0; i < 20; i++)
                {
                        if (license->number[i] < license->number[i + 1])
                        {
                                license->number[i] = license->number[i + 1];
                                license->choosing[i] = true;
                                break;
                        }
                }
        }
        license->number[i] = 0;

        FILE *fp;
        fp = fopen("licenselog.log", "a");
        //returns -1 if filepointer is null
        if (fp == NULL)
        {
                perror("Failed to open log");
                return -1;
        }
        //writes to file
        else
        {
                fprintf(fp, "%s: %s\n", timestamp, msg);
                fflush(fp);
                fclose(fp);
                return 0;
        }
        return 0;
}

int max(int num1, int num2){
        if (num1 > num2)
                return num1;
        else
                return num2;
}

