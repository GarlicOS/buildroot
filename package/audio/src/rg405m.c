#include <linux/input.h>

#include "spreadtrum.h"
#include "rg405m.h"

int rg405m_main(int argc, char * argv[])
{
	// Initialize AGDSP
	if (init_spreadtrum_agdsp())
	{
		// Initialize ALSA
		if (init_spreadtrum_alsa())
		{
			// Restore the brightness
			restore_brightness();

			// React to audio events (jack events, volume buttons, etc)
			spreadtrum_update("sprdphone-sc2730 Headset Jack", EV_SW, KEY_1, "gpio-keys", EV_KEY, KEY_VOLUMEDOWN, KEY_VOLUMEUP, "retrogame_joypad", EV_KEY, KEY_BACK);
		}
	}

	// Exit
	return 0;
}
