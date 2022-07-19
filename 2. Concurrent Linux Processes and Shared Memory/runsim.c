#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/time.h>

//link custom external header files
#include "config.h"
#include "licenseobj.h"

#define MAX_CANON 20
#define IPC_RESULT_ERROR (-1)
#define BUF_SIZE 1024
#define IP_RESULT_ERROR 1

// Declare *license(pointer) to hold license object
extern struct License *license;

// This function removed the shared memory segment
int removeSharedMemory(int shareMemoryID, void *shmaddr)
{
        int error = 0;
        if (shmdt(shmaddr) == -1)
                error = errno;
        if ((shmctl(shareMemoryID, IPC_RMID, NULL) == -1) && !error)
                error = errno;
        if (!error)
                return 0;
        errno = error;
        return -1;
}
 
//Display how to use information about the program on the screen
void usage(char *argv[])
{
        fprintf(stderr, "Usage: %s [number of processes] < testing.data\n", argv[0]);
}

//docommand will request a license from the license manager object.
// Notice that if the license is not available, the request function will go into a wait state.
void docommand(char *cline)
{
        pid_t cpid = 0;
        int gcCount = 0;
        char errMsg[255];
        getlicense();
        // Fork Grandchild and make calls 
        if ((cpid = fork()) < 0)
        {
                printf(errMsg, "Error: Failed to create child process");
                perror(errMsg);
                exit(1);
        }
        else if (cpid == 0)
        {
                gcCount++;
                getlicense();
                char delim[] = " ";
                char *binaryPath = strtok(cline, delim);
                char *arg1 = strtok(NULL, delim);
                char *arg2 = strtok(NULL, delim);
                char *arg3 = strtok(NULL, delim);
                execl(binaryPath, binaryPath, arg1, arg2, arg3, NULL);
        }
        // Return license if any of the grandchildren have finished their wait periods.
        else       
        {
                if ((cpid = waitpid(-1, NULL, WNOHANG)) == -1)
                {
                        printf("Waiting on grand child to exit...\n");
                        gcCount--;
                }
                returnlicense();
        }
}

// this function receives signal and print out the message and exit the program
void signalHandler(int sig)
{
        printf("Caught signal %d exiting....\n", sig);
        signal(SIGQUIT, SIG_IGN);
        kill(0, SIGTERM);
        time_t now;
        struct tm *tinfo;
        char tString[20];
        tinfo = localtime(&now);
        strftime(tString, sizeof(tString), "%H:%M:%S", tinfo);
        strcat(tString, " Exiting runsim");
        logmsg(tString);
        exit(0);
}

//User presses Ctrl-c and the interrupt is handeled here
static void interruptHandler(int s)
{
        int errsave;
        errsave = errno;
        write(STDERR_FILENO, "Exceeded time limit....", 1);
        raise(SIGINT);
        errno = errsave;
}

// this function will check if program is interrupted by user
static int setupinterrupt(void)
{
        struct sigaction act;
        act.sa_handler = interruptHandler;
        act.sa_flags = 0;
        return (sigemptyset(&act.sa_mask) || sigaction(SIGPROF, &act, NULL));
}

//this function sets up the timer
static int setTimer(void)
{
        struct itimerval value;
        value.it_interval.tv_sec = MAX_SECONDS;
        value.it_interval.tv_usec = 0;
        value.it_value = value.it_interval;
        return (setitimer(ITIMER_PROF, &value, NULL));
}


