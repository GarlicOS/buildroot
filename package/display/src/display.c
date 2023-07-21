#include <string.h>
#include <stdlib.h>

#include "rg35xxplus.h"

int main(int argc, char * argv[])
{
	// Get the device codename
	char * code_name = getenv("DEVICE_CODENAME");

	// The bootloader didn't set the codename properly
	if (code_name == NULL)
	{
		// Which means we can't provide display route control
		return 1;
	}

	// We're handling the Anbernic RG35XX+
	if (strcmp(code_name, "rg35xx+") == 0)
	{
		// Initialize external displays and handle routing
		rg35xxplus_main(argc, argv);
	}

	// Exit
	return 0;
}