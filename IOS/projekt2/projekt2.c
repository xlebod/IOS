#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <time.h>
#include <signal.h>
#include <ctype.h>
#include <sys/mman.h>

#define BOATMEMBERS_SEM "/xlebod00.ios.projekt.boatMembers"
#define CAPTAIN_SEM "/xlebod00.ios.projekt.captain"
#define mysleep(maxtime) {if(maxtime != 0)sleep((rand() % maxtime)+1);}
#define MMAP(shmvariable){shmvariable = mmap(NULL, sizeof(*(shmvariable)), PROT_READ | PROT_WRITE, \
MAP_SHARED, , 0);}
#define MUNMAP(shmvariable){munmap((shmvariable), sizeof(*(shmvariable)));}

int *hackerCount;
int *serfCount;
int *actionNumber;
int hackerCountID;
int serfCountID;
int actionNumberID;
sem_t *boatMembers = NULL;
sem_t *captain = NULL;

int init()
{
	if ((boatMembers = sem_open(BOATMEMBERS_SEM, O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED){
		printf("1\n");
		return -1;
	}

	if ((captain = sem_open(CAPTAIN_SEM, O_CREAT | O_EXCL , 0666, 0)) == SEM_FAILED){
		return -1;
	}

	/*if ((hackerCountID = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) == -1){
	 	printf("2\n");
	 	return -1;
	}

	if ((serfCountID = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) == -1){
	 	printf("3\n");
	 	return -1;
	}

	if ((actionNumberID = shmget(IPC_PRIVATE, sizeof(int),IPC_CREAT | 0666)) == -1){
		printf("4\n");
		return -1;
	}
	if ((hackerCount = shmat(hackerCountID, NULL, 0)) == NULL){
        printf("5\n");
        return -1;
    }

	if ((serfCount = shmat(serfCountID, NULL, 0)) == NULL){
        printf("6\n");
        return -1;
    }

    if ((actionNumber = shmat(actionNumberID, NULL, 0)) == NULL){
    	printf("7\n");
    	return -1;
    }*/
    MMAP(hackerCount);
    MMAP(serfCount);
    *hackerCount = 0;
    *serfCount = 0;
    return 0;
}

void ClearData()
{
/*	shmctl(hackerCountID, IPC_RMID, NULL);
	shmctl(serfCountID, IPC_RMID, NULL);
	shmctl(actionNumberID, IPC_RMID, NULL);*/
	MUNMAP(hackerCount);
	MUNMAP(serfCount);
	sem_close(boatMembers);
	sem_unlink(BOATMEMBERS_SEM);
	sem_close(captain);
	sem_unlink(CAPTAIN_SEM);
}

void Hacker()
{
	printf("HACKER IS HERE\n");
	exit(EXIT_SUCCESS);
}

void Serf()
{
	printf("SERF IS HERE\n");
	exit(EXIT_SUCCESS);
}

void HackerGenerator(int personCount, int hackerTime, int waitTime)
{
	for(int i = 0; i < personCount; i++){
		pid_t hackerID = fork();
		if (hackerID == 0){
			Hacker();
		}
		if (hackerID < 0){
			//error
		}
		hackerCount++;
		printf("HACKERS: %d\n",*hackerCount );
		mysleep(hackerTime);
		for(int i = 0; i < personCount; i++) wait(NULL);
	}
		exit(EXIT_SUCCESS);
}

void SerfGenerator(int personCount, int serfTime, int waitTime)
{
	for(int i = 0; i < personCount; i++){
		pid_t serfID = fork();
		if (serfID == 0){
			serfCount += 1;
			printf("SERFS: %d\n",*serfCount );
			Serf();
		}
		if (serfID < 0){
			//error
		}
		mysleep(serfTime);
		for(int i = 0; i < personCount; i++) wait(NULL);
	}
	exit(EXIT_SUCCESS);
}

int main(int argc, char const *argv[])
{
	if (argc != 7){
		fprintf(stderr, "Invalid amount of arguments. Argument count: %d\n",argc);
	}
	long personCount = 0;
	long hackerTime = 0;
	long serfTime = 0;
	long rowTime = 0;
	long waitTime = 0;
	long capacity = 0;
	char *ptr;
	// LOADING ARGUMENTS 
	// ./projekt2 P H S R W C
	for(int i = 3; i != argc*2; i++){
		int argument = (i/2);
		if ((i % 2) == 0){
			if (ptr[0] != '\0'){
				fprintf(stderr, "Invalid argument: %s\n",argv[argument-1]);
				return 1;
			}
		}
		if (i == 3){
			if((personCount = strtol(argv[argument], &ptr, 10)) < 0 || (personCount % 2) != 0){
				return 1;
			}
		}
		else if (i == 5){
			if((hackerTime = strtol(argv[argument], &ptr, 10)) < 0 || hackerTime > 2000){
				return 1;
			}
		}
		else if (i == 7){
			if((serfTime = strtol(argv[argument], &ptr, 10)) < 0 || serfTime > 2000){
				return 1;
			}
		}
		else if (i == 9){
			if((rowTime = strtol(argv[argument], &ptr, 10)) < 0 || rowTime > 2000){
				return 1;
			}
		}
		else if (i == 11){
			if((waitTime = strtol(argv[argument], &ptr, 10)) < 20 || waitTime > 2000){
				return 1;
			}
		}
		else if (i == 13){
			if((capacity = strtol(argv[argument], &ptr, 10)) < 5){
				return 1;
			}
		}
//		printf("P= %li; H = %li; S = %li; R = %li; W = %li; C = %li\n",personCount, hackerTime,
//			serfTime, rowTime, waitTime, capacity);
	}
	//INITIALIZING SEMAPHORES AND SHARED MEMORY VARIABLES
	if (init() == -1){
		ClearData();
		return -1;
	}

	pid_t hackerGeneratorID = fork();
	if(hackerGeneratorID == 0){
		HackerGenerator(personCount,hackerTime,waitTime);
		exit(EXIT_SUCCESS);
	}
	else if (hackerGeneratorID > 0){
		pid_t SerfGeneratorID = fork();
		if (SerfGeneratorID == 0){
			SerfGenerator(personCount,serfTime,waitTime);
		}
		if (SerfGeneratorID < 0){
			//error
		}
	}
	else {
		//error
	}
	wait(NULL);
	wait(NULL);

	ClearData();
	printf("%s\n","done, Bossman" );
	return 0;
}