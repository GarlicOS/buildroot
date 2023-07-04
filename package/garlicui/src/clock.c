#include <sys/time.h>
#include <sys/ioctl.h>
#include <linux/rtc.h>
#include <fcntl.h>

#include "clock.h"

/**
 * @brief The device directory that contains the RTC device nodes.
 */
#define DEVICE_DIRECTORY "/dev"

/**
 * @brief The RTC device node prefix.
 */
#define RTC_DEVICE_PREFIX "rtc"

/**
 * @brief Sets the current time.
 */
void clock_set_current_time(time_t timestamp)
{
	// Convert the given timestamp into a timeval structure
	struct timeval tv;
	tv.tv_sec = timestamp;
	tv.tv_usec = 0;

	// Set the time of day (which will save the time into the RTC if the system has one)
	settimeofday(&tv, NULL);

	// Convert the epoch timestamp into a tm structure
	struct tm * timeinfo = localtime(&timestamp);

	// Open the device directory
	DIR * dev_dir = opendir(DEVICE_DIRECTORY);

	// We managed to open the device directory
	if (dev_dir != NULL)
	{
		// The current directory entry
		struct dirent * dir_entry;

		// Iterate all directory entries
		while ((dir_entry = readdir(dev_dir)) != NULL)
		{
			// We've found a RTC device
			if (strncmp(dir_entry->d_name, RTC_DEVICE_PREFIX, strlen(RTC_DEVICE_PREFIX)) == 0)
			{
				// Allocate a buffer for the device path
				char * dev_path = malloc(strlen(DEVICE_DIRECTORY) + 1 + strlen(dir_entry->d_name) + 1);

				// We managed to allocate a buffer for the device path
				if (dev_path != NULL)
				{
					// Puzzle together the device path
					sprintf(dev_path, "%s/%s", DEVICE_DIRECTORY, dir_entry->d_name);

					// Open the device for read/write access
					int dev_fd = open(dev_path, O_RDWR);

					// We managed to open the device for read/write access
					if (dev_fd >= 0)
					{
						// Prepare the RTC time structure
						struct rtc_time rtc_time;
						rtc_time.tm_sec = timeinfo->tm_sec;
						rtc_time.tm_min = timeinfo->tm_min;
						rtc_time.tm_hour = timeinfo->tm_hour;
						rtc_time.tm_mday = timeinfo->tm_mday;
						rtc_time.tm_mon = timeinfo->tm_mon;
						rtc_time.tm_year = timeinfo->tm_year;
						rtc_time.tm_wday = timeinfo->tm_wday;
						rtc_time.tm_yday = timeinfo->tm_yday;
						rtc_time.tm_isdst = timeinfo->tm_isdst;

						// Set the RTC
						ioctl(dev_fd, RTC_SET_TIME, &rtc_time);

						// Close the device
						close(dev_fd);
					}

					// Free the device path buffer
					free(dev_path);
				}
			}
		}

		// Close the device directory
		closedir(dev_dir);
	}
}

/**
 * @brief Checks whether the given year is a leap year.
 */
int clock_is_leap_year(int year)
{
	// A leap year is a year that is evenly divisible by 4...
	if (year % 4 == 0)
	{
		// Except years that are evenly divisable by 100...
		if (year % 100 == 0)
		{
			// With the exception of those evenly divisable by 400...
			if (year % 400 == 0)
			{
				// This is a leap year
				return 1;
			}

			// All others are regular years
			else
			{
				// This is a regular year
				return 0;
			}
		}

		// All others are leap years
		else
		{
			// This is a leap year
			return 1;
		}
	}

	// All others are regular years
	else
	{
		// This is a regular year
		return 0;
	}

	// This is a regular year
	return 0;
}

/**
 * @brief Returns the number of days in the given month.
 */