// Main driver for the entire project
int main(int argc, char *argv[])
{
        char errMsg[255];
        int numberOfProcess;
        signal(SIGINT, signalHandler); // handel the Ctrl-c from the user here
        if (setupinterrupt() == -1)
        {
                printf(errMsg, "%s: Error: Failed to set up handler for SIGPROF", argv[0]);
                perror(errMsg);
                return 1;
        }
        if (setTimer() == -1)
        {
                printf(errMsg, "%s: Error: Failed to set up ITIMER_PROF interval timer", argv[0]);
                perror(errMsg);
                return 1;
        }
        //set cpid to 0 for parent process id
        pid_t cpid = 0;
        int shareMemoryID, processCount; //set number of maximum children running at a time

        if (argc != 2)
        {
                usage(argv); // displays how to use the executable file.
                return 1;
        }
        else if (argc == 2)
                numberOfProcess = atoi(argv[1]); // get the number of processes from the argument passed in CLI
        else
                numberOfProcess = 5; // 5 by default
        if ((shareMemoryID = shmget(IPC_PRIVATE, sizeof(int), 0666 | IPC_CREAT)) == -1)
        {
                printf(errMsg, "%s: Error: Failed to create shared memory segment", argv[0]);
                perror(errMsg);
                return 1;
        }
        //send the data to the pointers 
        if ((license = (struct License *)shmat(shareMemoryID, NULL, 0)) == (void *)-1)
        {
                printf(errMsg, "%s: Error: Failed to attach shared memory segment", argv[0]);
                perror(errMsg);
                if (shmctl(shareMemoryID, IPC_RMID, NULL) == -1)
                {
                        printf(errMsg, "%s: Error: Failed to remove memory segment", argv[0]);
                        perror(errMsg);
                }
                return 1;
        }

        //allocate shared memories based on number of licenses
        initlicense(numberOfProcess);
        license->number[0] = 0;
        printf("Number of License: %d\n", license->nlicenses);
        getlicense(); // get licenses from the object
        processCount = 0;
        char buffer[MAX_CANON];
        //read from stdin
        while (fgets(buffer, sizeof buffer, stdin) != NULL)
        {
                if (processCount == NUM_PROCESSES)
                {
                        cpid = wait(NULL);
                        if (cpid != -1)
                        {
                                printf(errMsg, "%s: Error: Waited for child with pid %d because max processes reached", argv[0], (long)getpid(), cpid);
                                perror(errMsg);
                        }
                        processCount--;
                }
                //fork a child that calls docommand function
                if ((cpid = fork()) < 0)
                {
                        printf(errMsg, "%s: Error: Failed to create child process", argv[0]);
                        perror(errMsg);
                        if (removeSharedMemory(shareMemoryID, license) == -1)
                        {
                                printf(errMsg, "%s: Error: Failed to destroy shared memory segment", argv[0]);
                                perror(errMsg);
                        }
                        exit(1);
                }
                else if (cpid == 0)
                {
                        if ((license = (struct License *)shmat(shareMemoryID, NULL, 0)) == (struct License *)-1)
                        {
                                printf(errMsg, "%s: Error: Failed to attach shared memory segment", argv[0]);
                                perror(errMsg);
                                if (shmctl(shareMemoryID, IPC_RMID, NULL) == -1)
                                {
                                        printf(errMsg, "%s: Error: Failed to remove memory segment", argv[0]);
                                        perror(errMsg);
                                }
                                return 1;
                        }
                        processCount++;
                        license->number[processCount] = processCount;
                        //pass the input string from stdin to docommand which will execl the command (child)
                        char progstr[20];
                        strcpy(progstr, "./");
                        strcat(progstr, buffer);
                        docommand(progstr);
                        break;
                }
                else
                {
                        // wait for the child until execution
                        if ((cpid = waitpid(-1, NULL, WNOHANG)) == -1)
                        {
                                processCount--;
                        }
                }
        }
        time_t now;
        struct tm *tinfo;
        char tString[20];
        tinfo = localtime(&now);
        strftime(tString, sizeof(tString), "%H:%M:%S", tinfo);
        strcat(tString, " Exiting runsim");
        logmsg(tString);


        //finally remove the meomry used
        if (removeSharedMemory(shareMemoryID, license) == -1)
        {
                printf(errMsg, "%s: Error: Failed to destroy shared memory segment", argv[0]);
                perror(errMsg);
                return 1;
        }
        return 0;
}

