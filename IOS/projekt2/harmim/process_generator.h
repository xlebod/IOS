/**
 * @file process_generator.h
 * @brief Process generator interface.
 * @author Dominik Harmim <xharmi00@stud.fit.vutbr.cz>
 * @date 28.4.2017
 */


#ifndef PROJ2_PROCESS_GENERATOR_H
#define PROJ2_PROCESS_GENERATOR_H


#include <sys/types.h>


#define PRINT_FORMAT "%-6i : %c %-4i : %s\n" /// Format string for print action.
#define PRINT_FORMAT_WAITING "%-6i : %c %-4i : %s : %i : %i\n" /// Format string for print waiting action.


/**
 * Action which do each adult process.
 *
 * @param wait_time Max time for which the process simulates activity in the center.
 * @param child_count Child processes total count.
 */
void adult_action(int wait_time, int child_count);


/**
 * Action which do each child process.
 *
 * @param wait_time Max time for which the process simulates activity in the center.
 * @param adult_count Adult processes total count.
 */
void child_action(int wait_time, int adult_count);


/**
 * Start generate processes.
 * Setting errno if case of error.
 *
 * @param count Processes to generate count.
 * @param generate_time Max time after which a new process is generated.
 * @param wait_time Max time for which the process simulates activity in the center.
 * @param adult_count Adult processes total count.
 * @param action Callback to action which do each generated process.
 * @return PID of process which generating child processes or < 0 in case of error.
 */
pid_t generator_start(
	int count,
	int generate_time,
	int wait_time,
	int adult_count,
	void (*action)(int, int)
);


#endif //PROJ2_PROCESS_GENERATOR_H
