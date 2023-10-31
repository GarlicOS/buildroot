#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "input.h"
#include "tinymix.h"
#include "spreadtrum.h"

#define HEADSET_STATE_PATH "/sys/kernel/headset/state"

#define AGDSP_CTL_PIPE "/dev/audio_pipe_voice"

#define AGDSP_VOICE_CHANNEL 2
#define AGDSP_EFFECT_CHANNEL 8
#define AGDSP_RECORDPROC_CHANNEL 9
#define AGDSP_BTHAL_CHANNEL 11

#define AGDSP_CMD_INIT 0x33

struct dsp_smsg
{
	uint16_t command;		/* command */
	uint16_t channel;		/* channel index */
	uint32_t parameter0;	/* msg parameter0 */
	uint32_t parameter1;	/* msg parameter1 */
	uint32_t parameter2;	/* msg parameter2 */
	uint32_t parameter3;	/* msg parameter3 */
};

int init_spreadtrum_agdsp()
{
	// Open "/dev/audio_pipe_voice" (the AGDSP communication pipe)
	int agdsp_pipd_fd = open(AGDSP_CTL_PIPE, O_RDWR);

	// There seems to be no "/dev/audio_pipe_voice" pipe
	if (agdsp_pipd_fd < 0)
	{
		// Exit
		return 0;
	}

	// The initialization status
	int initialized = 0;

	// The AGDSP communication pipe has been opened
	if (agdsp_pipd_fd >= 0)
	{
		// Send AGDSP_CMD_INIT into the AGDSP_VOICE_CHANNEL
		struct dsp_smsg msg;
		memset(&msg, 0, sizeof(struct dsp_smsg));
		msg.channel = AGDSP_VOICE_CHANNEL;
		msg.command = AGDSP_CMD_INIT;
		int written = write(agdsp_pipd_fd, &msg, sizeof(struct dsp_smsg));
		if (written == sizeof(struct dsp_smsg))
		{
			// Wait for the AGDSP reply
			while (1)
			{
				// Clear the message buffer
				memset(&msg, 0, sizeof(struct dsp_smsg));

				// We received the reply to our initial command
				if (read(agdsp_pipd_fd, &msg, sizeof(struct dsp_smsg)) == sizeof(struct dsp_smsg) &&
					msg.channel == AGDSP_VOICE_CHANNEL && msg.command == AGDSP_CMD_INIT)
				{
					// ALSA (via MMAP) should now be functional
					initialized = 1;

					// Break out
					break;
				}
			}
		}
	}

	// Close "/dev/audio_pipe_voice"
	close(agdsp_pipd_fd);

	// Return the initialization status
	return initialized;
}

