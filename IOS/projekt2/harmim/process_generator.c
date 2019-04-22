/**
 * @file process_generator.c
 * @brief Processes generator implementation.
 * @author Dominik Harmim <xharmi00@stud.fit.vutbr.cz>
 * @date 28.4.2017
 */


#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "process_generator.h"
#include "resources.h"


/**
 * Sleep for random time.
 *
 * @param max Max time in milliseconds.
 */
static void wait_random_time(int max)
{
	if (max > 0) {
		usleep((useconds_t) (random() % max * 1000));
	}
}


/**
 * Exit generated process.
 *
 * @param process_type Process type (A or C).
 * @param counter Number of finishing process.
 */
static void exit_process(char process_type, int counter)
{
	// wait for all generated processes exit using barrier, which is set to total processes count from begining
	/****************** Barrier ******************/
	sem_wait(semaphores.mutex); /// lock mutex
	{
		(*shared_vars.working_counter)--; // process ended with work

		if (*shared_vars.working_counter == 0) {
			// if woring processes is 0, then unlock barrier
			sem_post(semaphores.finished_barrier); /// unlock barrier
		}
	}
	sem_post(semaphores.mutex); /// unlock mutex

	sem_wait(semaphores.finished_barrier); /// lock barrier
	sem_post(semaphores.finished_barrier); /// unlock barrier


	/****************** Finishing ******************/
	sem_wait(semaphores.mutex); /// lock mutex
	{
		// print: finished
		fprintf(shared_vars.output_file, PRINT_FORMAT, ++(*shared_vars.action_counter), process_type, counter,
				"finished");
	}
	sem_post(semaphores.mutex); /// unlock mutex


	/****************** Exit process ******************/
	exit(EXIT_SUCCESS);
}


void adult_action(int wait_time, int adult_count)
{
	(void) adult_count;
	int process_counter;

	/****************** Starting ******************/
	sem_wait(semaphores.mutex); /// lock mutex
	{
		process_counter = ++(*shared_vars.adult_counter);
		(*shared_vars.current_adult_count)++;

		// check if there are children waiting to enter
		if (*shared_vars.child_waiting) {
			// let enter waiting children, but max 3
			int n = *shared_vars.child_waiting > 3 ? 3 : *shared_vars.child_waiting;
			for (int i = 0; i < n; i++) {
				sem_post(semaphores.child_queue); /// unlock child_queue
				(*shared_vars.child_waiting)--;
				(*shared_vars.current_child_count)++;
			}
		}

		// print: started
		fprintf(shared_vars.output_file, PRINT_FORMAT, ++(*shared_vars.action_counter), 'A', process_counter, "started");
		// print: enter
		fprintf(shared_vars.output_file, PRINT_FORMAT, ++(*shared_vars.action_counter), 'A', process_counter, "enter");
	}
	sem_post(semaphores.mutex); /// unlock mutex


	/****************** Critical section ******************/
	wait_random_time(wait_time); // simulate activity


	/****************** Leaving ******************/
	sem_wait(semaphores.mutex); /// lock mutex
	{
		// print: trying to leave
		fprintf(shared_vars.output_file, PRINT_FORMAT, ++(*shared_vars.action_counter), 'A', process_counter,
				"trying to leave");

		// before leave check whether there are enough adults left
		if (*shared_vars.current_child_count <= 3 * ((*shared_vars.current_adult_count) - 1)) {
			// there are enough adults left

			// print: leave
			fprintf(shared_vars.output_file, PRINT_FORMAT, ++(*shared_vars.action_counter), 'A', process_counter, "leave");

			(*shared_vars.current_adult_count)--;

		} else {
			// there are not enough adults left

			(*shared_vars.adult_waiting)++;

			// print: waiting
			fprintf(shared_vars.output_file, PRINT_FORMAT_WAITING, ++(*shared_vars.action_counter), 'A', process_counter,
					"waiting", *shared_vars.current_adult_count, *shared_vars.current_child_count);

			sem_post(semaphores.mutex); /// unlock mutex
			sem_wait(semaphores.adult_queue); /// lock adult_queue

			sem_wait(semaphores.mutex); /// lock mutex

			// print: leave
			fprintf(shared_vars.output_file, PRINT_FORMAT, ++(*shared_vars.action_counter), 'A', process_counter, "leave");
		}

		// if there are no more adults, let waiting children enter
		if (adult_count == *shared_vars.adult_counter && ! *shared_vars.current_adult_count && *shared_vars.child_waiting) {
			int waiting_count = *shared_vars.child_waiting;
			for (int i = 0; i < waiting_count; i++) {
				sem_post(semaphores.child_queue); /// unlock child_queue
				(*shared_vars.child_waiting)--;
				(*shared_vars.current_child_count)++;
			}
		}
	}
	sem_post(semaphores.mutex); /// unlock mutex


	/****************** Exit process ******************/
	exit_process('A', process_counter);
}


