
#include "shm_lib.h"




void Hacker(int capacity, int waitTime, int rowTime)
{
	shmAccessOpen();
	int processNum = shm_numberOfProcesses[0]++;
	shmAccessClose();
	//printf("HACKER[%d]: LOADED\n", processNum );
	char is_captain = 0;
	char cant_enter_molo = 0;
	char cant_enter_ship = 0;
	do
	{
		shmAccessOpen();
		printf("Hacker Count on molo: %d; Serf Count: %d\n",shm_hackerCount[0],shm_serfCount[0] );
		if(shm_serfCount[0] + shm_hackerCount[0] < capacity){
			printf("Hacker[%d]: Got on Molo\n",processNum);
			cant_enter_molo = 0;
			sem_wait(molo);
			shm_hackerCount[0]++;
			shmAccessClose();
		}
		else{
			printf("Hacker[%d]:waiting\n",processNum);
			cant_enter_molo = 1;
			shmAccessClose();
			mysleep(waitTime);
		}	
	} while (cant_enter_molo);

	do{
		sem_wait(boardBoat);
		shmAccessOpen();
		if(shm_hackerBoat[0] + shm_serfBoat[0] < 4 && shm_serfBoat[0] < 3 && 
			!(shm_serfBoat[0] == 1 && shm_hackerBoat[0] == 2) && shm_hackerCount[0] >= 2 && 
			!((shm_hackerBoat[0] == 2 && shm_serfBoat == 0) && shm_hackerCount[0] < 4)){	
				cant_enter_ship = 0;
				printf("HACKER[%d]: BOARDING SHIP\n",processNum );
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
					printf("HACKER[%d]: Sailing away\n",processNum );

					mysleep(rowTime);

					sem_post(disembark);
					sem_post(disembark);
					sem_post(disembark);
					printf("waiting for end\n");
					sem_wait(boatMembers);
					sem_wait(boatMembers);
					sem_wait(boatMembers);
					shmAccessOpen();
					if(shm_hackerBoat[0] == 1 && shm_serfBoat[0] == 0){
						shm_hackerBoat[0] = 0;
						shmAccessClose();
						printf("HACKER[%d]: Ending ride\n",processNum );
						sem_post(captain);
						for(int i = 0; i < 4; i++) sem_post(boardBoat);
						exit(EXIT_SUCCESS);
					}
					shmAccessClose();
					pid_t errorID = fork();
					if(errorID > 0){
						fprintf(stderr, "Did not succesfully disembark\n");
						shmAccessOpen();
						printf("HACKER[%d]: Leftover members - H: %d, S: %d\n",processNum, shm_hackerCount[0], shm_serfCount[0] );
						shmAccessClose();
						ClearData();
						exit(EXIT_FAILURE);
					}
				}

				else{
					shmAccessClose();
					sem_wait(disembark);
					printf("HACKER[%d]: DISSEMBARKING\n",processNum );
					shmAccessOpen();
					shm_hackerBoat[0]--;
					shmAccessClose();
					sem_post(boatMembers);
					exit(EXIT_SUCCESS);
				}
			}
			else{
				shmAccessClose();
				cant_enter_ship = 1;
				sem_post(boardBoat);
			}
	}while(cant_enter_ship);
	fprintf(stderr, "HACKER[%d]:I failed Bossman\n",processNum);
	exit(EXIT_FAILURE);
}







