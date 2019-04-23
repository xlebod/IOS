/**
 * @file proj2.c
 * @brief Child Care problem.
 * @author Dominik Harmim <xharmi00@stud.fit.vutbr.cz>
 * @date 28.4.2017
 * @version 1.0
 */


#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "argument_processor.h"
#include "resources.h"
#include "process_generator.h"


#define EXPECTED_ARGUMENTS_COUNT 6 /// Expected number of program arguments.
#define MIN_PROCESSES_COUNT 0 /// Minimum number of acceptable processes (arguments: A and C).
#define MIN_PROCESSES_DELAY_TIME 0 /// Minimum delay for processes (arguments: AGT, CGT, AWT and CWT).
#define MAX_PROCESSES_DELAY_TIME 5001 /// Maximum delay for processes (arguments: AGT, CGT, AWT and CWT).

#define EXIT_FAILURE_SYS_CALL 2 /// System call error exit code.
#define SYS_CALL_ERROR_MESSAGE "System call error" /// System call error message.


/// Usage string (help).
const char USAGE_STRING[] =
		"usage:\n"
		"    proj2 A C AGT CGT AWT CWT\n"
		"        - A is number of adult process; A > 0.\n"
		"        - C is number of child process; C > 0.\n"
		"        - AGT max time after which a new adult process is generated;\n"
		"              AGT >= 0 && AGT < 5001.\n"
		"        - CGT max time after which a new child process is generated;\n"
		"              CGT >= 0 && CGT < 5001.\n"
		"        - AWT max time for which the adult process simulates activity\n"
		"              in the center; AWT >= 0 && AWT < 5001.\n"
		"        - CWT max time for which the child process simulates activity\n"
		"              in the center; CWT >= 0 && CWT < 5001.\n"
		"        - Time is in milliseconds.\n"
		"        - All parameters are integers.\n"
		"\n";


/**
 * Condition callback for testing number of processes (arguments: A and C).
 *
 * @param arg Argument to check.
 * @return True if argument is valid, false otherwise.
 */
static bool number_of_processes_condition(struct argument *arg)
{
	return arg->value.i_value > MIN_PROCESSES_COUNT;
}


/**
 * Condition callback for testing delay for processes (arguments: AGT, CGT, AWT and CWT).
 *
 * @param arg Argument to check.
 * @return True if argument is valid, false otherwise.
 */
static bool delay_for_process_condition(struct argument *arg)
{
	return arg->value.i_value >= MIN_PROCESSES_DELAY_TIME && arg->value.i_value < MAX_PROCESSES_DELAY_TIME;
}


/**
 * Set expected program arguments.
 *
 * @param expected Output structure of expected arguments.
 */
static void set_expected_arguments(struct expected_arguments *expected)
{
	struct expected_argument *arg;

	// A
	arg = &expected->args[0];
	arg->type = ARG_TYPE_INTEGER;
	arg->condition = &number_of_processes_condition;

	// C
	arg = &expected->args[1];
	arg->type = ARG_TYPE_INTEGER;
	arg->condition = &number_of_processes_condition;

	// AGT
	arg = &expected->args[2];
	arg->type = ARG_TYPE_INTEGER;
	arg->condition = &delay_for_process_condition;

	// CGT
	arg = &expected->args[3];
	arg->type = ARG_TYPE_INTEGER;
	arg->condition = &delay_for_process_condition;

	// AWT
	arg = &expected->args[4];
	arg->type = ARG_TYPE_INTEGER;
	arg->condition = &delay_for_process_condition;

	// CWT
	arg = &expected->args[5];
	arg->type = ARG_TYPE_INTEGER;
	arg->condition = &delay_for_process_condition;
}


/**
 * Get program arguments.
 *
 * @param argc Number of arguments.
 * @param argv Argument values.
 * @param arguments Output arguments sctructure.
 * @return EXIT_SUCCESS if getting was seccessful, error code otherwise.
 */
