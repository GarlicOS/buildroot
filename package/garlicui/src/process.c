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
		// Open the command file
		FILE * command_file = fopen("/tmp/command", "w");

		// We managed to open the command file
		if (command_file != NULL)
		{
			// Iterate all arguments
			for (int i = 0; args[i] != NULL; i++)
			{
				// The argument contains whitespaces
				if (strchr(args[i], ' ') != NULL)
				{
					// Write the argument to the file
					fprintf(command_file, "\"%s\"", args[i]);
				}

				// The argument doesn't contain whitespaces
				else
				{
					// Write the argument to the file
					fprintf(command_file, "%s", args[i]);
				}

				// We haven't reached the last argument yet
				if (args[i + 1] != NULL)
				{
					// Which means we need a space separator
					fprintf(command_file, " ");
				}
			}

			// Close the command file
			fclose(command_file);

			// Sync all changes to disk
			io_sync();

			// Quit SDL (which helps avoid a fbcon driver black screen issue)
			SDL_Quit();

			// Exit the application
			exit(0);

			// Exit the function
			return 0;
		}
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