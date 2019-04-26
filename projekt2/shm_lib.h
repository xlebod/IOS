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
#define MEMACCESS_SEM "/xlebod00.ios.projekt.memAccess"
#define MOLO_SEM "/xlebod00.ios.projekt.molo"
#define EMPTYBOAT_SEM "/xlebod00.ios.projekt.emptyBoat"
#define DISEMBARK_SEM "/xlebod00.ios.projekt.boat"
#define shmKEYaction "/xlebod00-ios-actionnum"
#define shmKEYserf "/xlebod00-ios-serf"
#define shmKEYhacker "/xlebod00-ios-hacker"
#define shmKEYprocesses "/xlebod00-ios-processes"
#define shmKEYhackerBoat "/xlebod00-ios-hackerBoat"
#define shmKEYserfBoat "/xlebod00-ios-serfBoat"
#define shmSIZE sizeof(int)

//SHM VARIABLES
int *shm_actionNumber;
int *shm_hackerCount;
int *shm_serfCount;
int *shm_numberOfProcesses;
int *shm_hackerBoat;
int *shm_serfBoat;

//SEMAPHORES
sem_t *boatMembers = NULL;
sem_t *captain = NULL;
sem_t *memAccess = NULL;
sem_t *molo = NULL;
sem_t *emptyBoat = NULL;
sem_t *disembark = NULL;



