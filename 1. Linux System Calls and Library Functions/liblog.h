#include <time.h>
#include <stdio.h>

//set message length limit 

#define MAX_MESSAGE_LENGTH 300 

typedef struct data_struct {
    time_t time; // Time stamp
    char type; // Message type (I/W/E/F)
    char * string; // Message string
} data_t;

int addmsg ( const char type, const char * msg );
void clearlog();
char * getlog();
int savelog ( char * filename );