#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <errno.h>

//link custom external header files
#include "licenseobj.h"

// Declare *license(pointer) to hold license object
extern struct License *license;

int main(int argc, char *argv[])
{
        time_t now;
        struct tm *tinfo;
        char tstring[9];

        int sleep_time = atoi(argv[1]);
        int repeat_factor = atoi(argv[2]);
        if (argc <= 2)
        {
                fprintf(stderr, "Usage: %s sleep_time repeat_factor\n", argv[1]);
                exit(1);
        }
        //execute loop with sleep and repeat
        for (int i = 1; i < repeat_factor; i++)
        {
                char str[10];
                time(&now);
                tinfo = localtime(&now);
                strftime(tstring, sizeof(tstring), "%H:%M:%S", tinfo);
                sleep(sleep_time);
                sprintf(str, " %d", getpid());
                strcat(tstring, str);
                logmsg(tstring);
        }
        return sleep_time;
}