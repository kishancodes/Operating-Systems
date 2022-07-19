#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

//declare a structure for license object to be used in c files.
struct License
{
        int nlicenses;
        bool choosing[20];
        int number[20];
};

/* specified declared meathods in the ass2.pdf */
int getlicense(void);
int returnlicense(void);
int initlicense(int number);
int addtolicenses(int n);
int removelicenses(int n);
int logmsg(const char *msg);