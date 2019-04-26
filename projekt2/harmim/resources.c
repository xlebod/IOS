/**
 * @file resources.c
 * @brief Global resources implementation.
 * @author Dominik Harmim <xharmi00@stud.fit.vutbr.cz>
 * @date 28.4.2017
 */


#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "resources.h"


/**
 * Set semaphore.
 *
 * @param sem Pointer to semaphore.
 * @param name Semaphore name.
 * @param value Semaphore inicialize value.
 * @return True if successful, false otherwise.
 */
static bool set_semaphore(sem_t **sem, char *name, int value)
{
	if ((*sem = sem_open(name, O_CREAT | O_EXCL, DEFFILEMODE, value)) == SEM_FAILED) {
		return false;
	}

	return true;
}


/**
 * Set shared memory.
 *
 * @param id Pointer to ID of shared memory.
 * @param mem Pointer to shared memory.
 * @param size Size of shared memory.
 * @return True if successful, false otherwise.
 */
static bool set_shm(int *id, void **mem, size_t size)
{
	if ((*id = shmget(IPC_PRIVATE, size, IPC_CREAT | DEFFILEMODE)) == -1) {
		return false;
	}
	if ((*mem = shmat(*id, NULL, 0)) == NULL) {
		return false;
	}

	return true;
}


/**
 * Set integer shared memory.
 *
 * @param id Pointer to ID of shared memory.
 * @param mem Pointer to shared memory.
 * @param value Default value of shared memory.
 * @return True if successful, false otherwise.
 */
static bool set_int_shm(int *id, int **mem, int value)
{
	if ( ! set_shm(id, (void **) mem, sizeof (int))) {
		return false;
	}

	*mem = (int *) *mem;
	**mem = value;

	return true;
}


/**
 * Set FILE shared memory.
 *
 * @param id Pointer to ID of shared memory.
 * @param mem Pointer to shared memory.
 * @return True if successful, false otherwise.
 */
static bool set_file_shm(int *id, FILE **mem)
{
	if ( ! set_shm(id, (void **) mem, sizeof (FILE))) {
		return false;
	}

	*mem = (FILE *) *mem;

	return true;
}


bool set_resources(int processes_count)
{
	// semaphores
	if ( ! set_semaphore(&semaphores.mutex, MUTEX_NAME, 1)) {
		return false;
	}
	if ( ! set_semaphore(&semaphores.child_queue, CHILD_QUEUE_NAME, 0)) {
		return false;
	}
	if ( ! set_semaphore(&semaphores.adult_queue, ADULT_QUEUE_NAME, 0)) {
		return false;
	}
	if ( ! set_semaphore(&semaphores.finished_barrier, FINISHED_BARRIER, 0)) {
		return false;
	}

	// shared memory
	if ( ! set_int_shm(&shared_vars.action_counter_id, &shared_vars.action_counter, 0)) {
		return false;
	}
	if ( ! set_int_shm(&shared_vars.child_counter_id, &shared_vars.child_counter, 0)) {
		return false;
	}
	if ( ! set_int_shm(&shared_vars.child_waiting_id, &shared_vars.child_waiting, 0)) {
		return false;
	}
	if ( ! set_int_shm(&shared_vars.adult_counter_id, &shared_vars.adult_counter, 0)) {
		return false;
	}
	if ( ! set_int_shm(&shared_vars.adult_waiting_id, &shared_vars.adult_waiting, 0)) {
		return false;
	}
	if ( ! set_int_shm(&shared_vars.working_counter_id, &shared_vars.working_counter, processes_count)) {
		return false;
	}
	if ( ! set_int_shm(&shared_vars.current_child_count_id, &shared_vars.current_child_count, 0)) {
		return false;
	}
	if ( ! set_int_shm(&shared_vars.current_adult_count_id, &shared_vars.current_adult_count, 0)) {
		return false;
	}
	if ( ! set_file_shm(&shared_vars.output_file_id, &shared_vars.output_file)) {
		return false;
	}

	// open output file
	shared_vars.output_file = fopen(OUTPUT_FILE_NAME, "w");
	if ( ! shared_vars.output_file) {
		return false;
	}
	setbuf(shared_vars.output_file, NULL); // disable output buffering

	return true;
}


/**
 * Clean semaphores.
 *
 * @param sem Semaphore structrue.
 * @param name Semaphore name.
 */
static void clean_semaphore(sem_t *sem, char *name)
{
	sem_close(sem);
	sem_unlink(name);
}


/**
 * Clean shared memory.
 *
 * @param id Shared memory ID.
 */
static void clean_shm(int id)
{
	shmctl(id, IPC_RMID, NULL);
}


void clean_resources()
{
	// semaphores
	clean_semaphore(semaphores.mutex, MUTEX_NAME);
	clean_semaphore(semaphores.child_queue, CHILD_QUEUE_NAME);
	clean_semaphore(semaphores.adult_queue, ADULT_QUEUE_NAME);
	clean_semaphore(semaphores.finished_barrier, FINISHED_BARRIER);

	// shared memory
	clean_shm(shared_vars.action_counter_id);
	clean_shm(shared_vars.child_counter_id);
	clean_shm(shared_vars.child_waiting_id);
	clean_shm(shared_vars.adult_counter_id);
	clean_shm(shared_vars.adult_waiting_id);
	clean_shm(shared_vars.working_counter_id);
	clean_shm(shared_vars.current_child_count_id);
	clean_shm(shared_vars.current_adult_count_id);
	clean_shm(shared_vars.output_file_id);

	// close output file
	fclose(shared_vars.output_file);
}
