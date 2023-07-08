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
#include "pocket2plus.h"

int32_t get_merged_axis_min_value(int axis)
{
	switch (axis)
	{
		case MERGED_AXIS_LEFT_ANALOG_HORIZONTAL:
		case MERGED_AXIS_LEFT_ANALOG_VERTICAL:
		case MERGED_AXIS_RIGHT_ANALOG_HORIZONTAL:
		case MERGED_AXIS_RIGHT_ANALOG_VERTICAL:
			return MERGED_ANALOG_VALUE_MIN;
		case MERGED_AXIS_DPAD_HORIZONTAL:
		case MERGED_AXIS_DPAD_VERTICAL:
			return MERGED_DPAD_VALUE_MIN;
		case MERGED_AXIS_LEFT_TRIGGER:
		case MERGED_AXIS_RIGHT_TRIGGER:
			return MERGED_TRIGGER_VALUE_MIN;
		default:
			break;
	}
	
	return -1;
}

int32_t get_merged_axis_max_value(int axis)
{
	switch (axis)
	{
		case MERGED_AXIS_LEFT_ANALOG_HORIZONTAL:
		case MERGED_AXIS_LEFT_ANALOG_VERTICAL:
		case MERGED_AXIS_RIGHT_ANALOG_HORIZONTAL:
		case MERGED_AXIS_RIGHT_ANALOG_VERTICAL:
			return MERGED_ANALOG_VALUE_MAX;
		case MERGED_AXIS_DPAD_HORIZONTAL:
		case MERGED_AXIS_DPAD_VERTICAL:
			return MERGED_DPAD_VALUE_MAX;
		case MERGED_AXIS_LEFT_TRIGGER:
		case MERGED_AXIS_RIGHT_TRIGGER:
			return MERGED_TRIGGER_VALUE_MAX;
		default:
			break;
	}
	
	return 1;
}

void merge_inputs(int merged_gamepad)
{
	// Get the device codename
	char * code_name = getenv("DEVICE_CODENAME");
	
	// The bootloader didn't set the codename properly
	if (code_name == NULL)
	{
		// Which means we can't merge the gamepads
		return;
	}
	
	// We're handling the Anbernic RG405M
	if (strcmp(code_name, "rg405m") == 0)
	{
		// Merge gpio-keys & retrogame_joypad
		merge_rg405m_inputs(merged_gamepad);
	}

	// We're handling the Retroid Pocket 2+
	if (strcmp(code_name, "pocket2plus") == 0)
	{
		// Convert Retroid serial console controller events and combine them with gpio-keys
		merge_pocket2plus_inputs(merged_gamepad);
	}
}