void Serf(int capacity, int waitTime, int rowTime)
{

	shmAccessOpen();
	int processNum = shm_numberOfProcesses[0]++;
	shmAccessClose();
	//printf("SERF[%d]: LOADED\n", processNum );
	char is_captain = 0;
	char cant_enter_molo = 0;
	char cant_enter_ship = 0;
	//ENTERING MOLO
	do
	{
		shmAccessOpen();
		printf("Hacker Count on molo: %d; Serf Count: %d\n",shm_hackerCount[0],shm_serfCount[0] );
		if(shm_serfCount[0] + shm_hackerCount[0] < capacity){
			printf("SERF[%d] Got on Molo\n",processNum);
			cant_enter_molo = 0;
			sem_wait(molo);
			shm_serfCount[0]++;
			shmAccessClose();
		}
		else{
			printf("SERF[%d]:waiting\n",processNum);
			cant_enter_molo = 1;
			shmAccessClose();
			mysleep(waitTime);
		}	
	} while (cant_enter_molo);
	//CAN BOARD BOAT
	do{
		sem_wait(boardBoat);
		shmAccessOpen();
		if(shm_hackerBoat[0] + shm_serfBoat[0] < 4 && shm_hackerBoat[0] <= 2 && 
			!(shm_hackerBoat[0] == 1 && shm_serfBoat[0] == 2) && shm_serfCount[0] >= 2 &&
			!((shm_serfBoat[0] == 2 && shm_hackerBoat[0] == 0) && shm_serfCount[0] < 4)){
			cant_enter_ship = 0;
			printf("SERF[%d]: BOARDING SHIP\n", processNum);
			shm_serfBoat[0]++;
			//CPATAIN BRANCH
			if(shm_hackerBoat[0] + shm_serfBoat[0] == 4){
				
				//assigning captain
				sem_wait(captain);
				is_captain = 1;


				//Taking boat members from molo
				for(int i = 0; i < shm_serfBoat[0]; i++){
					shm_serfCount[0]--;
					sem_post(molo);
				}
				for(int i = 0; i < shm_hackerBoat[0]; i++){
					shm_hackerCount[0]--;
					sem_post(molo);
				}

				shmAccessClose();

				printf("SERF[%d]: Sailing away\n",processNum );
				mysleep(rowTime);

				printf("waiting for end\n");
				for (int i = 0; i < 3; ++i) sem_post(disembark);
				for (int i = 0; i < 3; ++i) sem_wait(boatMembers);

				shmAccessOpen();
				if(shm_serfBoat[0] == 1 && shm_hackerBoat[0] == 0){
					shm_serfBoat[0] = 0;
					shmAccessClose();
					printf("SERF[%d]: Ending ride\n",processNum );
					sem_post(captain);
					for(int i = 0; i < 4; i++) sem_post(boardBoat);
					exit(EXIT_SUCCESS);
				}
				shmAccessClose();
				pid_t errorID = fork();
				if(errorID > 0){
					fprintf(stderr, "Did not succesfully disembark\n");
						shmAccessOpen();
						printf("SERF[%d]: Leftover members - H: %d, S: %d\n",processNum, shm_hackerCount[0], shm_serfCount[0] );
						shmAccessClose();

					exit(EXIT_FAILURE);
				}
			}

			//SHIP MEMBER BRANCH
			else{
				shmAccessClose();
				sem_wait(disembark);
				printf("SERF[%d]: DISSEMBARKING\n",processNum );
				shmAccessOpen();
				shm_serfBoat[0]--;
				shmAccessClose();
				sem_post(boatMembers);
				exit(EXIT_SUCCESS);
			}
		}
		else{
			shmAccessClose();
			cant_enter_ship = 1;
			sem_post(boardBoat);
		}
	}while(cant_enter_ship);

	fprintf(stderr, "SERF[%d]: I failed Bossman\n",processNum);
	exit(EXIT_FAILURE);
}






void HackerGenerator(int personCount, int hackerTime, int waitTime,int capacity, int rowTime)
{
	for(int i = 0; i < personCount; i++){
		pid_t hackerID = fork();
		if (hackerID == 0){
			Hacker(capacity, waitTime, rowTime);
		}
		if (hackerID < 0){
			fprintf(stderr, "Couldn't generate Hackers.\n");
			exit(EXIT_FAILURE);
		}
		mysleep(hackerTime);
	}
	for(int i = 0; i < personCount; i++) wait(NULL);
	exit(EXIT_SUCCESS);
}






void SerfGenerator(int personCount, int serfTime, int waitTime, int capacity, int rowTime)
{
	for(int i = 0; i < personCount; i++){
		pid_t serfID = fork();
		if (serfID == 0){
			Serf(capacity, waitTime, rowTime);
		}
		if (serfID < 0){
			fprintf(stderr, "Couldn't generate Serfs\n");
			ClearData();
			exit(EXIT_FAILURE);
		}
		mysleep(serfTime);
	}
	for(int i = 0; i < personCount; i++) wait(NULL);
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
		printf("initialization went awry\n");
		ClearData();
		return -1;
	}
	//Allowing one process to access memory
	sem_post(memAccess);
	sem_post(captain);
	for(int i = 0; i < capacity; i++){
		sem_post(molo);
	}
	for(int i = 0; i < 4; i++) sem_post(boardBoat);

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
