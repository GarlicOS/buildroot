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
#include "rg405m.h"

static int32_t get_rg405m_axis_min_value(int axis)
{
	switch (axis)
	{
		case RG405M_AXIS_LEFT_ANALOG_HORIZONTAL:
		case RG405M_AXIS_LEFT_ANALOG_VERTICAL:
		case RG405M_AXIS_RIGHT_ANALOG_HORIZONTAL:
		case RG405M_AXIS_RIGHT_ANALOG_VERTICAL:
			return RG405M_ANALOG_VALUE_MIN;
		case RG405M_AXIS_DPAD_HORIZONTAL:
		case RG405M_AXIS_DPAD_VERTICAL:
			return RG405M_DPAD_VALUE_MIN;
		default:
			break;
	}

	return -1;
}

static int32_t get_rg405m_axis_max_value(int axis)
{
	switch (axis)
	{
		case RG405M_AXIS_LEFT_ANALOG_HORIZONTAL:
		case RG405M_AXIS_LEFT_ANALOG_VERTICAL:
		case RG405M_AXIS_RIGHT_ANALOG_HORIZONTAL:
		case RG405M_AXIS_RIGHT_ANALOG_VERTICAL:
			return RG405M_ANALOG_VALUE_MAX;
		case RG405M_AXIS_DPAD_HORIZONTAL:
		case RG405M_AXIS_DPAD_VERTICAL:
			return RG405M_DPAD_VALUE_MAX;
		default:
			break;
	}

	return 1;
}

static int translate_scancode(int scancode)
{
	switch (scancode)
	{
		case RG405M_SCANCODE_HOME:
			return MERGED_SCANCODE_HOME;
		case RG405M_SCANCODE_A:
			return MERGED_SCANCODE_B;
		case RG405M_SCANCODE_B:
			return MERGED_SCANCODE_A;
		case RG405M_SCANCODE_Y:
			return MERGED_SCANCODE_X;
		case RG405M_SCANCODE_X:
			return MERGED_SCANCODE_Y;
		case RG405M_SCANCODE_L1:
			return MERGED_SCANCODE_L1;
		case RG405M_SCANCODE_R1:
			return MERGED_SCANCODE_R1;
		case RG405M_SCANCODE_SELECT:
			return MERGED_SCANCODE_SELECT;
		case RG405M_SCANCODE_START:
			return MERGED_SCANCODE_START;
		case RG405M_SCANCODE_L3:
			return MERGED_SCANCODE_L3;
		case RG405M_SCANCODE_R3:
			return MERGED_SCANCODE_R3;
		case RG405M_SCANCODE_VOLUME_MINUS:
			return MERGED_SCANCODE_VOLUME_MINUS;
		case RG405M_SCANCODE_VOLUME_PLUS:
			return MERGED_SCANCODE_VOLUME_PLUS;
		case RG405M_SCANCODE_POWER:
			return MERGED_SCANCODE_POWER;
		default:
			break;
	}

	return scancode;
}

static int translate_axiscode(int axis)
{
	switch (axis)
	{
		case RG405M_AXIS_LEFT_ANALOG_HORIZONTAL:
			return MERGED_AXIS_LEFT_ANALOG_HORIZONTAL;
		case RG405M_AXIS_LEFT_ANALOG_VERTICAL:
			return MERGED_AXIS_LEFT_ANALOG_VERTICAL;
		case RG405M_AXIS_RIGHT_ANALOG_HORIZONTAL:
			return MERGED_AXIS_RIGHT_ANALOG_HORIZONTAL;
		case RG405M_AXIS_RIGHT_ANALOG_VERTICAL:
			return MERGED_AXIS_RIGHT_ANALOG_VERTICAL;
		case RG405M_AXIS_DPAD_HORIZONTAL:
			return MERGED_AXIS_DPAD_HORIZONTAL;
		case RG405M_AXIS_DPAD_VERTICAL:
			return MERGED_AXIS_DPAD_VERTICAL;
		default:
			break;
	}

	return axis;
}

static int32_t scale_axis_value(int32_t value, int32_t input_min_value, int32_t input_max_value, int32_t output_min_value, int32_t output_max_value)
{
	// Calculate the input range and output range
	int32_t input_range = input_max_value - input_min_value;
	int32_t output_range = output_max_value - output_min_value;

	// Scale the input value to the output range
	int32_t scaled_value = ((value - input_min_value) * output_range / input_range) + output_min_value;

	// We're undershooting the minimum output value
	if (scaled_value < output_min_value)
	{
		// Force the value into the bounds
		scaled_value = output_min_value;
	}

	// We're overshooting the maximum output value
	else if (scaled_value > output_max_value)
	{
		// Force the value into the bounds
		scaled_value = output_max_value;
	}

	// Return the scaled value
	return scaled_value;
}

static void scale_and_translate_axis(struct input_event * ev)
{
	// Translate the axis code
	uint16_t new_code = translate_axiscode(ev->code);

	// Scale range
	int32_t new_value = scale_axis_value(ev->value, get_rg405m_axis_min_value(ev->code), get_rg405m_axis_max_value(ev->code), get_merged_axis_min_value(new_code), get_merged_axis_max_value(new_code));

	// Replace the old values
	ev->code = new_code;
	ev->value = new_value;
}

static int convert_to_axis_event(struct input_event * ev)
{
	switch (ev->code)
	{
		case RG405M_SCANCODE_L2:
		case RG405M_SCANCODE_R2:
			// Convert digital to analog triggers
			ev->type = EV_ABS;
			ev->code = ev->code == RG405M_SCANCODE_L2 ? MERGED_AXIS_LEFT_TRIGGER : MERGED_AXIS_RIGHT_TRIGGER;
			ev->value = ev->value ? MERGED_TRIGGER_VALUE_MAX : MERGED_TRIGGER_VALUE_MIN;
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

	// We're handling a axis event
	else if (ev->type == EV_ABS)
	{
		// Convert the axis where necessary
		scale_and_translate_axis(ev);
	}

	// Trigger global hotkeys
	hotkey(ev);

	// Filter out repeat events (these occur on the RG405M_GPIO_KEYS input device)
	if (ev->value != BTN_REPEAT)
	{
		// Pass the event through to the merged gamepad
		write(fd, ev, sizeof(*ev));
	}
}

void merge_rg405m_inputs(int merged_gamepad)
{
	// Open the gpio-keys (houses the power and volume buttons)
	int gpio_keys = open(RG405M_GPIO_KEYS, O_RDONLY | O_NONBLOCK);

	// Open the retrogame_joypad (houses everything else)
	int gamepad = open(RG405M_GAMEPAD, O_RDONLY | O_NONBLOCK);

	// Determine the highest file descriptor (needed for select)
	int maxfd = gpio_keys > gamepad ? gpio_keys : gamepad;

	// Initialize the select structure
	fd_set rfds, wfds;
	FD_ZERO(&rfds);
	FD_SET(gpio_keys, &rfds);
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

			// A gpio-keys event was captured
			if (FD_ISSET(gpio_keys, &wfds))
			{
				// We managed to read the event data
				if (read(gpio_keys, &ev, sizeof(ev)) == sizeof(ev))
				{
					// Pass the event through to our merged gamepad
					input_event(merged_gamepad, &ev);
				}
			}

			// A retrogame_joypad event was captured
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

	// Close the retrogame_joypad input device
	close(gamepad);

	// Close the gpio-keys input device
	close(gpio_keys);
}