#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "input.h"
#include "tinymix.h"
#include "rg35xxplus.h"

#define HEADSET_STATE_PATH "/sys/module/snd_soc_sunxi_component_jack/parameters/jack_state"

int init_rg35xxplus_alsa()
{
	// Open the mixer
	struct mixer * mixer = mixer_open(0);

	// We failed to open the mixer
	if (mixer == NULL)
	{
		// Exit
		return 0;
	}

	// The card's default settings work for us so far...
	// Should we ever need to make adjustments on boot then we'll put them here.

	// Close the mixer
	mixer_close(mixer);

	// Exit
	return 1;
}

static void switch_audio_route(int32_t headphones_plugged_in)
{
	// Open the mixer
	struct mixer * mixer = mixer_open(0);

	// We failed to open the mixer
	if (mixer == NULL)
	{
		// Exit
		return;
	}

	// Set the mixer values
	set_value(mixer, "SPK Switch", headphones_plugged_in ? "0" : "1");

	// Close the mixer
	mixer_close(mixer);
}

static int adjust_volume(int delta)
{
	// Whether the delta has been applied
	int result = 0;

	// Open the mixer
	struct mixer * mixer = mixer_open(0);

	// We managed to open the mixer
	if (mixer != NULL)
	{
		// The starting volume
		int starting_volume;

		// The current volume
		int cur_volume;

		// The minimum volume
		int min_volume;

		// The maximum volume
		int max_volume;

		// Get the volume settings
		if (get_int_value(mixer, "digital volume", &cur_volume, &min_volume, &max_volume))
		{
			// Backup the starting volume
			starting_volume = cur_volume;

			// Add the delta to the current volume
			cur_volume += delta;

			// Keep the volume within the min-max boundary
			if (cur_volume > max_volume)
			{
				cur_volume = max_volume;
			}
			else if (cur_volume < min_volume)
			{
				cur_volume = min_volume;
			}

			// Create a buffer to hold the volume string
			char new_volume[16];

			// Print the volume string
			sprintf(new_volume, "%d", cur_volume);

			// Set the new volume
			set_value(mixer, "digital volume", new_volume);

			// We caused an actual change in volume
			result = starting_volume != cur_volume;
		}

		// Close the mixer
		mixer_close(mixer);
	}

	// Return the result
	return result;
}

static int is_headset_connected()
{
	// The current headset state
	int state = 0;

	// Open the headset state device
	FILE * fd = fopen(HEADSET_STATE_PATH, "r");

	// We managed to open the headset state device
	if (fd != NULL)
	{
		// Fetch the state
		state = fgetc(fd) - '0';
	}

	// Return the state
	return state;
}

static void rg35xxplus_update()
{
	// The control pipe descriptor
	int control_pipe = -1;

	// The response pipe descriptor
	int response_pipe = -1;

	// The last headset connected state
	int last_headset_connected = is_headset_connected();

	// Ensure we pick the right route on boot
	switch_audio_route(last_headset_connected);

	// The operation loop
	while (1)
	{
		// The control pipe hasn't been opened yet
		if (control_pipe < 0)
		{
			// Create the named pipe
			mkfifo(AUDIO_CONTROL_PIPE, 0666);

			// Open the control pipe
			control_pipe = open(AUDIO_CONTROL_PIPE, O_RDONLY | O_NONBLOCK);
		}

		// The response pipe hasn't been opened yet
		if (response_pipe < 0)
		{
			// Create the named pipe
			mkfifo(AUDIO_RESPONSE_PIPE, 0666);

			// Open the response pipe
			response_pipe = open(AUDIO_RESPONSE_PIPE, O_WRONLY | O_NONBLOCK);
		}

		// Determine the highest file descriptor (needed for select)
		int maxfd = control_pipe;

		// Initialize the select structure
		fd_set rfds, wfds;
		FD_ZERO(&rfds);

		// Both pipes have been opened
		if (control_pipe >= 0 && response_pipe >= 0)
		{
			// Add the control pipe to the select structure
			FD_SET(control_pipe, &rfds);

			// Set the timeout to 1 second
			struct timeval tv;
			tv.tv_sec = 1;
			tv.tv_usec = 0;

			// Reset the select structure
			memcpy(&wfds, &rfds, sizeof(fd_set));

			// Wait for incoming events
			int select_result = select(maxfd + 1, &wfds, NULL, NULL, &tv);

			// We've received at least one incoming event
			if (select_result > 0)
			{
				// A control pipe event was captured
				if (FD_ISSET(control_pipe, &wfds))
				{
					// The captured input event
					int direction = 0;

					// We managed to read the event data
					if (read(control_pipe, &direction, sizeof(direction)) == sizeof(direction))
					{
						// Adjust the volume
						int result = adjust_volume(direction);

						// Report back the result
						write(response_pipe, &result, sizeof(result));
					}
				}
			}

			// The current headset connected state
			int headset_connected = is_headset_connected();

			// The headset state changed
			if (headset_connected != last_headset_connected)
			{
				// Pick the right audio route
				switch_audio_route(headset_connected);

				// Keep track of the headset state
				last_headset_connected = headset_connected;
			}
		}
	}

	// We have an open control pipe
	if (control_pipe >= 0)
	{
		// Close the control pipe
		close(control_pipe);
	}
}

int rg35xxplus_main(int argc, char * argv[])
{
	// Initialize ALSA
	if (init_rg35xxplus_alsa())
	{
		// React to audio events (jack events, volume buttons, etc)
		rg35xxplus_update();
	}

	// Exit
	return 0;
}