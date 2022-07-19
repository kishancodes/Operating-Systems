#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <semaphore.h>

extern int shmid;
extern struct nLicenses *shm;

struct nLicenses
{
	int availLicenses; // Total number of licenses
	int childProc;	   // Number of child processes
	int processes;	   // Number of processes currentlyrunning
	sem_t semempty;	   // Semaphore for empty slots
	sem_t semfull;	   // Semaphore for full slots
	int block;		   // Flag for if all available liceses are being used
};

void addTime(FILE *ofptr);
int childCheck(pid_t child[], int size);
void signalHandler();
void killProcesses();

void print_help();
void print_usage();

extern int
getlicense();
extern int returnlicense();
extern int initlicense();
extern void addtolicenses(int n);
extern void removelicenses(int n);
extern void logmsg(char *pid, char *c, char *repeat);

#endif
