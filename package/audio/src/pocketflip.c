#include <linux/input.h>

#include "spreadtrum.h"
#include "pocketflip.h"

int pocketflip_main(int argc, char * argv[])
{
	// Initialize AGDSP
	if (init_spreadtrum_agdsp())
	{
		// Initialize ALSA
		if (init_spreadtrum_alsa())
		{
			// TODO: Figure out what we can use as a modifier key on the Flip... but the following TODO probably moves this issue out of scope anyway...
			// TODO: Move this to the gamepad provider package and communicate with the audio provider via sockets

			// Restore the brightness
			restore_brightness();

			// React to audio events (jack events, volume buttons, etc)
			spreadtrum_update("sprdphone-sc2730 Headset Jack", EV_SW, KEY_1, "gpio-keys", EV_KEY, KEY_VOLUMEDOWN, KEY_VOLUMEUP, "retrogame_joypad", EV_KEY, KEY_BACK);
		}
	}

	// Exit
	return 0;
}
