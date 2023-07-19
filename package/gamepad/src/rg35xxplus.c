#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/input.h>
#include <linux/uinput.h>

#include "gamepad.h"
#include "rg35xxplus.h"

static int translate_scancode(int scancode)
{
	switch (scancode)
	{
		case RG35XXPLUS_SCANCODE_HOME:
			return MERGED_SCANCODE_HOME;
		case RG35XXPLUS_SCANCODE_A:
			return MERGED_SCANCODE_B;
		case RG35XXPLUS_SCANCODE_B:
			return MERGED_SCANCODE_A;
		case RG35XXPLUS_SCANCODE_Y:
			return MERGED_SCANCODE_X;
		case RG35XXPLUS_SCANCODE_X:
			return MERGED_SCANCODE_Y;
		case RG35XXPLUS_SCANCODE_L1:
			return MERGED_SCANCODE_L1;
		case RG35XXPLUS_SCANCODE_R1:
			return MERGED_SCANCODE_R1;
		case RG35XXPLUS_SCANCODE_SELECT:
			return MERGED_SCANCODE_SELECT;
		case RG35XXPLUS_SCANCODE_START:
			return MERGED_SCANCODE_START;
		case RG35XXPLUS_SCANCODE_VOLUME_MINUS:
			return MERGED_SCANCODE_VOLUME_MINUS;
		case RG35XXPLUS_SCANCODE_VOLUME_PLUS:
			return MERGED_SCANCODE_VOLUME_PLUS;
		case RG35XXPLUS_SCANCODE_POWER:
			return MERGED_SCANCODE_POWER;
		default:
			break;
	}

	return scancode;
}

static int convert_to_axis_event(struct input_event * ev)
{
	switch (ev->code)
	{
		case RG35XXPLUS_SCANCODE_L2:
		case RG35XXPLUS_SCANCODE_R2:
			// Convert digital to analog triggers
			ev->type = EV_ABS;
			ev->code = ev->code == RG35XXPLUS_SCANCODE_L2 ? MERGED_AXIS_LEFT_TRIGGER : MERGED_AXIS_RIGHT_TRIGGER;
			ev->value = ev->value ? MERGED_TRIGGER_VALUE_MAX : MERGED_TRIGGER_VALUE_MIN;
			return 1;
		case RG35XXPLUS_SCANCODE_DPAD_UP:
		case RG35XXPLUS_SCANCODE_DPAD_LEFT:
		case RG35XXPLUS_SCANCODE_DPAD_RIGHT:
		case RG35XXPLUS_SCANCODE_DPAD_DOWN:
			// Convert the digital dpad input to analog values
			ev->type = EV_ABS;
			ev->value = ev->code == RG35XXPLUS_SCANCODE_DPAD_UP || ev->code == RG35XXPLUS_SCANCODE_DPAD_LEFT ? (ev->value ? -1 : 0) : (ev->value ? 1 : 0);
			ev->code = ev->code == RG35XXPLUS_SCANCODE_DPAD_UP || ev->code == RG35XXPLUS_SCANCODE_DPAD_DOWN ? MERGED_AXIS_DPAD_VERTICAL : MERGED_AXIS_DPAD_HORIZONTAL;
			return 1;
		default:
			break;
	}

	return 0;
}

static void input_event(int fd, struct input_event * ev)
{
	// We're handling a key event
	if (ev->type == EV_KEY)
	{
		// Convert the key to an axis event where necessary
		if (!convert_to_axis_event(ev))
		{
			// Translate the key scancode where necessary
			ev->code = translate_scancode(ev->code);
		}
	}

	// Trigger global hotkeys
	hotkey(ev);

	// Pass the event through to the merged gamepad
	write(fd, ev, sizeof(*ev));
}

void merge_rg35xxplus_inputs(int merged_gamepad)
{
	// Open the axp2202-pek (houses the power and volume buttons)
	int pmu_keys = open(RG35XXPLUS_PMU_KEYS, O_RDONLY | O_NONBLOCK);

	// Open the Deeplay-keys (houses everything else)
	int gamepad = open(RG35XXPLUS_GAMEPAD, O_RDONLY | O_NONBLOCK);

	// Determine the highest file descriptor (needed for select)
	int maxfd = pmu_keys > gamepad ? pmu_keys : gamepad;

	// Initialize the select structure
	fd_set rfds, wfds;
	FD_ZERO(&rfds);
	FD_SET(pmu_keys, &rfds);
	FD_SET(gamepad, &rfds);

	// The operation loop
	while(1)
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

			// A axp2202-pek event was captured
			if (FD_ISSET(pmu_keys, &wfds))
			{
				// We managed to read the event data
				if (read(pmu_keys, &ev, sizeof(ev)) == sizeof(ev))
				{
					// Pass the event through to our merged gamepad
					input_event(merged_gamepad, &ev);
				}
			}

			// A Deeplay-keys event was captured
			if (FD_ISSET(gamepad, &wfds))
			{
				// We managed to read the event data
				if (read(gamepad, &ev, sizeof(ev)) == sizeof(ev))
				{
					// Pass the event through to our merged gamepad
					input_event(merged_gamepad, &ev);
				}
			}
		}
	}

	// Close the Deeplay-keys input device
	close(gamepad);

	// Close the axp2202-pek input device
	close(pmu_keys);
}