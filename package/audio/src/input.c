#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <linux/input.h>
#include <linux/limits.h>

#include "input.h"

const char * get_input_device_path(const char * input_device_name)
{
	// The static device path buffer
	static char device_path[PATH_MAX];

	// The input device directory
	const char * input_device_directory = "/dev/input";

	// Open the input device directory
	DIR * dir = opendir(input_device_directory);

	// We failed to open the input device directory
	if (dir == NULL)
	{
		// Exit
		return NULL;
	}

	// The returned value
	const char * result = NULL;

	// The current enumeration entry
	struct dirent * entry;

	// The current enumeration entry's file statistics
	struct stat st;

	// Iterate all files inside the input device directory
	while ((entry = readdir(dir)))
	{
		// Allocate some memory for the file path
		char path[PATH_MAX];

		// And puzzle together an absolute file path
		snprintf(path, sizeof(path), "%s/%s", input_device_directory, entry->d_name);

		// We failed to get the file statistics
		if (lstat(path, &st) == -1)
		{
			// Continue onto the next file
			continue;
		}

		// Ignore everything but character devices
		if (S_ISCHR(st.st_mode))
		{
			// Open the input device
			int fd = open(path, O_RDONLY);

			// We couldn't open the input device
			if (fd < 0)
			{
				// Continue onto the next file
				continue;
			}

			// Allocate some memory for the device name
			char name[256];

			// We failed to get the device name
			if (ioctl(fd, EVIOCGNAME(sizeof(name)), name) < 1)
			{
				// Close the input device
				close(fd);

				// Continue onto the next file
				continue;
			}

			// Close the input device
			close(fd);

			// The device name matches
			if (strcmp(name, input_device_name) == 0)
			{
				// Copy the device path
				strcpy(device_path, path);

				// Set the result
				result = (const char *)device_path;

				// Break
				break;
			}
		}
	}

	// Close the directory
	closedir(dir);

	// Return the result
	return result;
}

const char * wait_for_input_device(const char * input_device_name)
{
	// The device path
	const char * device_path = NULL;

	// Wait for the device
	while (device_path == NULL)
	{
		// Find the device path
		device_path = get_input_device_path(input_device_name);
	}

	// Return the device path
	return device_path;
}
