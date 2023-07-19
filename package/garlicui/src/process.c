#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "process.h"

/**
 * @brief Clones the given arguments.
 */
static char ** clone_arguments(const char * args[])
{
	// First, determine the size of the argument list
	int size = 0;
	while (args[size] != NULL)
	{
		size++;
	}

	// Allocate memory for the duplicated argument list
	char ** duplicate = (char **)malloc((size + 1) * sizeof(char *));
	if (duplicate != NULL)
	{
		// Copy each argument using strdup
		for (int i = 0; i < size; i++)
		{
			duplicate[i] = strdup(args[i]);
		}

		// Add the NULL terminator at the end
		duplicate[size] = NULL;
	}

	// And return the duplicated arguments
	return duplicate;
}

/**
 * @brief Runs the given process, waits for it to exit and returns its exit code.
 */
int process_run(struct gui_context * context, const char * args[], int replace_parent)
{
	// The exit code of the child process
	int exit_code = -1;

	// The user wants to replace the parent process
	if (replace_parent)
	{
		// Clone the arguments (the original will no longer be available after destroying the GUI context)
		char ** clone_args = clone_arguments(args);

		// Destroy the GUI context (which in turn shuts down SDL, which in turn closes the /dev/fb* descriptor)
		gui_destroy_context(context);

		// Replace the current process with the new process
		execvp(args[0], (char * const*)clone_args);

		// NOTE: If execvp ever fails at this point then we're royally screwed...
	}

	// The user wants to start a child process
	else
	{
		// Create a new process
		pid_t pid = fork();

		// We're inside the new child process
		if (pid == 0)
		{
			// Replace the bootstrap process with the actual process
			execvp(args[0], (char *const *)args);
		}

		// We're inside the parent process
		else if (pid > 0)
		{
			// The child process status
			int status;

			// Wait for the child process to exit
			waitpid(pid, &status, 0);

			// The child process exited gracefully
			if (WIFEXITED(status))
			{
				// Return the exit code of the child process
				exit_code = WEXITSTATUS(status);
			}
		}
	}

	// Return the exit code of the child process
	return exit_code;
}