int clock_get_days_in_month(struct tm * calendar_time)
{
	// The number of days in this month
	int max_days = 31;

	// Handle the different months
	switch (calendar_time->tm_mon)
	{
		// February
		case 1:
		{
			// We're in a leap year
			if (clock_is_leap_year(calendar_time->tm_year + 1900))
			{
				// February has 29 days in leap years
				max_days = 29;
			}

			// We're in a regular year
			else
			{
				// February has 28 days in regular years
				max_days = 28;
			}

			// Prevent fall-through
			break;
		}

		// April, June, September, November
		case 3:
		case 5:
		case 8:
		case 10:
		{
			// These months have 30 days
			max_days = 30;

			// Prevent fall-through
			break;
		}
	}

	// Return the number of days in this month
	return max_days;
}

/**
 * @brief Takes the given raw time and renders its component into strings.
 */
void clock_render_text_parts(time_t raw_time, int meridian, char ** year, char ** month, char ** day, char ** hour, char ** minute, char ** suffix)
{
	// Convert the local time into a tm structure
	struct tm * calendar_time = localtime(&raw_time);

	// Allocate a temporary buffer so we can format the time
	static char clock_buffer[256];

	// The current buffer position
	char * buffer = clock_buffer;

	// The caller wants the year part
	if (year != NULL)
	{
		// Extract the year
		*year = buffer;
		strftime(*year, (size_t)(clock_buffer + sizeof(clock_buffer) - buffer), "%Y", calendar_time);
		buffer += strlen(*year) + 1;
	}

	// The caller wants the month part
	if (month != NULL)
	{
		// Extract the 2-digit month
		*month = buffer;
		strftime(*month, (size_t)(clock_buffer + sizeof(clock_buffer) - buffer), "%m", calendar_time);
		buffer += strlen(*month) + 1;
	}

	// The caller wants the day part
	if (day != NULL)
	{
		// Extract the 2-digit day
		*day = buffer;
		strftime(*day, (size_t)(clock_buffer + sizeof(clock_buffer) - buffer), "%d", calendar_time);
		buffer += strlen(*day) + 1;
	}

	// The caller wants the hour part
	if (hour != NULL)
	{
		// Extract the 2-digit hour
		*hour = buffer;
		strftime(*hour, (size_t)(clock_buffer + sizeof(clock_buffer) - buffer), meridian ? "%I" : "%H", calendar_time);
		buffer += strlen(*hour) + 1;
	}

	// The caller wants the minute part
	if (minute != NULL)
	{
		// Extract the 2-digit minute
		*minute = buffer;
		strftime(*minute, (size_t)(clock_buffer + sizeof(clock_buffer) - buffer), "%M", calendar_time);
		buffer += strlen(*minute) + 1;
	}

	// The caller wants the suffix
	if (suffix != NULL)
	{
		// Extract the suffix
		*suffix = buffer;
		strftime(*suffix, (size_t)(clock_buffer + sizeof(clock_buffer) - buffer), "%p", calendar_time);
		buffer += strlen(*suffix) + 1;
	}
}

/**
 * @brief Formats the current time into a string.
 */
const char * clock_render_text(int meridian)
{
	// The local time
	time_t local_time;

	// Get the local time
	time(&local_time);

	// Convert the local time into a tm structure
	struct tm * calendar_time = localtime(&local_time);

	// Allocate a temporary buffer so we can format the time
	static char clock_buffer[16];

	// Format the clock style
	strftime(clock_buffer, sizeof(clock_buffer), meridian ? "%I:%M %p" : "%H:%M", calendar_time);

	// Return the formatted time
	return (const char *)clock_buffer;
}

/**
 * @brief Renders the clock overlay surface.
 */
SDL_Surface * clock_render(int meridian, TTF_Font * font, Uint32 color)
{
	// Render and return the time's surface
	return SDL_TrimTransparent(TTF_RenderUTF8_Blended(font, clock_render_text(meridian), SDL_ToSDLColor(color)), 1);
}