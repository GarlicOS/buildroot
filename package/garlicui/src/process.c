#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "process.h"

/**
 * @brief Runs the given process, waits for it to exit and returns its exit code.
 */
int process_run(const char * args[], int replace_parent)
{
	// The exit code of the child process
	int exit_code = -1;

	// The user wants to replace the parent process
	if (replace_parent)
	{
		// Replace the current process with the new process
		execvp(args[0], (char * const*)args);
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