static int get_arguments(int argc, char *argv[], struct argument *arguments[])
{
	// inicialization of expected arguments structure
	struct expected_arguments expected_args;
	if ( ! init_expected_arguments(&expected_args, EXPECTED_ARGUMENTS_COUNT)) {
		perror(SYS_CALL_ERROR_MESSAGE);

		return EXIT_FAILURE_SYS_CALL;
	}

	// setting expected arguments
	set_expected_arguments(&expected_args);
	// process arguments
	if ( ! process_arguments(argc, argv, arguments, &expected_args)) {
		fprintf(stderr, "%s: invalid arguments.\n\n%s", argv[0], USAGE_STRING);
		// clean expected arguments structure
		clean_expected_arguments(&expected_args);

		return EXIT_FAILURE;
	}

	// clean expected arguments structure
	clean_expected_arguments(&expected_args);

	return EXIT_SUCCESS;
}


/**
 * SIGUSR1 signal handler.
 *
 * @param signal Singal code.
 */
static void usr_signal_handler(int signal)
{
	(void) signal;

	_Exit(EXIT_FAILURE_SYS_CALL);
}


/**
 * Inicialization.
 * Setting errno if case of error.
 *
 * @param adult_arg_count Argument A.
 * @param child_arg_count Argument C.
 * @return True if inicialization was seccessful, false otherwise.
 */
static bool inicialize(int adult_arg_count, int child_arg_count)
{
	// disable output buffering
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);

	srandom((unsigned) time(NULL)); // initialize random number generator

	// set signal handlers
	struct sigaction usr_act = {
			.sa_handler = &usr_signal_handler,
			.sa_flags = 0,
	};
	if (sigaction(SIGUSR1, &usr_act, NULL)) {
		return false;
	}

	// set global resources
	if ( ! set_resources(adult_arg_count + child_arg_count)) {
		clean_resources();

		return false;
	}

	return true;
}


/**
 * Main function.
 *
 * @param argc Number of arguments.
 * @param argv Argument values.
 * @return EXIT_SUCCESS, if run was seccessful, error code otherwise.
 */
int main(int argc, char *argv[])
{
	/****************** Processing arguments ******************/
	// inicialization of input arguments structure
	struct argument *arguments;
	if ( ! init_arguments(&arguments, EXPECTED_ARGUMENTS_COUNT)) {
		perror(SYS_CALL_ERROR_MESSAGE);

		return EXIT_FAILURE_SYS_CALL;
	}

	// get arguments
	int error_code = get_arguments(argc, argv, &arguments);
	if (error_code) {
		clean_arguments(&arguments); // clean input arguments structure

		return error_code;
	}

	int arg_a = arguments[0].value.i_value,
		arg_c = arguments[1].value.i_value,
		arg_agt = arguments[2].value.i_value,
		arg_cgt = arguments[3].value.i_value,
		arg_awt = arguments[4].value.i_value,
		arg_cwt = arguments[5].value.i_value;

	clean_arguments(&arguments); // clean input arguments structure


	/****************** Inicialize ******************/
	if ( ! inicialize(arg_a, arg_c)) {
		perror(SYS_CALL_ERROR_MESSAGE);

		return EXIT_FAILURE_SYS_CALL;
	}


	/****************** Process generating ******************/
	pid_t adult_pid, child_pid;
	if (
		(adult_pid = generator_start(arg_a, arg_agt, arg_awt, arg_a, &adult_action)) < 0
		|| (child_pid = generator_start(arg_c, arg_cgt, arg_cwt, arg_a, &child_action)) < 0
	) {
		perror(SYS_CALL_ERROR_MESSAGE);
		clean_resources();
		kill(0, SIGUSR1); // send SIGUSR1 to all processes in group

		return EXIT_FAILURE_SYS_CALL;
	}


	/****************** Waiting for exit processes ******************/
	waitpid(adult_pid, NULL, 0);
	waitpid(child_pid, NULL, 0);


	/****************** Clean ******************/
	clean_resources();


	return EXIT_SUCCESS;
}
