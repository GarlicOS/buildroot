#include <string.h>
#include <stdlib.h>

#include "rg405.h"
#include "rg35xxplus.h"
#include "pocket2plus.h"
#include "pocketflip.h"

int main(int argc, char * argv[])
{
	// Get the device codename
	char * code_name = getenv("DEVICE_CODENAME");

	// The bootloader didn't set the codename properly
	if (code_name == NULL)
	{
		// Which means we can't provide audio
		return 1;
	}

	// We're handling the Anbernic RG405
	if (strcmp(code_name, "rg405") == 0)
	{
		// Initialize the soundcard and handle routing
		rg405_main(argc, argv);
	}

	// We're handling the Anbernic RG35XX+
	else if (strcmp(code_name, "rg35xx+") == 0)
	{
		// Initialize the soundcard and handle routing
		rg35xxplus_main(argc, argv);
	}

	// We're handling the Retroid Pocket 2+
	else if (strcmp(code_name, "pocket2plus") == 0)
	{
		// Initialize the soundcard and handle routing
		pocket2plus_main(argc, argv);
	}

	// We're handling the Retroid Pocket Flip
	else if (strcmp(code_name, "pocketflip") == 0)
	{
		// Initialize the soundcard and handle routing
		pocketflip_main(argc, argv);
	}

	// Exit
	return 0;
}
