#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/input.h>
#include <linux/uinput.h>

#include "gamepad.h"
#include "rg405m.h"
#include "rg35xxplus.h"
#include "retroid_input.h"

#define AUDIO_CONTROL_PIPE "/tmp/audio_control"
#define AUDIO_RESPONSE_PIPE "/tmp/audio_response"
#define PERSISTENT_BRIGHTNESS_PATH "/root/.config/brightness"
#define PERSISTENT_VOLUME_PATH "/root/.config/volume"
#define DEFAULT_BRIGHTNESS 72

#define SUNXI_DISP_LCD_SET_BRIGHTNESS 0x102
#define SUNXI_DISP_LCD_GET_BRIGHTNESS 0x103

static int current_volume = 0;

static int get_min_brightness()
{
	// The minimum brightness
	int minimum_brightness = 0;

	// Get the minimum brightness
	const char * minimum_brightness_env = getenv("LCD_BRIGHTNESS_MINIMUM");

	// We've been given a valid minimum brightness
	if (minimum_brightness_env != NULL)
	{
		// Parse the minimum brightness
		sscanf(minimum_brightness_env, "%d", &minimum_brightness);
	}

	// Return the minimum brightness
	return minimum_brightness;
}

static int get_max_brightness()
{
	// The maximum brightness
	int maximum_brightness = 0;

	// Get the maximum brightness
	const char * maximum_brightness_env = getenv("LCD_BRIGHTNESS_MAXIMUM");

	// We've been given a valid maximum brightness
	if (maximum_brightness_env != NULL)
	{
		// We've been given a sysfs node path
		if (maximum_brightness_env[0] == '/')
		{
			// Open the maximum brightness sysfs node
			FILE * file = fopen(maximum_brightness_env, "r");

			// We managed to open the brightness sysfs node
			if (file != NULL)
			{
				// Read the current brightness
				fscanf(file, "%d", &maximum_brightness);

				// Close the brightness sysfs node
				fclose(file);
			}
		}

		// We've been given a number
		else
		{
			// Parse the maximum brightness
			sscanf(maximum_brightness_env, "%d", &maximum_brightness);
		}
	}

	// Return the maximum brightness
	return maximum_brightness;
}

static int get_sunxi_brightness()
{
	// The current brightness
	int current_brightness = -1;

	// Open the sunxi display device
	int disp = open("/dev/disp", O_RDWR);

	// We managed to open the sunxi display device
	if (disp >= 0)
	{
		// The argument buffer
		unsigned long arg[3];

		// Initialize the argument buffer
		memset(arg, 0, sizeof(arg));

		// Set the display number (only here for documentation's sake)
		arg[0] = 0;

		// Get the current brightness
		current_brightness = ioctl(disp, SUNXI_DISP_LCD_GET_BRIGHTNESS, (void*)arg);

		// Close the sunxi display device
		close(disp);
	}

	// Return the current brightness
	return current_brightness;
}

static int get_brightness()
{
	// The current brightness
	int current_brightness = DEFAULT_BRIGHTNESS;

	// Get the brightness path
	const char * brightness_path_env = getenv("LCD_BRIGHTNESS_PATH");

	// We've been given a valid brightness path
	if (brightness_path_env != NULL)
	{
		// We're handling a sunxi device
		if (strcmp(brightness_path_env, "/dev/disp") == 0)
		{
			// Get the brightness
			current_brightness = get_sunxi_brightness();
		}

		// We're handling a regular device
		else
		{
			// Open the brightness sysfs node
			FILE * file = fopen(brightness_path_env, "r");

			// We managed to open the brightness sysfs node
			if (file != NULL)
			{
				// Read the current brightness
				fscanf(file, "%d", &current_brightness);

				// Close the brightness sysfs node
				fclose(file);
			}
		}
	}

	// Return the current brightness
	return current_brightness;
}

static int get_persistent_brightness()
{
	// The persistent brightness
	int persistent_brightness = DEFAULT_BRIGHTNESS;

	// Open the persistent brightness file
	FILE * file = fopen(PERSISTENT_BRIGHTNESS_PATH, "r");

	// We managed to open the persistent brightness file
	if (file != NULL)
	{
		// Read the persistent brightness
		fscanf(file, "%d", &persistent_brightness);
		
		// Close the persistent brightness file
		fclose(file);
	}

	// Return the persistent brightness
	return persistent_brightness;
}