void child_action(int wait_time, int adult_count)
{
	/****************** Starting ******************/
	sem_wait(semaphores.mutex); /// lock mutex

	int process_counter = ++(*shared_vars.child_counter);

	// print: started
	fprintf(shared_vars.output_file, PRINT_FORMAT, ++(*shared_vars.action_counter), 'C', process_counter, "started");

	// check whether there are enough adults
	if (
		*shared_vars.current_child_count < 3 * (*shared_vars.current_adult_count)
		|| (adult_count == *shared_vars.adult_counter && ! *shared_vars.current_adult_count)
	) {
		// there are enough adult => enter

		(*shared_vars.current_child_count)++;

		// print: enter
		fprintf(shared_vars.output_file, PRINT_FORMAT, ++(*shared_vars.action_counter), 'C', process_counter, "enter");
		sem_post(semaphores.mutex); /// unlock mutex

	} else {
		// there are not enough adults

		// print: waiting
		fprintf(shared_vars.output_file, PRINT_FORMAT_WAITING, ++(*shared_vars.action_counter), 'C', process_counter,
				"waiting", *shared_vars.current_adult_count, *shared_vars.current_child_count);

		(*shared_vars.child_waiting)++;

		sem_post(semaphores.mutex); /// unlock mutex
		sem_wait(semaphores.child_queue); /// lock child_queue


		sem_wait(semaphores.mutex); /// lock mutex
		{
			// print enter
			fprintf(shared_vars.output_file, PRINT_FORMAT, ++(*shared_vars.action_counter), 'C', process_counter, "enter");
		}
		sem_post(semaphores.mutex); /// unlock mutex
	}


	/****************** Critical section ******************/
	wait_random_time(wait_time); // simulate activity


	/****************** Leaving ******************/
	sem_wait(semaphores.mutex); /// lock mutex
	{
		// print: trying to leave
		fprintf(shared_vars.output_file, PRINT_FORMAT, ++(*shared_vars.action_counter), 'C', process_counter,
				"trying to leave");
		// print: leave
		fprintf(shared_vars.output_file, PRINT_FORMAT, ++(*shared_vars.action_counter), 'C', process_counter,
				"leave");

		(*shared_vars.current_child_count)--;

		// check if there are adults waiting to leave and some can leave
		if (
			*shared_vars.adult_waiting
			&& *shared_vars.current_child_count <= 3 * ((*shared_vars.current_adult_count) - 1)
		) {
			(*shared_vars.adult_waiting)--;
			(*shared_vars.current_adult_count)--;
			sem_post(semaphores.adult_queue); /// unlock adult_queue
		}
	}
	sem_post(semaphores.mutex); /// unlock mutex


	/****************** Exit process ******************/
	exit_process('C', process_counter);
}


pid_t generator_start(int count, int generate_time, int wait_time, int adult_count, void (*action)(int, int))
{
	pid_t pid = fork(); // create new process for generating child processes
	if (pid != 0) {
		// parent process or fork error
		return pid;
	}

	/****************** Child process ******************/
	// start generating
	for (int i = 0; i < count; i++) {
		// wait before generating new process
		wait_random_time(generate_time);

		// create new child process
		pid_t child_pid = fork();
		if (child_pid < 0) {
			// fork error
			return child_pid;

		} else if (child_pid == 0) {
			// child process
			action(wait_time, adult_count); // do callback action
		}

		// parent process -> continue;
	}

	// passive wait for all child processes
	while (wait(NULL)) {
		if (errno == ECHILD) {
			// no child processes
			break;
		}
	}

	// exit child processes generator
	exit(EXIT_SUCCESS);
}
