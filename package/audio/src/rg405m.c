#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>

#include "tinymix.h"
#include "rg405m.h"

#define AGDSP_CTL_PIPE "/dev/audio_pipe_voice"

#define AGDSP_VOICE_CHANNEL 2
#define AGDSP_EFFECT_CHANNEL 8
#define AGDSP_RECORDPROC_CHANNEL 9
#define AGDSP_BTHAL_CHANNEL 11

#define AGDSP_CMD_INIT 0x33

#define HEADPHONE_JACK "/dev/input/event5"

struct dsp_smsg
{
	uint16_t command;		/* command */
	uint16_t channel;		/* channel index */
	uint32_t parameter0;	/* msg parameter0 */
	uint32_t parameter1;	/* msg parameter1 */
	uint32_t parameter2;	/* msg parameter2 */
	uint32_t parameter3;	/* msg parameter3 */
};

static int init_alsa()
{
	// Open the mixer
	struct mixer * mixer = mixer_open(0);
	
	// We failed to open the mixer
	if (mixer == NULL)
	{
		// Exit
		return 1;
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
	return 0;
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

static void input_event(struct input_event * ev)
{
	// We're handling a switch event
	if (ev->type == EV_SW)
	{
		// A headphone jack event occured
		if (ev->code == KEY_1)
		{
			// Switch the audio route
			switch_audio_route(ev->value);
		}
	}
}

int rg405m_main(int argc, char * argv[])
{
	// Open "/dev/audio_pipe_voice" (the AGDSP communication pipe)
	int agdsp_pipd_fd = open(AGDSP_CTL_PIPE, O_RDWR);

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
					break;
				}
			}
		}
	}

	// Close "/dev/audio_pipe_voice"
	close(agdsp_pipd_fd);

	// Initialize ALSA
	if (init_alsa() == 0)
	{
		// Open the "sprdphone-sc2730 Headset Jack" input device (fires the headphone jack event)
		int headphone_jack = open(HEADPHONE_JACK, O_RDONLY | O_NONBLOCK);

		// Determine the highest file descriptor (needed for select)
		int maxfd = headphone_jack;

		// Initialize the select structure
		fd_set rfds, wfds;
		FD_ZERO(&rfds);
		FD_SET(headphone_jack, &rfds);

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
						// Pass the event through to our merged gamepad
						input_event(&ev);
					}
				}
			}
		}
		
		// Close the gpio-keys input device
		close(headphone_jack);
	}

	// Exit
	return 0;
}
