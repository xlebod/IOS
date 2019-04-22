/**
 * @file argument_processor.h
 * @brief Argument processor interface.
 * @author Dominik Harmim <xharmi00@stud.fit.vutbr.cz>
 * @date 28.4.2017
 */


#ifndef PROJ2_ARGUMENT_PROCESSOR_H
#define PROJ2_ARGUMENT_PROCESSOR_H


#include <stdbool.h>


/**
 * @enum Argument type.
 */
enum arg_type {
	ARG_TYPE_INTEGER, /// Integer type.
};


/**
 * @union Argument value.
 */
union arg_value {
	int i_value; /// Value type of integer.
};


/**
 * @struct Struct for argument.
 */
struct argument {
	enum arg_type type; /// Type of argument.
	union arg_value value; /// Value of argument.
};


/**
 * @struct Struct for expected argument.
 */
struct expected_argument {
	enum arg_type type; /// Type of argument.
	bool (*condition)(struct argument *arg); /// Condition callback for argument.
};


/**
 * @struct Struct for collection of expected arguments.
 */
struct expected_arguments {
	int count; /// Arguments count.
	struct expected_argument *args; /// Array of expected arguments.
};


/**
 * Inicialize collection of structs for argument.
 * Setting errno if case of error.
 *
 * @param arguments Collection of structs to inicialize.
 * @param count Number of arguments to inicialize.
 * @return True if successful, false otherwise.
 */
bool init_arguments(struct argument *arguments[], int count);


/**
 * Clean collection of structs for arguments.
 *
 * @param arguments Collection for structs to clear.
 */
void clean_arguments(struct argument *arguments[]);


/**
 * Inicialize struct for collection of expected arguments.
 * Setting errno if case of error.
 *
 * @param expected Struct for collection of expected arguments to inicialize.
 * @param count Number of arguments to inicialize.
 * @return Struct for collection of expected arguments to inicialize.
 */
bool init_expected_arguments(struct expected_arguments *expected, int count);


/**
 * Clean struct for collection of expected arguments.
 *
 * @param expected Struct for collection of expected arguments to clean.
 */
void clean_expected_arguments(struct expected_arguments *expected);


/**
 * Process arguments.
 *
 * @param argc Number of arguments.
 * @param argv Argument values.
 * @param return_arguments Collection of structs for output arguments.
 * @param expected Struct for collection of expected arguments.
 * @return True if process was seccessful, false otherwise.
 */
bool process_arguments(int argc, char *argv[], struct argument *return_arguments[], struct expected_arguments *expected);


#endif //PROJ2_ARGUMENT_PROCESSOR_H
