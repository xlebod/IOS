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
#include <unistd.h>
#include <sys/mman.h>

#define mysleep(maxtime) {if(maxtime != 0)sleep((rand() % maxtime)+1);}

//SEM NAMES AND SMH KEYS
#define BOATMEMBERS_SEM "/xlebod00.ios.projekt.boatMembers"
#define CAPTAIN_SEM "/xlebod00.ios.projekt.captain"
#define shmKEYaction "/xlebod00-ios-actionnum"
#define shmKEYserf "/xlebod00-ios-serf"
#define shmKEYhacker "/xlebod00-ios-hacker"
#define shmSIZE sizeof(int)

//SHM VARIABLES
int *shm_actionNumber;
int *shm_hackerCount;
int *shm_serfCount;

//SEMAPHORES
sem_t *boatMembers = NULL;
sem_t *captain = NULL;


int SemInit()
{
	if ((boatMembers = sem_open(BOATMEMBERS_SEM, O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED){
		printf("1\n");
		return -1;
	}

	if ((captain = sem_open(CAPTAIN_SEM, O_CREAT | O_EXCL , 0666, 0)) == SEM_FAILED){
		return -1;
	}
	return 0;
}

void ClearSem()
{
	sem_close(boatMembers);
	sem_unlink(BOATMEMBERS_SEM);
	sem_close(captain);
	sem_unlink(CAPTAIN_SEM);
}


void actionNumber_int()
{
	int shmID_actionNumber;
	shmID_actionNumber = shm_open(shmKEYaction, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	ftruncate(shmID_actionNumber, shmSIZE);
	shm_actionNumber = (int *)mmap(NULL, shmSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmID_actionNumber
		, 0);
	close(shmID_actionNumber);
	if(shm_actionNumber == (void *)-1){
		fprintf(stderr, "Chyba pri pristupu k shared memory variable: actionNum");
		exit(EXIT_FAILURE);
	}
	shm_actionNumber[0] = 0;
	munmap(shm_actionNumber, shmSIZE);
}

void actionNumber_open()
{
	int shmID_actionNumber;
    shmID_actionNumber = shm_open(shmKEYaction, O_RDWR, S_IRUSR | S_IWUSR);
    shm_actionNumber = (int*)mmap(NULL, shmSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmID_actionNumber, 0);
    close(shmID_actionNumber);
}

void actionNumber_close()
{
    munmap(shm_actionNumber, shmSIZE);
}

void actionNumber_unlink()
{
    shm_unlink(shmKEYaction);
}

void serfCount_int()
{
	int shmID_serfCount;
	shmID_serfCount = shm_open(shmKEYserf, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	ftruncate(shmID_serfCount, shmSIZE);
	shm_serfCount = (int *)mmap(NULL, shmSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmID_serfCount
		, 0);
	close(shmID_serfCount);
	if(shm_serfCount == (void *)-1){
		fprintf(stderr, "Chyba pri pristupu k shared memory variable: actionNum");
		exit(EXIT_FAILURE);
	}
	shm_serfCount[0] = 0;
	munmap(shm_serfCount, shmSIZE);
}

void serfCount_open()
{
	int shmID_serfCount;
    shmID_serfCount = shm_open(shmKEYserf, O_RDWR, S_IRUSR | S_IWUSR);
    shm_serfCount = (int*)mmap(NULL, shmSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmID_serfCount, 0);
    close(shmID_serfCount);
}

void serfCount_close()
{
    munmap(shm_serfCount, shmSIZE);
}

void serfCount_unlink()
{
    shm_unlink(shmKEYserf);
}

void hackerCount_int()
{
	int shmID_hackersCount;
	shmID_hackersCount = shm_open(shmKEYhacker, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	ftruncate(shmID_hackersCount, shmSIZE);
	shm_hackerCount = (int *)mmap(NULL, shmSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmID_hackersCount
		, 0);
	close(shmID_hackersCount);
	if(shm_hackerCount == (void *)-1){
		fprintf(stderr, "Chyba pri pristupu k shared memory variable: actionNum");
		exit(EXIT_FAILURE);
	}
	shm_hackerCount[0] = 0;
	munmap(shm_hackerCount, shmSIZE);
}

void hackerCount_open()
{
	int shmID_hackersCount;
    shmID_hackersCount = shm_open(shmKEYhacker, O_RDWR, S_IRUSR | S_IWUSR);
    shm_hackerCount = (int*)mmap(NULL, shmSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmID_hackersCount, 0);
    close(shmID_hackersCount);
}

void hackerCount_close()
{
    munmap(shm_hackerCount, shmSIZE);
}

void hackerCount_unlink()
{
    shm_unlink(shmKEYhacker);
}

