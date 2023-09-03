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
#include <pthread.h>

#include "gamepad.h"
#include "pocket2plus.h"
#include "retroid_input.h"

static int32_t get_pocket2plus_axis_min_value(int axis)
{
	switch (axis)
	{
		case POCKET2PLUS_AXIS_LEFT_ANALOG_HORIZONTAL:
		case POCKET2PLUS_AXIS_LEFT_ANALOG_VERTICAL:
		case POCKET2PLUS_AXIS_RIGHT_ANALOG_HORIZONTAL:
		case POCKET2PLUS_AXIS_RIGHT_ANALOG_VERTICAL:
			return POCKET2PLUS_ANALOG_VALUE_MIN;
		default:
			break;
	}

	return -1;
}

static int32_t get_pocket2plus_axis_max_value(int axis)
{
	switch (axis)
	{
		case POCKET2PLUS_AXIS_LEFT_ANALOG_HORIZONTAL:
		case POCKET2PLUS_AXIS_LEFT_ANALOG_VERTICAL:
		case POCKET2PLUS_AXIS_RIGHT_ANALOG_HORIZONTAL:
		case POCKET2PLUS_AXIS_RIGHT_ANALOG_VERTICAL:
			return POCKET2PLUS_ANALOG_VALUE_MAX;
		default:
			break;
	}

	return 1;
}

