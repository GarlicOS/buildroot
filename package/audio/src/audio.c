#include <string.h>
#include <stdlib.h>

#include "rg405m.h"
#include "pocket2plus.h"

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
	
	// We're handling the Anbernic RG405M
	if (strcmp(code_name, "rg405m") == 0)
	{
		// Initialize the soundcard and handle routing
		rg405m_main(argc, argv);
	}

	// We're handling the Retroid Pocket 2+
	if (strcmp(code_name, "pocket2plus") == 0)
	{
		// Initialize the soundcard and handle routing
		pocket2plus_main(argc, argv);
	}
	
	// Exit
	return 0;
}
