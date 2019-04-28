
#include "shm_lib.h"

void Hacker(int capacity, int waitTime, int rowTime, FILE *outFile)
{
	char name[] = "HACK";
	shmAccessOpen();
	// Assigns process number
	int processNum = shm_numberOfProcesses[0]++;
	fprintf(outFile, "%d: %s %d: starts \n", ++shm_actionNumber[0], name, processNum);
	shmAccessClose();
	//initialization of variables
	char cant_enter_molo = 0;
	char cant_enter_ship = 0;
	do
	{
		shmAccessOpen();
		// Checks the option to get on the molo
		if(shm_serfCount[0] + shm_hackerCount[0] < capacity){
			cant_enter_molo = 0;
			sem_wait(molo);
			// Gets on molo
			shm_hackerCount[0]++;
			fprintf(outFile, "%d: %s %d: waits: %d: %d\n", ++shm_actionNumber[0], name, processNum,
				shm_hackerCount[0], shm_serfCount[0]);
			shmAccessClose();
		}
		// if it can't, it waits and then tries again.
		else{
			fprintf(outFile, "%d: %s %d: leaves queue: %d: %d\n", ++shm_actionNumber[0], name,
				processNum, shm_hackerCount[0], shm_serfCount[0]);
			cant_enter_molo = 1;
			shmAccessClose();
			mysleep(waitTime);
			shmAccessOpen();
			fprintf(outFile, "%d: %s %d: is back\n", ++shm_actionNumber[0], name, processNum);
			shmAccessClose();
		}	
	} while (cant_enter_molo);

	do{
		sem_wait(boardBoat);
		shmAccessOpen();
		// Checks the option of loading into the boat.
		// Conditions:
		// - Less than 4 people on the boat
		// - Less than 3 serfs on board
		// - Current boat members aren't SHH.
		// - More than 1 hacker on molo
		// - If 4 hacks want to board, there need to be 4 hacks on the molo
		if(shm_hackerBoat[0] + shm_serfBoat[0] < 4 && shm_serfBoat[0] < 3 && 
			!(shm_serfBoat[0] == 1 && shm_hackerBoat[0] == 2) && shm_hackerCount[0] >= 2 && 
			!((shm_hackerBoat[0] == 2 && shm_serfBoat == 0) && shm_hackerCount[0] < 4)){	
			cant_enter_ship = 0;
			shm_hackerBoat[0]++;
			//if there are exactly 4 people on board - CAPTAIN BRANCH:
			if(shm_hackerBoat[0] + shm_serfBoat[0] == 4){
				sem_wait(captain);
				// Takes away the appropriate number of Serfs and Hacks
				for(int i = 0; i < shm_hackerBoat[0]; i++){
					shm_hackerCount[0]--;
					sem_post(molo);
				}
				for (int i = 0; i < shm_serfBoat[0]; i++){
					shm_serfCount[0]--;
					sem_post(molo);
				}
				// Starts the boat
				fprintf(outFile, "%d: %s %d: boards: %d: %d\n", ++shm_actionNumber[0], name,
					processNum, shm_hackerCount[0], shm_serfCount[0]);
				shmAccessClose();

				mysleep(rowTime);
				// Signals the members to disembark
				for (int i = 0; i < 3; i++) sem_post(disembark);
				// Waits for all 3 members to disembark
				for (int i = 0; i < 3; i++) sem_wait(boatMembers);

				shmAccessOpen();
				// If there is only the captain on board, ends the ride and exits.
				if(shm_hackerBoat[0] == 1 && shm_serfBoat[0] == 0){
					shm_hackerBoat[0] = 0;
					fprintf(outFile, "%d: %s %d: captain exits: %d: %d\n", ++shm_actionNumber[0],
						name, processNum, shm_hackerCount[0], shm_serfCount[0]);
					shmAccessClose();
					sem_post(captain);
					for(int i = 0; i < 4; i++) sem_post(boardBoat);
					exit(EXIT_SUCCESS);
				}
				// If there are leftover members on board ends with an error message
				else{
					fprintf(stderr, "%d: %s %d: Did not succesfully disembark\n",
						++shm_actionNumber[0], name, processNum);
					fprintf(stderr,"Leftover on boat - H: %d, S: %d\n",	shm_hackerBoat[0],
						shm_serfBoat[0]);
					shmAccessClose();
					ClearData();
					exit(EXIT_FAILURE);
				}
			}
			// SHIP MEMBER BRANCH: 
			else{
				shmAccessClose();
				// Waits until it recieves a disembark command
				sem_wait(disembark);
				shmAccessOpen();
				// Gets off
				shm_hackerBoat[0]--;
				fprintf(outFile, "%d: %s %d: member exits: %d: %d\n", ++shm_actionNumber[0],
					name, processNum, shm_hackerCount[0], shm_serfCount[0]);
				shmAccessClose();
				// signals the captain about successful disembarking
				sem_post(boatMembers);
				exit(EXIT_SUCCESS);
			}
		}
		// If the process couldn't get on the boat, it tries again.
		else{
			shmAccessClose();
			cant_enter_ship = 1;
			sem_post(boardBoat);
		}
	}while(cant_enter_ship);
	//If for some reason it gets out, the process is ended with an error message
	shmAccessOpen();
	fprintf(stderr, "%d: %s %d: Ended unsuccefully\n",++shm_actionNumber[0], name, processNum);
	shmAccessClose();
	exit(EXIT_FAILURE);
}