int actionNumber_init()
{
	int shmID_actionNumber;
	shmID_actionNumber = shm_open(shmKEYaction, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	ftruncate(shmID_actionNumber, shmSIZE);
	shm_actionNumber = (int *)mmap(NULL, shmSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmID_actionNumber
		, 0);
	close(shmID_actionNumber);
	if(shm_actionNumber == (void *)-1){
		fprintf(stderr, "Chyba pri pristupu k shared memory variable: actionNum");
		return -1;
	}
	shm_actionNumber[0] = 0;
	munmap(shm_actionNumber, shmSIZE);
	return 0;
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

int serfCount_init()
{
	int shmID_serfCount;
	shmID_serfCount = shm_open(shmKEYserf, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	ftruncate(shmID_serfCount, shmSIZE);
	shm_serfCount = (int *)mmap(NULL, shmSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmID_serfCount
		, 0);
	close(shmID_serfCount);
	if(shm_serfCount == (void *)-1){
		fprintf(stderr, "Chyba pri pristupu k shared memory variable: serfCount");
		return -1;
	}
	shm_serfCount[0] = 0;
	munmap(shm_serfCount, shmSIZE);
	return 0;
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

int hackerCount_init()
{
	int shmID_hackersCount;
	shmID_hackersCount = shm_open(shmKEYhacker, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	ftruncate(shmID_hackersCount, shmSIZE);
	shm_hackerCount = (int *)mmap(NULL, shmSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmID_hackersCount
		, 0);
	close(shmID_hackersCount);
	if(shm_hackerCount == (void *)-1){
		fprintf(stderr, "Chyba pri pristupu k shared memory variable: hackerCount");
		return -1;
	}
	shm_hackerCount[0] = 0;
	munmap(shm_hackerCount, shmSIZE);
	return 0;
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

int numberOfProcesses_init()
{
	int shmID_numberOfProcesses;
	shmID_numberOfProcesses = shm_open(shmKEYprocesses, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	ftruncate(shmID_numberOfProcesses, shmSIZE);
	shm_numberOfProcesses = (int *)mmap(NULL, shmSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, 
		shmID_numberOfProcesses, 0);
	close(shmID_numberOfProcesses);
	if(shm_numberOfProcesses == (void *)-1){
		fprintf(stderr, "Chyba pri pristupu k shared memory variable: numberOfProcesses");
		return -1;
	}
	shm_numberOfProcesses[0] = 0;
	munmap(shm_numberOfProcesses, shmSIZE);
	return 0;
}

void numberOfProcesses_open()
{
	int shmID_numberOfProcesses;
    shmID_numberOfProcesses = shm_open(shmKEYprocesses, O_RDWR, S_IRUSR | S_IWUSR);
    shm_numberOfProcesses = (int*)mmap(NULL, shmSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmID_numberOfProcesses, 0);
    close(shmID_numberOfProcesses);
}

void numberOfProcesses_close()
{
    munmap(shm_numberOfProcesses, shmSIZE);
}

void numberOfProcesses_unlink()
{
    shm_unlink(shmKEYprocesses);
}

int hackerBoat_init()
{
	int shmID_hackerBoat;
	shmID_hackerBoat = shm_open(shmKEYhackerBoat, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	ftruncate(shmID_hackerBoat, shmSIZE);
	shm_hackerBoat = (int *)mmap(NULL, shmSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmID_hackerBoat
		, 0);
	close(shmID_hackerBoat);
	if(shm_hackerBoat == (void *)-1){
		fprintf(stderr, "Chyba pri pristupu k shared memory variable: actionNum");
		return -1;
	}
	shm_hackerBoat[0] = 0;
	munmap(shm_hackerBoat, shmSIZE);
	return 0;
}

void hackerBoat_open()
{
	int shmID_hackerBoat;
    shmID_hackerBoat = shm_open(shmKEYhackerBoat, O_RDWR, S_IRUSR | S_IWUSR);
    shm_hackerBoat = (int*)mmap(NULL, shmSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmID_hackerBoat, 0);
    close(shmID_hackerBoat);
}

void hackerBoat_close()
{
    munmap(shm_hackerBoat, shmSIZE);
}

void hackerBoat_unlink()
{
    shm_unlink(shmKEYhackerBoat);
}

int serfBoat_init()
{
	int shmID_serfBoat;
	shmID_serfBoat = shm_open(shmKEYserfBoat, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	ftruncate(shmID_serfBoat, shmSIZE);
	shm_serfBoat = (int *)mmap(NULL, shmSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmID_serfBoat
		, 0);
	close(shmID_serfBoat);
	if(shm_serfBoat == (void *)-1){
		fprintf(stderr, "Chyba pri pristupu k shared memory variable: actionNum");
		return -1;
	}
	shm_serfBoat[0] = 0;
	munmap(shm_serfBoat, shmSIZE);
	return 0;
}

void serfBoat_open()
{
	int shmID_serfBoat;
    shmID_serfBoat = shm_open(shmKEYserfBoat, O_RDWR, S_IRUSR | S_IWUSR);
    shm_serfBoat = (int*)mmap(NULL, shmSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmID_serfBoat, 0);
    close(shmID_serfBoat);
}

void serfBoat_close()
{
    munmap(shm_serfBoat, shmSIZE);
}

void serfBoat_unlink()
{
    shm_unlink(shmKEYserfBoat);
}


int init()
{
	if ((boatMembers = sem_open(BOATMEMBERS_SEM, O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED){
		return -1;
	}

	if ((captain = sem_open(CAPTAIN_SEM, O_CREAT | O_EXCL , 0666, 0)) == SEM_FAILED){
		return -1;
	}

	if((memAccess = sem_open(MEMACCESS_SEM, O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED){
		return -1;
	}

	if ((molo = sem_open(MOLO_SEM, O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED){
		return -1;
	}

	if ((emptyBoat = sem_open(EMPTYBOAT_SEM, O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED){
		return -1;
	}

	if ((disembark = sem_open(DISEMBARK_SEM, O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED){
		return -1;
	}

	if(serfCount_init() == -1){
		return -1;
	}

	if(hackerCount_init() == -1){
		return -1;
	}

	if(actionNumber_init() == -1){
		return -1;
	}

	if(numberOfProcesses_init() == -1){
		return -1;
	}

	if(hackerBoat_init() == -1){
		return -1;
	}

	if(serfBoat_init() == -1){
		return -1;
	}

	return 0;
}


void ClearData()
{
	sem_close(boatMembers);
	sem_unlink(BOATMEMBERS_SEM);
	sem_close(captain);
	sem_unlink(CAPTAIN_SEM);
	sem_close(memAccess);
	sem_unlink(MEMACCESS_SEM);
	actionNumber_unlink();
	serfCount_unlink();
	hackerCount_unlink();
	numberOfProcesses_unlink();
}

void shmAccessOpen()
{
	sem_wait(memAccess);
	serfCount_open();
	hackerCount_open();
	actionNumber_open();
	numberOfProcesses_open();
	hackerBoat_open();
	serfBoat_open();
}

void shmAccessClose()
{
	serfCount_close();
	hackerCount_close();
	actionNumber_close();
	numberOfProcesses_close();
	numberOfProcesses_close();
	hackerBoat_close();
	serfBoat_close();
	sem_post(memAccess);
}

