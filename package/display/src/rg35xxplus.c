#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "sunxi.h"
#include "rg35xxplus.h"

#define HDMI_STATE_PATH "/sys/class/extcon/hdmi/state"
#define HDMI_STATE_TRIGGER_COUNT 5

static unsigned int last_hdmi_connected;
static unsigned int last_hdmi_repeat;

static unsigned int is_hdmi_connected()
{
	// The current HDMI state
	unsigned int state = 0;

	// Open the HDMI state device
	FILE * fd = fopen(HDMI_STATE_PATH, "r");

	// We managed to open the HDMI state device
	if (fd != NULL)
	{
		// Fetch the state
		fscanf(fd, "HDMI=%u", &state);

		// Close the HDMI state device
		fclose(fd);
	}

	// Return the state
	return state;
}

static void switch_display_route(disp_output_type output_type)
{
	// Switch to the given display
	switch_sunxi_device(output_type, DISP_TV_MOD_720P_60HZ);

	// Enable vsync
	toggle_vsync(true);
}

static int init_rg35xxplus_display()
{
	// The last HDMI connected state
	last_hdmi_connected = is_hdmi_connected();

	// Prevent needless wait situations / double-taps on system boot
	last_hdmi_repeat = HDMI_STATE_TRIGGER_COUNT;

	// Switch to the most suitable display
	switch_display_route(last_hdmi_connected ? DISP_OUTPUT_TYPE_HDMI : DISP_OUTPUT_TYPE_LCD);

	// Exit
	return 1;
}

static void rg35xxplus_update()
{
	// The operation loop
	while (1)
	{
		// The current HDMI connected state
		int hdmi_connected = is_hdmi_connected();

		// The HDMI state changed
		if (hdmi_connected != last_hdmi_connected)
		{
			// Keep track of the HDMI state
			last_hdmi_connected = hdmi_connected;

			// Reset the repeat counter
			last_hdmi_repeat = 1;
		}

		// The HDMI state hasn't changed
		else if (last_hdmi_repeat < HDMI_STATE_TRIGGER_COUNT)
		{
			// Increment the repeat counter
			last_hdmi_repeat++;

			// We need to change the display route
			if (last_hdmi_repeat == HDMI_STATE_TRIGGER_COUNT)
			{
				// Switch to the most suitable display
				switch_display_route(hdmi_connected ? DISP_OUTPUT_TYPE_HDMI : DISP_OUTPUT_TYPE_LCD);
			}
		}

		// Wait a bit
		usleep(20000);
	}
}

int rg35xxplus_main(int argc, char * argv[])
{
	// Initialize display
	if (init_rg35xxplus_display())
	{
		// React to HDMI plug events
		rg35xxplus_update();
	}

	// Exit
	return 0;
}