void Serf(int capacity, int waitTime, int rowTime, FILE *outFile)
{
	char name[] = "SERF";
	shmAccessOpen();
	int processNum = shm_numberOfProcesses[0]++;
	fprintf(outFile, "%d: %s %d: starts \n", ++shm_actionNumber[0], name, processNum);
	shmAccessClose();
	char cant_enter_molo = 0;
	char cant_enter_ship = 0;

	do
	{
		shmAccessOpen();
		if(shm_serfCount[0] + shm_hackerCount[0] < capacity){
			cant_enter_molo = 0;
			sem_wait(molo);
			shm_serfCount[0]++;
			fprintf(outFile, "%d: %s %d: waits: %d: %d\n", ++shm_actionNumber[0], name, processNum,
				shm_hackerCount[0], shm_serfCount[0]);
			shmAccessClose();
		}
		else{
			fprintf(outFile, "%d: %s %d: leaves queue: %d: %d\n", ++shm_actionNumber[0], name,
				processNum, shm_hackerCount[0], shm_serfCount[0]);
			cant_enter_molo = 1;
			shmAccessClose();
			mysleep(waitTime);
			shmAccessOpen();
			fprintf(outFile, "%d: %s %d: is back\n", ++shm_actionNumber[0], name, processNum);
			shmAccessClose();
		}	
	} while (cant_enter_molo);

	do{
		sem_wait(boardBoat);
		shmAccessOpen();
		// Checks the option of loading into the boat.
		// Conditions:
		// - Less than 4 people on the boat
		// - Less than 3 serfs on board
		// - Current boat members aren't SHH.
		// - More than 1 hacker on molo
		// - If 4 hacks want to board, there need to be 4 hacks on the molo
		if(shm_hackerBoat[0] + shm_serfBoat[0] < 4 && shm_hackerBoat[0] <= 2 && 
			!(shm_hackerBoat[0] == 1 && shm_serfBoat[0] == 2) && shm_serfCount[0] >= 2 &&
			!((shm_serfBoat[0] == 2 && shm_hackerBoat[0] == 0) && shm_serfCount[0] < 4)){
			cant_enter_ship = 0;
			shm_serfBoat[0]++;
			//if there are exactly 4 people on board - CAPTAIN BRANCH:
			if(shm_hackerBoat[0] + shm_serfBoat[0] == 4){
				
				//assigning captain
				sem_wait(captain);

				// Takes away the appropriate number of Serfs and Hacks
				for(int i = 0; i < shm_serfBoat[0]; i++){
					shm_serfCount[0]--;
					sem_post(molo);
				}
				for(int i = 0; i < shm_hackerBoat[0]; i++){
					shm_hackerCount[0]--;
					sem_post(molo);
				}

				// Starts the boat
				fprintf(outFile, "%d: %s %d: boards: %d: %d\n", ++shm_actionNumber[0], name,
						processNum, shm_hackerCount[0], shm_serfCount[0]);
				shmAccessClose();
				mysleep(rowTime);

				// Signals the members to disembark
				for (int i = 0; i < 3; i++) sem_post(disembark);
				// Waits for all 3 members to disembark
				for (int i = 0; i < 3; i++) sem_wait(boatMembers);
				shmAccessOpen();
				// If there is only the captain on board, ends the ride and exits.
				if(shm_serfBoat[0] == 1 && shm_hackerBoat[0] == 0){
					shm_serfBoat[0] = 0;
					fprintf(outFile, "%d: %s %d: captain exits: %d: %d\n", ++shm_actionNumber[0],
							name, processNum, shm_hackerCount[0], shm_serfCount[0]);
					shmAccessClose();
					sem_post(captain);
					for(int i = 0; i < 4; i++) sem_post(boardBoat);
					exit(EXIT_SUCCESS);
				}
				// If there are leftover members on board ends with an error message
				else{
					fprintf(stderr, "%d: %s %d: Did not succesfully disembark\n",
							++shm_actionNumber[0], name, processNum);
						fprintf(stderr,"Leftover on boat - H: %d, S: %d\n",	shm_hackerBoat[0],
							shm_serfBoat[0]);
						shmAccessClose();
						ClearData();
						exit(EXIT_FAILURE);
				}
			}

			//SHIP MEMBER BRANCH
			else{
				shmAccessClose();
				// Waits for captains command to disembard
				sem_wait(disembark);
				shmAccessOpen();
				// disembarks with message
				shm_serfBoat[0]--;
				fprintf(outFile, "%d: %s %d: member exits: %d: %d\n", ++shm_actionNumber[0],
						name, processNum, shm_hackerCount[0], shm_serfCount[0]);
				shmAccessClose();
				// signals the captain about successful disembarking
				sem_post(boatMembers);
				exit(EXIT_SUCCESS);
			}
		}
		// If the process cant get on board, it tries again
		else{
			shmAccessClose();
			cant_enter_ship = 1;
			sem_post(boardBoat);
		}
	}while(cant_enter_ship);
	//If for some reason, it gets out, the process is ended with an error message
	shmAccessOpen();
	fprintf(stderr, "%d: %s %d: Ended unsuccefully\n",++shm_actionNumber[0], name, processNum);
	shmAccessClose();
	exit(EXIT_FAILURE);
}


