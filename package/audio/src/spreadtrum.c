#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>

#include "input.h"
#include "tinymix.h"
#include "spreadtrum.h"

#define DEFAULT_BRIGHTNESS 72
#define BRIGHTNESS_PATH "/sys/devices/platform/sprd_backlight/backlight/sprd_backlight/brightness"
#define PERSISTENT_BRIGHTNESS_PATH "/root/.config/brightness"

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

static void adjust_volume(int delta)
{
	// TODO: Move this to the gamepad provider package and communicate with the audio provider via sockets

	// Open the mixer
	struct mixer * mixer = mixer_open(0);

	// We failed to open the mixer
	if (mixer == NULL)
	{
		// Exit
		return;
	}

	// The current volume
	int cur_volume;

	// The minimum volume
	int min_volume;

	// The maximum volume
	int max_volume;

	// Get the volume settings (from the left channel)
	if (get_int_value(mixer, "HPL Gain HPL Playback Volume", &cur_volume, &min_volume, &max_volume))
	{
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
	}

	// Close the mixer
	mixer_close(mixer);
}

static void adjust_brightness(int delta, int absolute)
{
	// TODO: Move this to the gamepad provider package and communicate with the audio provider via sockets
	// TODO: Maybe we should turn all of this into a brightness path / min & max value quirk to make porting easier

	// Get the brightness path
	const char * brightness_path = BRIGHTNESS_PATH;

	// Read the current brightness value
	unsigned int current_brightness = DEFAULT_BRIGHTNESS;
	FILE * file = fopen(brightness_path, "r");
	if (file != NULL)
	{
		fscanf(file, "%u", &current_brightness);
		fclose(file);
	}

	// Adjust the brightness value
	current_brightness += delta;

	// Keep the brightness within the valid range
	if (current_brightness <= 0)
		current_brightness = 1;
	if (current_brightness >= 255)
		current_brightness = 255;

	// Write the updated brightness value back to the file
	file = fopen(brightness_path, "w");
	if (file != NULL)
	{
		fprintf(file, "%u", current_brightness);
		fclose(file);
	}

	// Write a copy to the user's .config directory so it can be restored on reboot
	file = fopen(PERSISTENT_BRIGHTNESS_PATH, "w");
	if (file != NULL)
	{
		fprintf(file, "%u", current_brightness);
		fclose(file);
	}
}

void restore_brightness()
{
	// TODO: Move this to the gamepad provider package and communicate with the audio provider via sockets
	// TODO: Maybe we should turn all of this into a brightness path / min & max value quirk to make porting easier

	// Get the brightness path
	const char * brightness_path = BRIGHTNESS_PATH;

	// The brightness value
	unsigned int brightness = DEFAULT_BRIGHTNESS;

	// Read the brightness value
	FILE * file = fopen(PERSISTENT_BRIGHTNESS_PATH, "r");
	if (file != NULL)
	{
		fscanf(file, "%u", &brightness);
		fclose(file);
	}

	// Keep the brightness within the valid range
	if (brightness <= 0)
		brightness = 1;
	if (brightness >= 255)
		brightness = 255;

	// Write the updated brightness to the file
	file = fopen(brightness_path, "w");
	if (file != NULL)
	{
		fprintf(file, "%u", brightness);
		fclose(file);
	}
}