static int translate_scancode(int scancode)
{
	switch (scancode)
	{
		case POCKET2PLUS_SCANCODE_HOME:
			return MERGED_SCANCODE_HOME;
		case POCKET2PLUS_SCANCODE_A:
			return MERGED_SCANCODE_B;
		case POCKET2PLUS_SCANCODE_B:
			return MERGED_SCANCODE_A;
		case POCKET2PLUS_SCANCODE_Y:
			return MERGED_SCANCODE_X;
		case POCKET2PLUS_SCANCODE_X:
			return MERGED_SCANCODE_Y;
		case POCKET2PLUS_SCANCODE_L1:
			return MERGED_SCANCODE_L1;
		case POCKET2PLUS_SCANCODE_R1:
			return MERGED_SCANCODE_R1;
		case POCKET2PLUS_SCANCODE_SELECT:
			return MERGED_SCANCODE_SELECT;
		case POCKET2PLUS_SCANCODE_START:
			return MERGED_SCANCODE_START;
		case POCKET2PLUS_SCANCODE_L3:
			return MERGED_SCANCODE_L3;
		case POCKET2PLUS_SCANCODE_R3:
			return MERGED_SCANCODE_R3;
		case POCKET2PLUS_SCANCODE_VOLUME_MINUS:
			return MERGED_SCANCODE_VOLUME_MINUS;
		case POCKET2PLUS_SCANCODE_VOLUME_PLUS:
			return MERGED_SCANCODE_VOLUME_PLUS;
		case POCKET2PLUS_SCANCODE_POWER:
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
		case POCKET2PLUS_AXIS_LEFT_ANALOG_HORIZONTAL:
			return MERGED_AXIS_LEFT_ANALOG_HORIZONTAL;
		case POCKET2PLUS_AXIS_LEFT_ANALOG_VERTICAL:
			return MERGED_AXIS_LEFT_ANALOG_VERTICAL;
		case POCKET2PLUS_AXIS_RIGHT_ANALOG_HORIZONTAL:
			return MERGED_AXIS_RIGHT_ANALOG_HORIZONTAL;
		case POCKET2PLUS_AXIS_RIGHT_ANALOG_VERTICAL:
			return MERGED_AXIS_RIGHT_ANALOG_VERTICAL;
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

	// Multiply scaled value by -1 (Retroid analog axes are in the opposite direction of merged_gamepad)
	scaled_value = scaled_value * -1;

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
	int32_t new_value = scale_axis_value(ev->value, get_pocket2plus_axis_min_value(ev->code), get_pocket2plus_axis_max_value(ev->code), get_merged_axis_min_value(new_code), get_merged_axis_max_value(new_code));

	// Replace the old values
	ev->code = new_code;
	ev->value = new_value;
}

static int convert_to_axis_event(struct input_event * ev)
{
	switch (ev->code)
	{
		case POCKET2PLUS_SCANCODE_L2:
		case POCKET2PLUS_SCANCODE_R2:
			// Convert digital to analog triggers
			ev->type = EV_ABS;
			ev->code = ev->code == POCKET2PLUS_SCANCODE_L2 ? MERGED_AXIS_LEFT_TRIGGER : MERGED_AXIS_RIGHT_TRIGGER;
			ev->value = ev->value ? MERGED_TRIGGER_VALUE_MAX : MERGED_TRIGGER_VALUE_MIN;
			return 1;
		case POCKET2PLUS_SCANCODE_DPAD_UP:
			// Convert dpad to analog axis
			ev->type = EV_ABS;
			ev->code = MERGED_AXIS_DPAD_VERTICAL;
			ev->value = ev->value ? MERGED_DPAD_VALUE_MIN : 0;
			return 1;
		case POCKET2PLUS_SCANCODE_DPAD_DOWN:
			// Convert dpad to analog axis
			ev->type = EV_ABS;
			ev->code = MERGED_AXIS_DPAD_VERTICAL;
			ev->value = ev->value ? MERGED_DPAD_VALUE_MAX : 0;
			return 1;
		case POCKET2PLUS_SCANCODE_DPAD_LEFT:
			// Convert dpad to analog axis
			ev->type = EV_ABS;
			ev->code = MERGED_AXIS_DPAD_HORIZONTAL;
			ev->value = ev->value ? MERGED_DPAD_VALUE_MIN : 0;
			return 1;
		case POCKET2PLUS_SCANCODE_DPAD_RIGHT:
			// Convert dpad to analog axis
			ev->type = EV_ABS;
			ev->code = MERGED_AXIS_DPAD_HORIZONTAL;
			ev->value = ev->value ? MERGED_DPAD_VALUE_MAX : 0;
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

// Struct to send to threads
typedef struct _volumekey_event {
	int merged_gamepad;
	int volume_pressed;
	int volume_keycode;
	int volume_thread_running;
} volumekey_event;

// Function to simulate volume key repeat by waiting one second before going into a loop of sending repeat events every 200ms
void *simulate_volumekey_repeat_pocket2plus(void *volumekey_struct)
{
	// Update thread status to show thread is running
	((volumekey_event*)volumekey_struct)->volume_thread_running = 1;
	// Wait a second before starting to repeat
	sleep(1);
	// While volume key pressed returns true(1), send repeat volume key events
	while(((volumekey_event*)volumekey_struct)->volume_pressed == 1)
	{
		// Make volume key repeat event
		struct input_event repeat_ev;
		repeat_ev.type = EV_KEY;
		repeat_ev.code = ((volumekey_event*)volumekey_struct)->volume_keycode;
		repeat_ev.value = BTN_REPEAT;
		// Send volume key repeat event
		input_event(((volumekey_event*)volumekey_struct)->merged_gamepad, &repeat_ev);
		// Wait 200ms
		usleep(200000);
	}
	// Update thread status to indicate the thread is stopping
	((volumekey_event*)volumekey_struct)->volume_thread_running = 0;
	return NULL;
}

void merge_pocket2plus_inputs(int merged_gamepad)
{
	// Open the gpio-keys (houses the power and volume buttons)
	int gpio_keys = open(POCKET2PLUS_GPIO_KEYS, O_RDONLY | O_NONBLOCK);

	// Variables for volume button status
	int volumeup_pressed = 0;
	int volumedown_pressed = 0;
	pthread_t volumeup_thread, volumedown_thread;
	volumekey_event volumeup_struct;
	volumeup_struct.merged_gamepad = merged_gamepad;
	volumeup_struct.volume_keycode = KEY_VOLUMEUP;
	volumeup_struct.volume_thread_running = 0;
	volumekey_event volumedown_struct;
	volumedown_struct.merged_gamepad = merged_gamepad;
	volumedown_struct.volume_keycode = KEY_VOLUMEDOWN;
	volumedown_struct.volume_thread_running = 0;

	// Open the Retroid serial port for gamepad input
	int gamepad = open(POCKET2PLUS_GAMEPAD, 0x102);

	// Initialize Retroid serial port and enable analog stick support
	init_uart(gamepad);
	enable_analog_sticks(gamepad);

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
					// Simulate volume key repeating
					if (ev.code == KEY_VOLUMEUP)
					{
						volumeup_pressed = ev.value;
						volumeup_struct.volume_pressed = volumeup_pressed;
						// Run thread if volume up key is pressed and thread does not already exist
						if (volumeup_pressed == 1 && volumeup_struct.volume_thread_running == 0)
						{
							pthread_create(&volumeup_thread, NULL, &simulate_volumekey_repeat_pocket2plus, (void *)&volumeup_struct);
						}
					}
					if (ev.code == KEY_VOLUMEDOWN)
					{
						volumedown_pressed = ev.value;
						volumedown_struct.volume_pressed = volumedown_pressed;
						// Run thread if volume down key is pressed and thread does not already exist
						if (volumedown_pressed == 1 && volumedown_struct.volume_thread_running == 0)
						{
							pthread_create(&volumedown_thread, NULL, &simulate_volumekey_repeat_pocket2plus, (void *)&volumedown_struct);
						}
					}
					// Pass the event through to our merged gamepad
					input_event(merged_gamepad, &ev);
				}
			}

			// A Retroid serial gamepad event was captured
			if (FD_ISSET(gamepad, &wfds))
			{
				// Read the input event
				unsigned char buffer[20];
				int readlen;
				readlen = read(gamepad, buffer, sizeof(buffer));

				// Check to see if we read 20 bytes from Retroid's serial port
				// A proper serial input event should be 20 bytes long
				if (readlen > 0 && readlen < 20)
				{
					printf("Error: read length is %d. Something went wrong.\n", readlen);
				}

				// Convert proper Retroid input events to a normal input event, then send to merged gamepad
				if (readlen == 8)
				{
					// Create a fake event for the Retroid controller
					struct input_event uart_ev;
					// Pull event type from the serial event
					uart_ev.type = buffer[0];
					// Pull keycode and pressed status from read serial line
					uart_ev.code = buffer[3];
					// Concatenate the last two bytes of hex values (this is the full input value for analog events)
					uart_ev.value = (buffer[6]<<8) | (buffer[7]);
					// Pass the event through to our merged gamepad
					input_event(merged_gamepad, &uart_ev);
					// Send a sync event, otherwise input won't register
					uart_ev.type = EV_SYN;
					uart_ev.code = 0;
					uart_ev.value = 0;
					input_event(merged_gamepad, &uart_ev);
				}

				// Convert proper Retroid input events to a normal input event, then send to merged gamepad
				if (readlen == 20)
				{
					// Create a fake event for the Retroid controller
					struct input_event uart_ev;
					// Pull event type from the serial event
					uart_ev.type = buffer[12];
					// Pull keycode and pressed status from read serial line
					uart_ev.code = buffer[15];
					// Concatenate the last two bytes of hex values (this is the full input value for analog events)
					uart_ev.value = (buffer[18]<<8) | (buffer[19]);
					// Pass the event through to our merged gamepad
					input_event(merged_gamepad, &uart_ev);
					// Send a sync event, otherwise input won't register
					uart_ev.type = EV_SYN;
					uart_ev.code = 0;
					uart_ev.value = 0;
					input_event(merged_gamepad, &uart_ev);
				}
			}
		}
	}

	// Close the Retroid serial input device
	close(gamepad);

	// Close the gpio-keys input device
	close(gpio_keys);
}