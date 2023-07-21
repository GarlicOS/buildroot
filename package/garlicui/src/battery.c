#include <stdio.h>
#include <limits.h>

#include "battery.h"

/**
 * @brief The unscaled battery icon width (based around a 640x480 reference implementation).
 */
#define DEFAULT_BATTERY_ICON_WIDTH 43

/**
 * @brief The unscaled battery icon height (based around a 640x480 reference implementation).
 */
#define DEFAULT_BATTERY_ICON_HEIGHT DEFAULT_BAR_ICON_HEIGHT

/**
 * @brief Gets the battery capacity path.
 */
static const char * battery_capacity_path()
{
	// Get the device-specific battery capacity path
	const char * path = (const char *)getenv("BATTERY_CAPACITY_PATH");

	// We have no device-specific battery capacity path to work with
	if (path == NULL)
	{
		// Fall back to a "safe" default
		path = "/sys/class/power_supply/battery/capacity";
	}

	// Return the battery capacity path
	return path;
}

/**
 * @brief Gets the charger online status path.
 */
static const char * battery_charger_online_status_path()
{
	// Get the device-specific charger online path
	const char * path = (const char *)getenv("CHARGER_ONLINE_PATH");

	// We have no device-specific charger online path to work with
	if (path == NULL)
	{
		// Fall back to a "safe" default
		path = "/sys/class/power_supply/usb/online";
	}

	// Return the charger online path
	return path;
}

/**
 * @brief Checks whether the battery is currently charging.
 */
int battery_is_charging()
{
	// The charger state
	int online = -1;

	// Attempt to read the charger state
	io_read_int(battery_charger_online_status_path(), &online);

	// Return the charger state
	return online;
}

/**
 * @brief Gets the battery percentage.
 */
int battery_percentage()
{
	// The battery percentage
	int percentage = -1;

	// Attempt to read the battery percentage
	io_read_int(battery_capacity_path(), &percentage);

	// Return the battery percentage
	return percentage;
}

/**
 * @brief Renders the battery status overlay.
 */
SDL_Surface * battery_render(float scale_factor, Uint32 color)
{
	// Create a SDL surface with the desired dimensions
	SDL_Surface * surface = SDL_CreateRGBSurface(SDL_SRCALPHA, DEFAULT_BATTERY_ICON_WIDTH * scale_factor, DEFAULT_BATTERY_ICON_HEIGHT * scale_factor, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	// We managed to create the required SDL surface
	if (surface != NULL)
	{
		// Convert the given color into a SDL_Color structure
		SDL_Color battery_color = SDL_ToSDLColor(color);

		// Scale the outline width and padding
		int battery_outline_width = 3 * scale_factor;
		int battery_fill_padding = 2 * scale_factor;

		// Render the battery main body
		boxRGBA(surface, 0, 0, surface->w - battery_outline_width - 1, surface->h - 1, battery_color.r, battery_color.g, battery_color.b, 0xff);

		// Render the battery's positive pin
		boxRGBA(surface, surface->w - battery_outline_width, surface->h >> 2, surface->w - 1, surface->h - (surface->h >> 2) - 1, battery_color.r, battery_color.g, battery_color.b, 0xff);

		// Hollow out the battery main body to build visible padding
		boxRGBA(surface, battery_outline_width, battery_outline_width, surface->w - (battery_outline_width << 1) - 1, surface->h - battery_outline_width - 1, 0, 0, 0, 0);

		// Determine the current battery percentage
		int percentage = battery_percentage();

		// Determine the current charging status
		int charging = battery_is_charging();

		// Pick a suitable battery fill color based on the current charging status
		if (charging)
		{
			battery_color.r = 0x54;
			battery_color.g = 0xa1;
			battery_color.b = 0x0c;
		}

		// Calculate the maximum battery fill width
		int max_fill_width = surface->w - (battery_outline_width * 3) - (battery_fill_padding << 1);

		// And the current battery fill width
		int fill_width = (int)((float)max_fill_width * (float)percentage / 100.0f);

		// Fill the battery main body
		boxRGBA(surface, battery_outline_width + battery_fill_padding, battery_outline_width + battery_fill_padding, surface->w - (battery_outline_width << 1) - battery_fill_padding - 1, surface->h - battery_outline_width - battery_fill_padding - 1, battery_color.r, battery_color.g, battery_color.b, 0xff);
	}

	// Return the rendered battery icon
	return surface;
}