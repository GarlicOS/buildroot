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

// The button states
#define BTN_RELEASED 0
#define BTN_PRESSED 1
#define BTN_REPEAT 2

// The RG405M input device paths
#define TOUCHSCREEN "/dev/input/event0"
#define GPIO_KEYS "/dev/input/event1"
#define GAMEPAD "/dev/input/event2"
#define GAMEPAD_TOUCHSCREEN "/dev/input/event3"
#define RUMBLE_MOTOR "/dev/input/event4"
#define HEADPHONE_JACK "/dev/input/event5"
#define HEADPHONE_KEYBOARD "/dev/input/event6"

// The gpio-keys input device scancodes
#define SCANCODE_VOLUME_MINUS 114
#define SCANCODE_VOLUME_PLUS 115
#define SCANCODE_POWER 116

// The retrogame_joypad input device scancodes
#define SCANCODE_HOME 158
#define SCANCODE_A 304
#define SCANCODE_B 305
#define SCANCODE_X 307
#define SCANCODE_Y 308
#define SCANCODE_L1 310
#define SCANCODE_R1 311
#define SCANCODE_L2 312
#define SCANCODE_R2 313
#define SCANCODE_SELECT 314
#define SCANCODE_START 315
#define SCANCODE_L3 317
#define SCANCODE_R3 318

// The retrogame_joypad axis numbers
#define AXIS_LEFT_ANALOG_HORIZONTAL 0
#define AXIS_LEFT_ANALOG_VERTICAL 1
#define AXIS_RIGHT_ANALOG_HORIZONTAL 2
#define AXIS_RIGHT_ANALOG_VERTICAL 5
#define AXIS_DPAD_HORIZONTAL 16
#define AXIS_DPAD_VERTICAL 17

// The retrogame_joypad analog stick axis range
#define ANALOG_VALUE_MIN -1800
#define ANALOG_VALUE_MAX 1800
#define ANALOG_VALUE_FUZZ 64
#define ANALOG_VALUE_FLAT 32

// The retrogame_joypad dpad axis range
#define DPAD_VALUE_MIN -1
#define DPAD_VALUE_MAX 1
#define DPAD_VALUE_FUZZ 0
#define DPAD_VALUE_FLAT 0

static int translate_scancode(int scancode)
{
	switch (scancode)
	{
		case SCANCODE_A:
			return BTN_EAST;
		case SCANCODE_B:
			return BTN_SOUTH;
		case SCANCODE_Y:
			return BTN_WEST;
		case SCANCODE_X:
			return BTN_NORTH;
		case SCANCODE_HOME:
			return BTN_MODE;
		case SCANCODE_VOLUME_MINUS:
			return BTN_C;
		case SCANCODE_VOLUME_PLUS:
			return BTN_Z;
		case SCANCODE_POWER:
			return BTN_TOP;
		default:
			break;
	}
	
	return scancode;
}

static void input_event(int fd, struct input_event * ev)
{
	// We're handling a key event
	if (ev->type == EV_KEY)
	{
		// Filter out repeat events (these occur on the power and volume keys)
		if (ev->value == BTN_REPEAT)
			return;
		
		// Translate key scancodes where necessary
		ev->code = translate_scancode(ev->code);
	}
	
	// Pass the event through to the merged gamepad
	write(fd, ev, sizeof(*ev));
}

