#ifndef BATTERY_H
#define BATTERY_H

#include "io.h"
#include "icon.h"

/**
 * @brief Checks whether the battery is currently charging.
 */
int battery_is_charging();

/**
 * @brief Gets the battery percentage.
 */
int battery_percentage();

/**
 * @brief Renders the battery status overlay.
 */
SDL_Surface * battery_render(float scale_factor, Uint32 color);

#endif