int main(int argc, char * argv[])
{
	// Open uinput
	int merged_gamepad = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
	
	// Configure the virtual device
    struct uinput_user_dev merged_gamepad_configuration;
	memset(&merged_gamepad_configuration, 0, sizeof(merged_gamepad_configuration)); // Initialize memory
	snprintf(merged_gamepad_configuration.name, UINPUT_MAX_NAME_SIZE, "Internal Gamepad"); // Gamepad name
	merged_gamepad_configuration.id.bustype = BUS_USB; // Connected via USB
	merged_gamepad_configuration.id.vendor  = 0x045e; // Microsoft
	merged_gamepad_configuration.id.product = 0x028e; // Xbox 360 Controller
	merged_gamepad_configuration.id.version = 1; // First version
    merged_gamepad_configuration.absmin[MERGED_AXIS_LEFT_ANALOG_HORIZONTAL] = MERGED_ANALOG_VALUE_MIN; // Left Analog X
    merged_gamepad_configuration.absmax[MERGED_AXIS_LEFT_ANALOG_HORIZONTAL] = MERGED_ANALOG_VALUE_MAX; // Left Analog X
    merged_gamepad_configuration.absfuzz[MERGED_AXIS_LEFT_ANALOG_HORIZONTAL] = MERGED_ANALOG_VALUE_FUZZ; // Left Analog X
    merged_gamepad_configuration.absflat[MERGED_AXIS_LEFT_ANALOG_HORIZONTAL] = MERGED_ANALOG_VALUE_FLAT; // Left Analog X
    merged_gamepad_configuration.absmin[MERGED_AXIS_LEFT_ANALOG_VERTICAL] = MERGED_ANALOG_VALUE_MIN; // Left Analog Y
    merged_gamepad_configuration.absmax[MERGED_AXIS_LEFT_ANALOG_VERTICAL] = MERGED_ANALOG_VALUE_MAX; // Left Analog Y
    merged_gamepad_configuration.absfuzz[MERGED_AXIS_LEFT_ANALOG_VERTICAL] = MERGED_ANALOG_VALUE_FUZZ; // Left Analog Y
    merged_gamepad_configuration.absflat[MERGED_AXIS_LEFT_ANALOG_VERTICAL] = MERGED_ANALOG_VALUE_FLAT; // Left Analog Y
    merged_gamepad_configuration.absmin[MERGED_AXIS_LEFT_TRIGGER] = MERGED_TRIGGER_VALUE_MIN; // Left Trigger
    merged_gamepad_configuration.absmax[MERGED_AXIS_LEFT_TRIGGER] = MERGED_TRIGGER_VALUE_MAX; // Left Trigger
    merged_gamepad_configuration.absfuzz[MERGED_AXIS_LEFT_TRIGGER] = MERGED_TRIGGER_VALUE_FUZZ; // Left Trigger
    merged_gamepad_configuration.absflat[MERGED_AXIS_LEFT_TRIGGER] = MERGED_TRIGGER_VALUE_FLAT; // Left Trigger
    merged_gamepad_configuration.absmin[MERGED_AXIS_RIGHT_ANALOG_HORIZONTAL] = MERGED_ANALOG_VALUE_MIN; // Right Analog X
    merged_gamepad_configuration.absmax[MERGED_AXIS_RIGHT_ANALOG_HORIZONTAL] = MERGED_ANALOG_VALUE_MAX; // Right Analog X
    merged_gamepad_configuration.absfuzz[MERGED_AXIS_RIGHT_ANALOG_HORIZONTAL] = MERGED_ANALOG_VALUE_FUZZ; // Right Analog X
    merged_gamepad_configuration.absflat[MERGED_AXIS_RIGHT_ANALOG_HORIZONTAL] = MERGED_ANALOG_VALUE_FLAT; // Right Analog X
    merged_gamepad_configuration.absmin[MERGED_AXIS_RIGHT_ANALOG_VERTICAL] = MERGED_ANALOG_VALUE_MIN; // Right Analog Y
    merged_gamepad_configuration.absmax[MERGED_AXIS_RIGHT_ANALOG_VERTICAL] = MERGED_ANALOG_VALUE_MAX; // Right Analog Y
    merged_gamepad_configuration.absfuzz[MERGED_AXIS_RIGHT_ANALOG_VERTICAL] = MERGED_ANALOG_VALUE_FUZZ; // Right Analog Y
    merged_gamepad_configuration.absflat[MERGED_AXIS_RIGHT_ANALOG_VERTICAL] = MERGED_ANALOG_VALUE_FLAT; // Right Analog Y
    merged_gamepad_configuration.absmin[MERGED_AXIS_RIGHT_TRIGGER] = MERGED_TRIGGER_VALUE_MIN; // Right Trigger
    merged_gamepad_configuration.absmax[MERGED_AXIS_RIGHT_TRIGGER] = MERGED_TRIGGER_VALUE_MAX; // Right Trigger
    merged_gamepad_configuration.absfuzz[MERGED_AXIS_RIGHT_TRIGGER] = MERGED_TRIGGER_VALUE_FUZZ; // Right Trigger
    merged_gamepad_configuration.absflat[MERGED_AXIS_RIGHT_TRIGGER] = MERGED_TRIGGER_VALUE_FLAT; // Right Trigger
    merged_gamepad_configuration.absmin[MERGED_AXIS_DPAD_HORIZONTAL] = MERGED_DPAD_VALUE_MIN; // Dpad X
    merged_gamepad_configuration.absmax[MERGED_AXIS_DPAD_HORIZONTAL] = MERGED_DPAD_VALUE_MAX; // Dpad X
    merged_gamepad_configuration.absfuzz[MERGED_AXIS_DPAD_HORIZONTAL] = MERGED_DPAD_VALUE_FUZZ; // Dpad X
    merged_gamepad_configuration.absflat[MERGED_AXIS_DPAD_HORIZONTAL] = MERGED_DPAD_VALUE_FLAT; // Dpad X
    merged_gamepad_configuration.absmin[MERGED_AXIS_DPAD_VERTICAL] = MERGED_DPAD_VALUE_MIN; // Dpad Y
    merged_gamepad_configuration.absmax[MERGED_AXIS_DPAD_VERTICAL] = MERGED_DPAD_VALUE_MAX; // Dpad Y
    merged_gamepad_configuration.absfuzz[MERGED_AXIS_DPAD_VERTICAL] = MERGED_DPAD_VALUE_FUZZ; // Dpad Y
    merged_gamepad_configuration.absflat[MERGED_AXIS_DPAD_VERTICAL] = MERGED_DPAD_VALUE_FLAT; // Dpad Y
	ioctl(merged_gamepad, UI_SET_EVBIT, EV_KEY); // This gamepad has buttons
	ioctl(merged_gamepad, UI_SET_EVBIT, EV_ABS); // This gamepad has axes
	ioctl(merged_gamepad, UI_SET_KEYBIT, BTN_SOUTH); // A
	ioctl(merged_gamepad, UI_SET_KEYBIT, BTN_EAST); // B
	ioctl(merged_gamepad, UI_SET_KEYBIT, BTN_NORTH); // Y
	ioctl(merged_gamepad, UI_SET_KEYBIT, BTN_WEST); // X
	ioctl(merged_gamepad, UI_SET_KEYBIT, BTN_SELECT); // SELECT
	ioctl(merged_gamepad, UI_SET_KEYBIT, BTN_START); // START
	ioctl(merged_gamepad, UI_SET_KEYBIT, BTN_THUMBL); // L3
	ioctl(merged_gamepad, UI_SET_KEYBIT, BTN_THUMBR); // R3
	ioctl(merged_gamepad, UI_SET_KEYBIT, BTN_MODE); // HOME/BACK
	ioctl(merged_gamepad, UI_SET_KEYBIT, BTN_TL); // L1
	ioctl(merged_gamepad, UI_SET_KEYBIT, BTN_TR); // R1
	ioctl(merged_gamepad, UI_SET_KEYBIT, BTN_C); // Volume Minus
	ioctl(merged_gamepad, UI_SET_KEYBIT, BTN_Z); // Volume Plus
	ioctl(merged_gamepad, UI_SET_KEYBIT, BTN_TOP); // Power
	ioctl(merged_gamepad, UI_SET_ABSBIT, MERGED_AXIS_LEFT_ANALOG_HORIZONTAL); // Left Analog X
	ioctl(merged_gamepad, UI_SET_ABSBIT, MERGED_AXIS_LEFT_ANALOG_VERTICAL); // Left Analog Y
	ioctl(merged_gamepad, UI_SET_ABSBIT, MERGED_AXIS_LEFT_TRIGGER); // Left Trigger
	ioctl(merged_gamepad, UI_SET_ABSBIT, MERGED_AXIS_RIGHT_ANALOG_HORIZONTAL); // Right Analog X
	ioctl(merged_gamepad, UI_SET_ABSBIT, MERGED_AXIS_RIGHT_ANALOG_VERTICAL); // Right Analog Y
	ioctl(merged_gamepad, UI_SET_ABSBIT, MERGED_AXIS_RIGHT_TRIGGER); // Right Trigger
	ioctl(merged_gamepad, UI_SET_ABSBIT, MERGED_AXIS_DPAD_HORIZONTAL); // Dpad X
	ioctl(merged_gamepad, UI_SET_ABSBIT, MERGED_AXIS_DPAD_VERTICAL); // Dpad Y

	// Create the virtual device
	write(merged_gamepad, &merged_gamepad_configuration, sizeof(merged_gamepad_configuration));
	ioctl(merged_gamepad, UI_DEV_CREATE);
	
	// Merge the gamepad input devices
	merge_inputs(merged_gamepad);
	
	// Destroy our merged gamepad
	ioctl(merged_gamepad, UI_DEV_DESTROY);
	
	// Close uinput
	close(merged_gamepad);
	
	// Exit the application
	return 0;
}
