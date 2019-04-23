/**
 * @file argument_processor.c
 * @brief Argument processor implementation.
 * @author Dominik Harmim <xharmi00@stud.fit.vutbr.cz>
 * @date 28.4.2017
 */


#include <stdlib.h>
#include "argument_processor.h"


bool init_arguments(struct argument *arguments[], int count)
{
	if ( ! (*arguments = malloc(count * sizeof (struct argument)))) {
		return false;
	}

	return true;
}


void clean_arguments(struct argument *arguments[])
{
	free(*arguments);
}


bool init_expected_arguments(struct expected_arguments *expected, int count)
{
	expected->count = count;
	if ( ! (expected->args = malloc(count * sizeof (struct expected_argument)))) {
		return false;
	}

	return true;
}


void clean_expected_arguments(struct expected_arguments *expected)
{
	free(expected->args);
}


bool process_arguments(int argc, char *argv[], struct argument *return_arguments[], struct expected_arguments *expected)
{
	// ignore first argument, which is name of program
	argc--;
	argv++;

	// wrong number of arguments
	if (expected->count != argc) {
		return false;
	}

	char *endptr = NULL;
	struct argument arg;
	for (int i = 0; i < argc; i++) {
		// expected argument is integer
		if (expected->args[i].type == ARG_TYPE_INTEGER) {
			arg.type = ARG_TYPE_INTEGER;

			// convert string value of argument to integer
			arg.value.i_value = (int) strtol(argv[i], &endptr, 10);
			// check convertion output and input condition
			if (*endptr || ! expected->args[i].condition(&arg)) {
				return false;
			}

			(*return_arguments)[i] = arg;
		}
	}

	return true;
}
