#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/ioctl.h>
#include <sys/ioctl.h>

#define AGDSP_CTL_PIPE "/dev/audio_pipe_voice"

#define AGDSP_VOICE_CHANNEL 2
#define AGDSP_EFFECT_CHANNEL 8
#define AGDSP_RECORDPROC_CHANNEL 9
#define AGDSP_BTHAL_CHANNEL 11

#define AGDSP_CMD_INIT 0x33

struct dsp_smsg
{
	uint16_t command;	/* command */
	uint16_t channel;	/* channel index */
	uint32_t parameter0;	/* msg parameter0 */
	uint32_t parameter1;	/* msg parameter1 */
	uint32_t parameter2;	/* msg parameter2 */
	uint32_t parameter3;	/* msg parameter3 */
};

int main(int argc, char * argv[])
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

	// ALSA should now be functional via MMAP
	return 0;
}