int init_spreadtrum_alsa()
{
	// Open the mixer
	struct mixer * mixer = mixer_open(0);

	// We failed to open the mixer
	if (mixer == NULL)
	{
		// Exit
		return 0;
	}

	// Set the mixer values
	set_value(mixer, "agdsp_access_en", "1");
	set_value(mixer, "VBC_IIS_TX0_WD_SEL", "WD_24BIT");
	set_value(mixer, "VBC_IIS_TX1_WD_SEL", "WD_24BIT");
	set_value(mixer, "VBC_MUX_DAC0_IIS_PORT_SEL", "VBC_IIS_PORT_IIS0");
	set_value(mixer, "VBC_MUX_DAC1_IIS_PORT_SEL", "VBC_IIS_PORT_IIS0");
	set_value(mixer, "VBC_MUX_IIS0_PORT_DO_SEL", "IIS_DO_VAL_DAC0");
	set_value(mixer, "VBC_MUX_IIS2_PORT_DO_SEL", "IIS_DO_VAL_DAC2");
	set_value(mixer, "VBC_MUX_IIS3_PORT_DO_SEL", "IIS_DO_VAL_DAC2");
	set_value(mixer, "ag_iis0_ext_sel", "enable");
	set_value(mixer, "S_NORMAL_AP01_P_CODEC SWITCH", "1");
	set_value(mixer, "S_NORMAL_AP23_P_CODEC SWITCH", "1");
	set_value(mixer, "S_FAST_P_CODEC SWITCH", "1");
	set_value(mixer, "S_OFFLOAD_CODEC SWITCH", "1");
	set_value(mixer, "S_VOICE_P_CODEC SWITCH", "1");
	set_value(mixer, "S_VOIP_P_CODEC SWITCH", "1");
	set_value(mixer, "S_FM_CODEC SWITCH", "1");
	set_value(mixer, "S_LOOP_P_CODEC SWITCH", "1");
	set_value(mixer, "S_FM_DSP_CODEC SWITCH", "1");
	set_value(mixer, "S_VOICE_PCM_P SWITCH", "1");
	set_value(mixer, "HP mix mode", "0");
	set_value(mixer, "DAC LRCLK Select", "invert");
	set_value(mixer, "HPL EAR Sel", "HPL");
	set_value(mixer, "HPL Mixer DACLHPL Switch", "1");
	set_value(mixer, "HPR Mixer DACRHPR Switch", "1");
	set_value(mixer, "HPR Pin Switch", "1");
	set_value(mixer, "Speaker Function", "1");
	set_value(mixer, "Speaker1 Function", "1");
	char * arr_0_1[2] = { "0", "1" };
	set_values(mixer, "VBC DAC0 DSP MDG Set", arr_0_1, sizeof(arr_0_1) / sizeof(arr_0_1[0]));
	char * arr_0_4[2] = { "0", "4" };
	set_values(mixer, "VBC DAC1 DSP MDG Set", arr_0_4, sizeof(arr_0_4) / sizeof(arr_0_4[0]));
	set_values(mixer, "VBC DAC0 AUD MDG Set", arr_0_1, sizeof(arr_0_1) / sizeof(arr_0_1[0]));
	char * arr_30_30[2] = { "30", "30" };
	set_values(mixer, "VBC DAC0 DG Set", arr_30_30, sizeof(arr_30_30) / sizeof(arr_30_30[0]));
	set_value(mixer, "VBC FM_MUTE_SMOOTHDG STEP", "1");
	set_value(mixer, "Inter PA Config", "303");
	set_value(mixer, "HPL Gain HPL Playback Volume", "6");
	set_value(mixer, "HPR Gain HPR Playback Volume", "6");
	set_value(mixer, "HeadPhone Function", "1");

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
	set_value(mixer, "Speaker Mute", headphones_plugged_in ? "1" : "0");

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

		// Get the volume settings (from the left channel)
		if (get_int_value(mixer, "HPL Gain HPL Playback Volume", &cur_volume, &min_volume, &max_volume))
		{
			// Backup the starting volume
			starting_volume = cur_volume;

			// Bound the minimum volume at 4 (anything below that seems to just mute)
			if (min_volume < 4)
			{
				min_volume = 4;
			}

			// Add the delta to the current volume (yes, gain gets louder as you lower it)
			cur_volume -= delta;

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

			// Set the left channel's new volume
			set_value(mixer, "HPL Gain HPL Playback Volume", new_volume);

			// Set the right channel's new volume
			set_value(mixer, "HPR Gain HPR Playback Volume", new_volume);

			// We caused an actual change in volume
			result = starting_volume != cur_volume;
		}

		// Close the mixer
		mixer_close(mixer);
	}

	// Return the result
	return result;
}

static void input_event(struct input_event * ev, uint16_t jack_event_type, uint16_t jack_event_code)
{
	// We're handling a headphone jack event
	if (ev->type == jack_event_type && ev->code == jack_event_code)
	{
		// Switch the audio route
		switch_audio_route(ev->value);
	}
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

void spreadtrum_update(const char * headphone_jack_device_name, uint16_t jack_event_type, uint16_t jack_event_code)
{
	// The headphone jack device descriptor
	int headphone_jack = -1;

	// The control pipe descriptor
	int control_pipe = -1;

	// The response pipe descriptor
	int response_pipe = -1;

	// Determine the headphone jack input device path
	const char * input_device_path = get_input_device_path(headphone_jack_device_name);

	// We determined the headphone jack input device path
	if (input_device_path != NULL)
	{
		// Open the headphone jack input device
		headphone_jack = open(input_device_path, O_RDONLY | O_NONBLOCK);

		// We managed to open the headphone jack input device
		if (headphone_jack >= 0)
		{
			// Ensure we pick the right route on boot
			switch_audio_route(is_headset_connected());

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
				int maxfd = headphone_jack > control_pipe ? headphone_jack : control_pipe;

				// Initialize the select structure
				fd_set rfds, wfds;
				FD_ZERO(&rfds);
				FD_SET(headphone_jack, &rfds);
				if (control_pipe >= 0 && response_pipe >= 0)
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
					// A headphone jack event was captured
					if (FD_ISSET(headphone_jack, &wfds))
					{
						// The captured input event
						struct input_event ev;

						// We managed to read the event data
						if (read(headphone_jack, &ev, sizeof(ev)) == sizeof(ev))
						{
							// Handle the event
							input_event(&ev, jack_event_type, jack_event_code);
						}
					}

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
			}

			// Close the headphone jack input device
			close(headphone_jack);

			// We have an open control pipe
			if (control_pipe >= 0)
			{
				// Close the control pipe
				close(control_pipe);
			}
		}
	}
}