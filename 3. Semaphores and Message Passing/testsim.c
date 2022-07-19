#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

int shmid;
struct nLicenses *shm;

int main(int argc, char *argv[])
{
	signal(SIGINT, signalHandler);

	int sleepT = atoi(argv[1]);
	int repeatF = atoi(argv[2]);

	// key for the shared memory segment
	key_t key = 9874;

	// Get the shared memory segment created using the key
	if ((shmid = shmget(key, sizeof(struct nLicenses) * 2, 0666)) < 0)
	{
		perror("testsim: Error: shmget ");
		exit(1);
	}

	// Attach shared memory to child
	if ((shm = (struct nLicenses *)shmat(shmid, NULL, 0)) == (struct nLicenses *)-1)
	{
		perror("testsim: Error: shmat ");
		exit(1);
	}

	int c, j, n = shm->childProc;
	int max = 0;
	for (c = 0; c < repeatF; c++)
	{
		// Critical section
		sem_wait(&shm->semfull);
		pid_t id = getpid();
		char pid[50];
		char num[50];

		sprintf(pid, "%d", id);
		sprintf(num, "%d", (c + 1));

		printf("Printing msg to file: %s %s of %s\n", pid, num, argv[2]);

		// Print to output fie using logmsg
		logmsg(pid, num, argv[2]);

		// Sleep before looping again
		sleep(sleepT);

		// Exit critical section
		sem_post(&shm->semempty);
	}

	//shm->processes--;
	addtolicenses(1);
	killProcesses();
	return 0;
}

// signal handler
void signalHandler()
{
	pid_t id = getpid();
	killProcesses();
	killpg(id, SIGINT);
	exit(1);
}

// kill all processes
void killProcesses()
{
	shmdt(shm);
	sem_destroy(&shm->semempty);
	sem_destroy(&shm->semfull);
}
