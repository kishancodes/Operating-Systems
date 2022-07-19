#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "config.h"

#define BUFFERSIZE 100

// Declare global variables
int shmid;
struct nLicenses *shm;

int main(int argc, char *argv[])
{
	int numLicenses = 0;
	int childCount = 0;
	int terminationTime = 20;
	signal(SIGINT, signalHandler);

	// Parse command line arguments
	if (argc == 2 && !strcmp(argv[1], "-h"))
	{
		print_help();
		return 0;
	}
	else if (argc == 4 && !strcmp(argv[1], "-t"))
	{
		terminationTime = atoi(argv[2]);
		// allowing only number for thr processes
		if (strspn(argv[3], "0123456789") == strlen(argv[3]))
		{
			numLicenses = atoi(argv[3]);
			if (numLicenses <= 0)
			{
				printf("runsim: Error: Integer must be greater than or equal to 1\n");
				printf("Usage: ./runsim [option...] n < testing.data -- where n is an integer greater than or equal to 1\n");
				return 0;
			}
		}
	}
	else if (argc > 4)
	{
		printf("runsim: Error: Invalid argument: %s\n", argv[1]);
		printf("Usage: for help use option -h\n\n");
		printf("Usage: ./runsim [option...] n < testing.data -- where n is an integer greater than or equal to 1\n");
		return 0;
	}
	else if (argc == 2 && strspn(argv[1], "0123456789") == strlen(argv[1]))
	{
		numLicenses = atoi(argv[1]);
		if (numLicenses <= 0)
		{
			printf("runsim: Error: Integer must be greater than or equal to 1\n");
			printf("Usage: ./runsim [option...] n < testing.data -- where n is an integer greater than or equal to 1\n");
			return 0;
		}
	}
	else
	{
		print_usage();
		return 0;
	}

	// Key for shared memory
	key_t key = 9874;

	// create the shared memory segment
	if ((shmid = shmget(key, sizeof(struct nLicenses) * 2, IPC_CREAT | 0666)) < 0)
	{
		perror("runsim: Error: shmget ");
		exit(1);
	}

	// attach the segment to the data space
	if ((shm = (struct nLicenses *)shmat(shmid, NULL, 0)) == (struct nLicenses *)-1)
	{
		perror("runsim: Error: shmat ");
		exit(1);
	}

	// Initialize shared memory
	shm->availLicenses = numLicenses;
	shm->processes++;
	initlicense();

	// get string from file
	char buffer[BUFFERSIZE];
	char lines[BUFFERSIZE][BUFFERSIZE];
	int i = 0, j = 0;
	char progName[50] = "./";
	char a2[50], a3[50];

	while (fgets(buffer, BUFFERSIZE, stdin) != NULL)
	{
		strcpy(lines[childCount], buffer);
		childCount++;
	}

	// create child processes
	shm->childProc = childCount;
	int runningProcesses = 0;
	int index = 0;
	pid_t pid, child[childCount];

	while (terminationTime > 0)
	{
		printf("Termination Time: %d\n", terminationTime);
		if (runningProcesses < 20)
		{
			while (getlicense() == 1)
			{
				if (numLicenses == 1)
				{
					terminationTime--;
					sleep(1);

					if (terminationTime < 0)
					{
						printf("runsim: Error: Did not complete all processes before alloted time limit -- terminating remaining child processes\n");
						signalHandler();
						exit(1);
					}
				}
			}
			// wait semaphore
			sem_wait(&shm->semempty);
			if (index < childCount)
			{

				for (i; lines[index][i] != ' '; i++)
				{
					progName[i + 2] = lines[index][i];
				}

				i++;

				for (i; lines[index][i] != ' '; i++)
				{
					a2[j] = lines[index][i];
					j++;
				}

				j = 0;
				i++;

				for (i; i < strlen(lines[index]) - 1; i++)
				{
					a3[j] = lines[index][i];
					j++;
				}
				i = 0;
				j = 0;

				removelicenses(1);

				// fork a child process
				pid = fork();
				child[index] = pid;
				runningProcesses++;
				shm->processes++;
				printf("running proc: %i\n", shm->processes);
				index++;
			}
			// post semaphore
			sem_post(&shm->semfull);

			if (pid == -1)
			{
				printf("runsim: Error: Fork error -- terminating program\n");
				killProcesses();
				exit(1);
			}
			else if (pid == 0)
			{
				char ch[50];
				sprintf(ch, "%d", index);
				printf("About to call testsim with arg: %s %s %s %s\n", progName, a2, a3, ch);
				execl(progName, "testsim", a2, a3, ch, (char *)NULL);
				perror("runsim: Error: ");
			}

			terminationTime--;
			sleep(1);
		}
		else
		{
			printf("runsim: Error: Too many processes running -- killing all then terminating\n");
			signalHandler();
			exit(1);
		}

		shm->processes--;
		int check = childCheck(child, childCount);
		if (check == 1)
		{
			break;
		}
	}

	// Check if there are any remaining child processes runnning
	if ((wait(NULL) > 0) && (shm->childProc != 0))
	{
		printf("runsim: Error: Did not complete all processes before alloted time limit -- terminating remaining child processes\n");
		signalHandler();
	}
	else
	{
		printf("Completed all %d processes successfully -- terminating program\n", childCount);
		killProcesses();
	}

	return 0;
}

// Check if any children processes are still runnning
int childCheck(pid_t child[], int size)
{
	int c, status;

	for (c = 0; c < size; c++)
	{
		pid_t wid;
		wid = waitpid(child[c], &status, WNOHANG);

		if (wid != 0)
		{
			child[c] = 0;
		}
	}

	for (c = 0; c < size; c++)
	{
		if (child[c] == 0)
		{
			continue;
		}
		else
		{
			return 0;
		}
	}

	return 1;
}

// signal handler
void signalHandler()
{
	pid_t id = getpgrp();
	killProcesses();
	killpg(id, SIGINT);
	exit(1);
}

// Terminate all processes
void killProcesses()
{
	shmctl(shmid, IPC_RMID, NULL);
	shmdt(shm);
	// destroys the semaphores
	sem_destroy(&shm->semempty);
	sem_destroy(&shm->semfull);
}

// print help menu
void print_help()
{
	printf("Usage: runsim [option...]\n\n");
	printf("Supported options:\n");
	printf("    -h          Display this information\n");
	printf("    -t <time>   Terminate after <time> seconds (default 20)\n");
}

// print usage
void print_usage()
{
	printf("Usage:  [option..]\n");
	printf("Try ` -h' for more information.\n");
}