int main(int argc, char * argv[])
{
	// Open uinput
	int merged_gamepad = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
	
	// Configure the virtual device
    struct uinput_user_dev merged_gamepad_configuration;
	memset(&merged_gamepad_configuration, 0, sizeof(merged_gamepad_configuration)); // Initialize memory
	snprintf(merged_gamepad_configuration.name, UINPUT_MAX_NAME_SIZE, "RG405M Gamepad"); // Gamepad name
	merged_gamepad_configuration.id.bustype = BUS_USB; // Connected via USB
	merged_gamepad_configuration.id.vendor  = 0x045e; // Microsoft
	merged_gamepad_configuration.id.product = 0x028e; // Xbox 360 Controller
	merged_gamepad_configuration.id.version = 1; // First version
    merged_gamepad_configuration.absmin[AXIS_LEFT_ANALOG_HORIZONTAL] = ANALOG_VALUE_MIN; // Left Analog X
    merged_gamepad_configuration.absmax[AXIS_LEFT_ANALOG_HORIZONTAL] = ANALOG_VALUE_MAX; // Left Analog X
    merged_gamepad_configuration.absfuzz[AXIS_LEFT_ANALOG_HORIZONTAL] = ANALOG_VALUE_FUZZ; // Left Analog X
    merged_gamepad_configuration.absflat[AXIS_LEFT_ANALOG_HORIZONTAL] = ANALOG_VALUE_FLAT; // Left Analog X
    merged_gamepad_configuration.absmin[AXIS_LEFT_ANALOG_VERTICAL] = ANALOG_VALUE_MIN; // Left Analog Y
    merged_gamepad_configuration.absmax[AXIS_LEFT_ANALOG_VERTICAL] = ANALOG_VALUE_MAX; // Left Analog Y
    merged_gamepad_configuration.absfuzz[AXIS_LEFT_ANALOG_VERTICAL] = ANALOG_VALUE_FUZZ; // Left Analog Y
    merged_gamepad_configuration.absflat[AXIS_LEFT_ANALOG_VERTICAL] = ANALOG_VALUE_FLAT; // Left Analog Y
    merged_gamepad_configuration.absmin[AXIS_RIGHT_ANALOG_HORIZONTAL] = ANALOG_VALUE_MIN; // Right Analog X
    merged_gamepad_configuration.absmax[AXIS_RIGHT_ANALOG_HORIZONTAL] = ANALOG_VALUE_MAX; // Right Analog X
    merged_gamepad_configuration.absfuzz[AXIS_RIGHT_ANALOG_HORIZONTAL] = ANALOG_VALUE_FUZZ; // Right Analog X
    merged_gamepad_configuration.absflat[AXIS_RIGHT_ANALOG_HORIZONTAL] = ANALOG_VALUE_FLAT; // Right Analog X
    merged_gamepad_configuration.absmin[AXIS_RIGHT_ANALOG_VERTICAL] = ANALOG_VALUE_MIN; // Right Analog Y
    merged_gamepad_configuration.absmax[AXIS_RIGHT_ANALOG_VERTICAL] = ANALOG_VALUE_MAX; // Right Analog Y
    merged_gamepad_configuration.absfuzz[AXIS_RIGHT_ANALOG_VERTICAL] = ANALOG_VALUE_FUZZ; // Right Analog Y
    merged_gamepad_configuration.absflat[AXIS_RIGHT_ANALOG_VERTICAL] = ANALOG_VALUE_FLAT; // Right Analog Y
    merged_gamepad_configuration.absmin[AXIS_DPAD_HORIZONTAL] = DPAD_VALUE_MIN; // Dpad X
    merged_gamepad_configuration.absmax[AXIS_DPAD_HORIZONTAL] = DPAD_VALUE_MAX; // Dpad X
    merged_gamepad_configuration.absfuzz[AXIS_DPAD_HORIZONTAL] = DPAD_VALUE_FUZZ; // Dpad X
    merged_gamepad_configuration.absflat[AXIS_DPAD_HORIZONTAL] = DPAD_VALUE_FLAT; // Dpad X
    merged_gamepad_configuration.absmin[AXIS_DPAD_VERTICAL] = DPAD_VALUE_MIN; // Dpad Y
    merged_gamepad_configuration.absmax[AXIS_DPAD_VERTICAL] = DPAD_VALUE_MAX; // Dpad Y
    merged_gamepad_configuration.absfuzz[AXIS_DPAD_VERTICAL] = DPAD_VALUE_FUZZ; // Dpad Y
    merged_gamepad_configuration.absflat[AXIS_DPAD_VERTICAL] = DPAD_VALUE_FLAT; // Dpad Y
	ioctl(merged_gamepad, UI_SET_EVBIT, EV_KEY); // This gamepad has buttons
	ioctl(merged_gamepad, UI_SET_EVBIT, EV_ABS); // This gamepad has axes
	ioctl(merged_gamepad, UI_SET_KEYBIT, BTN_SOUTH); // B
	ioctl(merged_gamepad, UI_SET_KEYBIT, BTN_EAST); // A
	ioctl(merged_gamepad, UI_SET_KEYBIT, BTN_NORTH); // X
	ioctl(merged_gamepad, UI_SET_KEYBIT, BTN_WEST); // Y
	ioctl(merged_gamepad, UI_SET_KEYBIT, BTN_SELECT); // SELECT
	ioctl(merged_gamepad, UI_SET_KEYBIT, BTN_START); // START
	ioctl(merged_gamepad, UI_SET_KEYBIT, BTN_THUMBL); // L3
	ioctl(merged_gamepad, UI_SET_KEYBIT, BTN_THUMBR); // R3
	ioctl(merged_gamepad, UI_SET_KEYBIT, BTN_MODE); // HOME/BACK
	ioctl(merged_gamepad, UI_SET_KEYBIT, BTN_TL); // L1
	ioctl(merged_gamepad, UI_SET_KEYBIT, BTN_TR); // R1
	ioctl(merged_gamepad, UI_SET_KEYBIT, BTN_TL2); // L2
	ioctl(merged_gamepad, UI_SET_KEYBIT, BTN_TR2); // R2
	ioctl(merged_gamepad, UI_SET_KEYBIT, BTN_C); // Volume Minus
	ioctl(merged_gamepad, UI_SET_KEYBIT, BTN_Z); // Volume Plus
	ioctl(merged_gamepad, UI_SET_KEYBIT, BTN_TOP); // Power
	ioctl(merged_gamepad, UI_SET_ABSBIT, AXIS_LEFT_ANALOG_HORIZONTAL); // Left Analog X
	ioctl(merged_gamepad, UI_SET_ABSBIT, AXIS_LEFT_ANALOG_VERTICAL); // Left Analog Y
	ioctl(merged_gamepad, UI_SET_ABSBIT, AXIS_RIGHT_ANALOG_HORIZONTAL); // Right Analog X
	ioctl(merged_gamepad, UI_SET_ABSBIT, AXIS_RIGHT_ANALOG_VERTICAL); // Right Analog Y
	ioctl(merged_gamepad, UI_SET_ABSBIT, AXIS_DPAD_HORIZONTAL); // Dpad X
	ioctl(merged_gamepad, UI_SET_ABSBIT, AXIS_DPAD_VERTICAL); // Dpad Y

	// Create the virtual device
	write(merged_gamepad, &merged_gamepad_configuration, sizeof(merged_gamepad_configuration));
	ioctl(merged_gamepad, UI_DEV_CREATE);
	
	// Open the gpio-keys (houses the power and volume buttons)
	int gpio_keys = open(GPIO_KEYS, O_RDONLY | O_NONBLOCK);
	
	// Open the retrogame_joypad (houses everything else)
	int gamepad = open(GAMEPAD, O_RDONLY | O_NONBLOCK);
	
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
	
	// Destroy our merged gamepad
	ioctl(merged_gamepad, UI_DEV_DESTROY);
	
	// Close uinput
	close(merged_gamepad);
	
	// Exit the application
	return 0;
}
