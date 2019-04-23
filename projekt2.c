
#include "shm_lib.c"




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
		shm_hackerCount++;
		printf("HACKERS: %d\n",*shm_hackerCount );
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
			shm_serfCount += 1;
			printf("SERFS: %d\n",*shm_serfCount );
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
	if (SemInit() == -1){
		ClearSem();
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

	ClearSem();
	printf("%s\n","done, Bossman" );
	return 0;
}