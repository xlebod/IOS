/**
 * @file resources.h
 * @brief Global resources interface.
 * @author Dominik Harmim <xharmi00@stud.fit.vutbr.cz>
 * @date 28.4.2017
 */


#ifndef PROJ2_RESOURCES_H
#define PROJ2_RESOURCES_H


#include <stdbool.h>
#include <stdio.h>
#include <semaphore.h>


#define MUTEX_NAME "/xharmi00-ios2-mutex" /// Semaphore mutex name.
#define CHILD_QUEUE_NAME "/xharmi00-ios2-childq" /// Semaphore child_queue name.
#define ADULT_QUEUE_NAME "/xharmi00-ios2-adultq" /// Samaphore adult_queue name.
#define FINISHED_BARRIER "/xharmi00-ios2-fbarr" /// Samaphore finished_barrier name.
#define OUTPUT_FILE_NAME "proj2.out" /// Output file name.


/**
 * @struct Structure of semaphores.
 */
struct semaphores {
	sem_t *mutex; /// For access to shared variables.
	sem_t *child_queue; /// For waiting children.
	sem_t *adult_queue; /// For waiting adults.
	sem_t *finished_barrier; /// For processes waiting to exit.
};


/// Global variable with semaphores.
struct semaphores semaphores;


/**
 * @struct Struct of shared variables.
 */
struct shared_vars {
	int *action_counter; /// Counter for actions.
	int action_counter_id; /// Counter for actions ID.

	int *child_counter; /// Counter for children.
	int child_counter_id; /// Counter for children ID.

	int *adult_counter; /// Counter for adults.
	int adult_counter_id; /// Counter for adults ID.

	int *child_waiting; /// Waiting children count.
	int child_waiting_id; /// Waiting children count ID.

	int *adult_waiting; /// Waiting adults count.
	int adult_waiting_id; /// Waiting adults count ID.

	int *current_child_count; /// Current count of children in center.
	int current_child_count_id; /// Current count of children in center ID.

	int *current_adult_count; /// Current count of adults in center.
	int current_adult_count_id; /// Current count of adults in center ID.

	int *working_counter; /// Still working processes count.
	int working_counter_id; /// Still working processes count ID.

	FILE *output_file; /// Open file handle.
	int output_file_id; /// Open file handle ID.
};


/// Global variable with shared_variables.
struct shared_vars shared_vars;


/**
 * Set global resources.
 *
 * @param processes_count Processes to generate count.
 * @return True if seccessful, false otherwise.
 */
bool set_resources(int processes_count);


/**
 * Clean global resources.
 */
void clean_resources();


#endif //PROJ2_RESOURCES_H
