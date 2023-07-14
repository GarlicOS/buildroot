#include <linux/input.h>

#include "spreadtrum.h"
#include "pocket2plus.h"

int pocket2plus_main(int argc, char * argv[])
{
	// Initialize AGDSP
	if (init_spreadtrum_agdsp())
	{
		// Initialize ALSA
		if (init_spreadtrum_alsa())
		{
			// React to audio events (jack events, volume buttons, etc)
			spreadtrum_update("sprdphone-sc2730 Headset Jack", EV_SW, KEY_1);
		}
	}

	// Exit
	return 0;
}