static void input_event(struct input_event * ev, uint16_t jack_event_type, uint16_t jack_event_code, uint16_t volume_event_type, uint16_t volume_down_event_code, uint16_t volume_up_event_code, uint16_t modifier_event_type, uint16_t modifier_event_code)
{
	// The modifier key state
	static int is_modifier_held = 0;

	// The volume direction
	int volume_direction = 0;

	// We're handling a modifier event
	if (ev->type == modifier_event_type && ev->code == modifier_event_code)
	{
		// Set the modifier key state
		is_modifier_held = ev->value != 0;
	}

	// We're handling a headphone jack event
	if (ev->type == jack_event_type && ev->code == jack_event_code)
	{
		// Switch the audio route
		switch_audio_route(ev->value);
	}

	// We're handling a volume / brightness down event
	else if (ev->type == volume_event_type && ev->code == volume_down_event_code && ev->value != 0)
	{
		// Set the volume direction
		volume_direction = -1;
	}

	// We're handling a volume / brightness up event
	else if (ev->type == volume_event_type && ev->code == volume_up_event_code && ev->value != 0)
	{
		// Set the volume direction
		volume_direction = 1;
	}

	// The modifier key is held
	if (is_modifier_held)
	{
		// Adjust the brightness
		adjust_brightness(volume_direction, 0);
	}

	// The modifier key isn't held
	else
	{
		// Adjust the volume
		adjust_volume(volume_direction);
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

void spreadtrum_update(const char * headphone_jack_device_name, uint16_t jack_event_type, uint16_t jack_event_code, const char * volume_control_device_name, uint16_t volume_event_type, uint16_t volume_down_event_code, uint16_t volume_up_event_code, const char * gamepad_device_name, uint16_t modifier_event_type, uint16_t modifier_event_code)
{
	// The headphone jack device descriptor
	int headphone_jack = -1;

	// The volume control device descriptor
	int volume_control = -1;

	// The gamepad device descriptor
	int gamepad = -1;

	// Determine the headphone jack input device path
	const char * input_device_path = get_input_device_path(headphone_jack_device_name);

	// We determined the headphone jack input device path
	if (input_device_path != NULL)
	{
		// Open the headphone jack input device
		headphone_jack = open(input_device_path, O_RDONLY | O_NONBLOCK);
	}

	// Determine the volume control input device path
	input_device_path = get_input_device_path(volume_control_device_name);

	// We determined the volume control input device path
	if (input_device_path != NULL)
	{
		// Open the volume control input device
		volume_control = open(input_device_path, O_RDONLY | O_NONBLOCK);
	}

	// Determine the gamepad input device path
	input_device_path = get_input_device_path(gamepad_device_name);

	// We determined the volume control input device path
	if (input_device_path != NULL)
	{
		// Open the volume control input device
		gamepad = open(input_device_path, O_RDONLY | O_NONBLOCK);
	}

	// Ensure we pick the right route on boot
	switch_audio_route(is_headset_connected());

	// Determine the highest file descriptor (needed for select)
	int maxfd = headphone_jack;
	if (maxfd < volume_control)
		maxfd = volume_control;
	if (maxfd < gamepad)
		maxfd = gamepad;

	// Initialize the select structure
	fd_set rfds, wfds;
	FD_ZERO(&rfds);
	if (headphone_jack >= 0)
		FD_SET(headphone_jack, &rfds);
	if (volume_control >= 0)
		FD_SET(volume_control, &rfds);
	if (gamepad >= 0)
		FD_SET(gamepad, &rfds);

	// The operation loop
	while (1)
	{
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
			// The captured input event
			struct input_event ev;

			// A headphone jack event was captured
			if (FD_ISSET(headphone_jack, &wfds))
			{
				// We managed to read the event data
				if (read(headphone_jack, &ev, sizeof(ev)) == sizeof(ev))
				{
					// Handle the event
					input_event(&ev, jack_event_type, jack_event_code, volume_event_type, volume_down_event_code, volume_up_event_code, modifier_event_type, modifier_event_code);
				}
			}

			// A volume control event was captured
			if (FD_ISSET(volume_control, &wfds))
			{
				// We managed to read the event data
				if (read(volume_control, &ev, sizeof(ev)) == sizeof(ev))
				{
					// Handle the event
					input_event(&ev, jack_event_type, jack_event_code, volume_event_type, volume_down_event_code, volume_up_event_code, modifier_event_type, modifier_event_code);
				}
			}

			// A gamepad event was captured
			if (FD_ISSET(gamepad, &wfds))
			{
				// We managed to read the event data
				if (read(gamepad, &ev, sizeof(ev)) == sizeof(ev))
				{
					// Handle the event
					input_event(&ev, jack_event_type, jack_event_code, volume_event_type, volume_down_event_code, volume_up_event_code, modifier_event_type, modifier_event_code);
				}
			}
		}
	}

	// Close the headphone jack input device
	close(headphone_jack);

	// Close the volume control input device
	close(volume_control);
}