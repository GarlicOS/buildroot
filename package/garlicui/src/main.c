#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "gui.h"

/**
 * @brief The GarlicUI entry point.
 */
int main(int argc, char * argv[])
{
	// The exit code
	int result = -1;

	// Create a GarlicUI context
	struct gui_context * context = gui_create_context(argc, argv);

	// We managed to create a GarlicUI context
	if (context != NULL)
	{
		// The application's update & render loop
		while (context->status.application.running)
		{
			// Update the context
			gui_update(context);

			// Render the context
			gui_render(context);
		}

		// Return the context's exit code
		result = context->status.application.exit_code;

		// Destroy the context
		gui_destroy_context(context);
	}

	// Return the exit code
	return result;
}