void HackerGenerator(int personCount, int hackerTime, int waitTime,int capacity, int rowTime, 
	FILE *outFile)
{
	for(int i = 0; i < personCount; i++){
		pid_t hackerID = fork();
		if (hackerID == 0){
			Hacker(capacity, waitTime, rowTime, outFile);
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


void SerfGenerator(int personCount, int serfTime, int waitTime, int capacity, int rowTime, 
	FILE *outFile)
{
	for(int i = 0; i < personCount; i++){
		pid_t serfID = fork();
		if (serfID == 0){
			Serf(capacity, waitTime, rowTime,outFile);
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
	// Opening output file
	FILE *outFile = fopen("proj2.out", "w");
	setbuf(outFile, NULL);
	if (outFile == NULL){
		fprintf(stderr, "Output file didn't open correctly.");
		return 1;
	}
	// initializing and declaring variables
	long personCount = 0;
	long hackerTime = 0;
	long serfTime = 0;
	long rowTime = 0;
	long waitTime = 0;
	long capacity = 0;
	char *ptr;

	// PARSING ARGUMENTS 
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
	}

	//INITIALIZING SEMAPHORES AND SHARED MEMORY VARIABLES
	if (init() == -1){
		ClearData();
		return -1;
	}

	//Allowing one process to access memory and 4 processes to board the ship
	sem_post(memAccess);
	sem_post(captain);
	for(int i = 0; i < capacity; i++){
		sem_post(molo);
	}
	for(int i = 0; i < 4; i++) sem_post(boardBoat);

	// Hacker generator
	pid_t hackerGeneratorID = fork();
	if(hackerGeneratorID == 0){
		HackerGenerator(personCount,hackerTime,waitTime,capacity,rowTime, outFile);
		exit(EXIT_SUCCESS);
	}

	else if (hackerGeneratorID > 0){
		// Serf generator
		pid_t SerfGeneratorID = fork();
		if (SerfGeneratorID == 0){
			SerfGenerator(personCount,serfTime,waitTime,capacity,rowTime, outFile);
		}
		if (SerfGeneratorID < 0){
			fprintf(stderr, "Failed to start the hacker generator process.\n");
			return 1;
		}
	}
	else {
		fprintf(stderr, "Failed to start the serf generator process.\n");
	}
	wait(NULL);
	wait(NULL);
	fclose(outFile);
	ClearData();
	return 0;
}