static void set_sunxi_brightness(int value)
{
	// Open the sunxi display device
	int disp = open("/dev/disp", O_RDWR);

	// We managed to open the sunxi display device
	if (disp >= 0)
	{
		// The argument buffer
		unsigned long arg[3];

		// Initialize the argument buffer
		memset(arg, 0, sizeof(arg));

		// Set the display number (only here for documentation's sake)
		arg[0] = 0;

		// Set the brightness
		arg[1] = value;

		// Write it to the sunxi display device
		ioctl(disp, SUNXI_DISP_LCD_SET_BRIGHTNESS, (void*)arg);

		// Close the sunxi display device
		close(disp);
	}
}

static void set_brightness(int value)
{
	// Get the brightness path
	const char * brightness_path_env = getenv("LCD_BRIGHTNESS_PATH");

	// We've been given a valid brightness path
	if (brightness_path_env != NULL)
	{
		// We're handling a sunxi device
		if (strcmp(brightness_path_env, "/dev/disp") == 0)
		{
			// Set the brightness
			set_sunxi_brightness(value);
		}

		// We're handling a regular device
		else
		{
			// Open the brightness sysfs node
			FILE * file = fopen(brightness_path_env, "w");

			// We managed to open the brightness sysfs node
			if (file != NULL)
			{
				// Set the current brightness
				fprintf(file, "%d", value);

				// Close the brightness sysfs node
				fclose(file);
			}
		}

		// Open the persistent brightness file
		FILE * persistent_file = fopen(PERSISTENT_BRIGHTNESS_PATH, "w");

		// We managed to open the persistent brightness file
		if (persistent_file != NULL)
		{
			// Set the current brightness
			fprintf(persistent_file, "%d", value);

			// Close the persistent brightness file
			fclose(persistent_file);
		}
	}
}

static void change_brightness(int direction)
{
	// Get the current, minimum and maximum brightness
	int brightness = (int)get_brightness();
	int min_brightness = (int)get_min_brightness();
	int max_brightness = (int)get_max_brightness();

	// Adjust the brightness
	brightness += direction;

	// We're below the minimum brightness
	if (brightness < min_brightness)
	{
		// Cap to the minimum brightness
		brightness = min_brightness;
	}

	// We're above the maximum brightness
	if (brightness > max_brightness)
	{
		// Cap to the maximum brightness
		brightness = max_brightness;
	}

	// Set the new brightness
	set_brightness(brightness);
}

static int get_persistent_volume()
{
	// The persistent volume
	int persistent_volume = 0;

	// Open the persistent volume file
	FILE * file = fopen(PERSISTENT_VOLUME_PATH, "r");

	// We managed to open the persistent volume file
	if (file != NULL)
	{
		// Read the persistent volume
		fscanf(file, "%d", &persistent_volume);

		// Close the persistent volume file
		fclose(file);
	}

	// Return the persistent volume
	return persistent_volume;
}

static void set_persistent_volume(int volume)
{
	// Open the persistent volume file
	FILE * file = fopen(PERSISTENT_VOLUME_PATH, "w");

	// We managed to open the persistent volume file
	if (file != NULL)
	{
		// Set the current volume
		fprintf(file, "%d", volume);

		// Close the persistent volume file
		fclose(file);
	}
}

size_t read_bytes(int fd, void * buffer, size_t count)
{
	// The total number of bytes read
	size_t totalBytesRead = 0;

	// Keep reading data until we got enough data
	while (totalBytesRead < count)
	{
		// Read the remaining number of bytes
		int bytesRead = read(fd, buffer + totalBytesRead, count - totalBytesRead);

		// We have read at least one byte
		if (bytesRead > 0)
		{
			// Accumulate the read bytes
			totalBytesRead += bytesRead;
		}
	}

	// Return the read bytes
	return totalBytesRead;
}

