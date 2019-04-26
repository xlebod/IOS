
#include "shm_lib.h"




void Hacker(int capacity, int waitTime, int rowTime)
{
	shmAccessOpen();
	int processNum = shm_numberOfProcesses[0]++;
	shmAccessClose();
	char is_captain = 0;
	char cant_enter_molo = 0;
	do
	{
		shmAccessOpen();
		if(shm_serfCount[0] + shm_hackerCount[0] < capacity){
			cant_enter_molo = 0;
			sem_wait(molo);
			shm_hackerCount[0]++;
			shmAccessClose();
		}
		else{
			cant_enter_molo = 1;
			shmAccessClose();
			mysleep(waitTime);
		}	
	} while (cant_enter_molo);
	shmAccessOpen();
	if(shm_hackerBoat[0] + shm_serfBoat[0] < 4 && shm_serfBoat[0] < 3){
		shm_hackerBoat[0]++;
		//CAPTAIN BRANCH
		if(shm_hackerBoat[0] + shm_serfBoat[0] == 4){
			sem_wait(captain);
			is_captain = 1;
			for(int i = 0; i < shm_hackerBoat[0]; i++){
				shm_hackerCount[0]--;
				sem_post(molo);
			}
			for (int i = 0; i < shm_serfBoat[0]; i++){
				shm_serfCount[0]--;
				sem_post(molo);
			}
			shmAccessClose();

			mysleep(rowTime);

			sem_post(disembark);
			sem_post(disembark);
			sem_post(disembark);

			sem_wait(boatMembers);
			sem_wait(boatMembers);
			sem_wait(boatMembers);
			shmAccessOpen();
			if(shm_hackerBoat[0] == 1 && shm_serfBoat[0] == 0){
				shm_hackerBoat[0] = 0;
				shmAccessClose();
				exit(EXIT_SUCCESS);
			}
			shmAccessClose();
			pid_t errorID = fork();
			if(errorID != 0){
				fprintf(stderr, "Did not succesfully disembark\n");
				exit(EXIT_FAILURE);
			}
		}
		else if(shm_hackerBoat[0] + shm_serfBoat[0] > 4){
			shmAccessClose();
			fprintf(stderr, "More than 4 members loaded into boat.");
			pid_t exitID = fork();
			//COMMIT SUDOKU
			if( exitID > 0)	exit(EXIT_FAILURE);
		}

		else{
			shmAccessClose();
			sem_wait(disembark);
			shmAccessOpen();
			shm_hackerBoat[0]--;
			shmAccessClose();
			sem_post(boatMembers);
			exit(EXIT_SUCCESS);
		}
	}
	else{shmAccessClose();}
	fprintf(stderr, "I failed Bossman -xoxo HACKER\n");
	exit(EXIT_FAILURE);
}

void Serf(int capacity, int waitTime, int rowTime)
{
	shmAccessOpen();
	int processNum = shm_numberOfProcesses[0]++;
	shmAccessClose();
	char is_captain = 0;
	char cant_enter_molo = 0;
	//ENTERING MOLO
	do
	{
		shmAccessOpen();
		if(shm_serfCount[0] + shm_hackerCount[0] < capacity){
			cant_enter_molo = 0;
			sem_wait(molo);
			shm_serfCount[0]++;
			shmAccessClose();
		}
		else{
			cant_enter_molo = 1;
			shmAccessClose();
			mysleep(waitTime);
		}	
	} while (cant_enter_molo);
	shmAccessOpen();
	//CAN BOARD BOAT
	if(shm_hackerBoat[0] + shm_serfBoat[0] < 4 && shm_hackerBoat[0] < 3){
		shm_serfBoat[0]++;
		//CPATAIN BRANCH
		if(shm_hackerBoat[0] + shm_serfBoat[0] == 4){
			sem_wait(captain);
			is_captain = 1;
			for(int i = 0; i < shm_serfBoat[0]; i++){
				sem_post(molo);
				shm_serfCount[0]--;
			}
			for(int i = 0; i < shm_hackerBoat[0]; i++){
				shm_hackerCount[0]--;
				sem_post(molo);
			}
			shmAccessClose();
			mysleep(rowTime);
			sem_post(disembark);
			sem_post(disembark);
			sem_post(disembark);

			sem_wait(boatMembers);
			sem_wait(boatMembers);
			sem_wait(boatMembers);

			shmAccessOpen();
			if(shm_hackerBoat[0] == 1 && shm_serfBoat[0] == 0){
				shm_hackerBoat[0] = 0;
				shmAccessClose();
				exit(EXIT_SUCCESS);
			}
			shmAccessClose();
			pid_t errorID = fork();
			if(errorID != 0){
				fprintf(stderr, "Did not succesfully disembark\n");
				exit(EXIT_FAILURE);
			}
		}
		//SHIP MEMBER BRANCH
		else if(shm_hackerBoat[0] + shm_serfBoat[0] > 4){
			shmAccessClose();
			fprintf(stderr, "More than 4 members loaded into boat.");
			pid_t exitID = fork();
			if( exitID > 0)	exit(EXIT_FAILURE);
		}
		else{
			shmAccessClose();
			sem_wait(disembark);
			shmAccessOpen();
			shm_serfBoat[0]--;
			shmAccessClose();
			sem_post(boatMembers);
			exit(EXIT_SUCCESS);
		}
	}
	else{shmAccessClose();}
	fprintf(stderr, "I failed Bossman -xoxo SERF\n");
	exit(EXIT_FAILURE);
}

void HackerGenerator(int personCount, int hackerTime, int waitTime,int capacity, int rowTime)
{
	for(int i = 0; i < personCount; i++){
		pid_t hackerID = fork();
		if (hackerID == 0){
			shmAccessOpen();
			shm_hackerCount[0]++;
			printf("HACKERS: %d\n",*shm_hackerCount );
			shmAccessClose();
			Hacker(capacity, waitTime, rowTime);
		}
		if (hackerID < 0){
			fprintf(stderr, "Couldn't generate Hackers.\n");
			exit(EXIT_FAILURE);
		}

		mysleep(hackerTime);
		for(int i = 0; i < personCount; i++) wait(NULL);
	}
		exit(EXIT_SUCCESS);
}

void SerfGenerator(int personCount, int serfTime, int waitTime, int capacity, int rowTime)
{
	for(int i = 0; i < personCount; i++){
		pid_t serfID = fork();
		if (serfID == 0){
			shmAccessOpen();
			shm_serfCount[0] += 1;
			printf("SERFS: %d\n",*shm_serfCount );
			shmAccessClose();
			Serf(capacity, waitTime, rowTime);
		}
		if (serfID < 0){
			fprintf(stderr, "Couldn't generate Serfs\n");
			exit(EXIT_FAILURE);
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
		return 1;
	}
	long personCount = 0;
	long hackerTime = 0;
	long serfTime = 0;
	long rowTime = 0;
	long waitTime = 0;
	long capacity = 0;
	char *ptr;
	// LOADING ARGUMENTS 
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
	//Allowing one process to access memory
	sem_post(memAccess);

	pid_t hackerGeneratorID = fork();
	if(hackerGeneratorID == 0){
		HackerGenerator(personCount,hackerTime,waitTime,capacity,rowTime);
		exit(EXIT_SUCCESS);
	}
	else if (hackerGeneratorID > 0){
		pid_t SerfGeneratorID = fork();
		if (SerfGeneratorID == 0){
			SerfGenerator(personCount,serfTime,waitTime,capacity,rowTime);
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