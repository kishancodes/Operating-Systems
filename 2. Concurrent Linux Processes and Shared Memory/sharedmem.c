#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(int argc, char *argv[])
{
        printf("Program :");
        for (int i = 0; i < argc; i++)
                printf("%s ", argv[i]);
        printf("\n\n");

        int key = 99999;
        int shmid;

        shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);

        if (shmid == -1)
                perror("shmget: ");
        printf("Segment id: %d\n", shmid);
}