static int change_volume(int direction)
{
	// The result
	int result = 0;

	// The audio provider control pipe
	static int control_fd = -1;

	// The audio provider response pipe
	static int response_fd = -1;

	// We haven't created the audio provider control pipe yet
	while (control_fd < 0)
	{
		// Create the named pipe
		mkfifo(AUDIO_CONTROL_PIPE, 0666);

		// Open the named pipe for writing
		control_fd = open(AUDIO_CONTROL_PIPE, O_WRONLY | O_NONBLOCK);
	}

	// We haven't created the audio provider response pipe yet
	while (response_fd < 0)
	{
		// Create the named pipe
		mkfifo(AUDIO_RESPONSE_PIPE, 0666);

		// Open the named pipe for reading
		response_fd = open(AUDIO_RESPONSE_PIPE, O_RDONLY | O_NONBLOCK);
	}

	// Write the message to the control pipe
	int write_result = write(control_fd, &direction, sizeof(direction));

	// Read the result from the response pipe
	read_bytes(response_fd, &result, sizeof(result));

	// We changed the volume successfully
	if (result)
	{
		// Keep track of the accumulated volume shift
		current_volume += direction;

		// Update the persistent volume file
		set_persistent_volume(current_volume);
	}

	// Return the result
	return result;
}

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

void hotkey(struct input_event * ev)
{
	// The hotkey related button states
	static int modifier = 0;
	static int volume_down = 0;
	static int volume_up = 0;

	// We got a key event
	if (ev->type == EV_KEY)
	{
		// The pressed key
		int * key = NULL;

		// Differentiate keys
		switch (ev->code)
		{
			// The modifier key
			case MERGED_SCANCODE_HOME:
			{
				// Get the state pointer
				key = &modifier;

				// Break out
				break;
			}

			case MERGED_SCANCODE_VOLUME_MINUS:
			{
				// Get the state pointer
				key = &volume_down;

				// Break out
				break;
			}

			case MERGED_SCANCODE_VOLUME_PLUS:
			{
				// Get the state pointer
				key = &volume_up;

				// Break out
				break;
			}
		}

		// A hotkey-related key was detected
		if (key != NULL)
		{
			// Determine the pressed state
			*key = ev->value == BTN_PRESSED || ev->value == BTN_REPEAT;

			// The volume / brightness direction
			int direction = 0;

			// Volume up is pressed
			if (volume_up)
			{
				// Steer into the positive
				direction++;
			}

			// Volume down is pressed
			if (volume_down)
			{
				// Steer into the negative
				direction--;
			}

			// The direction value isn't centered
			if (direction != 0)
			{
				// The modifier is pressed
				if (modifier)
				{
					// Change the brightness
					change_brightness(direction);
				}

				// The modifier isn't pressed
				else
				{
					// Change the volume
					change_volume(direction);
				}
			}
		}
	}
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

	// We're handling the Anbernic RG35XX+
	else if (strcmp(code_name, "rg35xx+") == 0)
	{
		// Wrap gpio-keys
		merge_rg35xxplus_inputs(merged_gamepad);
	}

	// We're handling the Retroid Pocket 2+
	else if (strcmp(code_name, "pocket2plus") == 0)
	{
		// Convert Retroid serial console controller events and combine them with gpio-keys
		merge_retroid_input_inputs(merged_gamepad);
	}

	// We're handling the Retroid Pocket 3 or Retroid Pocket 3+
	else if (strcmp(code_name, "pocket3") == 0 || strcmp(code_name, "pocket3plus") == 0)
	{
		// Convert Retroid serial console controller events and combine them with gpio-keys
		// merge_retroid_input_inputs(merged_gamepad);
		// Disabled for now until it is tested on RP3+
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

	// Restore the brightness
	set_brightness(get_persistent_brightness());

	// Restore the volume
	int persistent_volume = get_persistent_volume();
	for (int i = 0; i < abs(persistent_volume); i++)
	{
		change_volume(persistent_volume < 0 ? -1 : 1);
	}

	// Merge the gamepad input devices
	merge_inputs(merged_gamepad);

	// Destroy our merged gamepad
	ioctl(merged_gamepad, UI_DEV_DESTROY);

	// Close uinput
	close(merged_gamepad);

	// Exit the application
	return 0;
}