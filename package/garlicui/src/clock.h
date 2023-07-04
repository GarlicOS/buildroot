#ifndef CLOCK_H
#define CLOCK_H

#include "sdl.h"

/**
 * @brief Sets the current time.
 */
void clock_set_current_time(time_t timestamp);

/**
 * @brief Checks whether the given year is a leap year.
 */
int clock_is_leap_year(int year);

/**
 * @brief Returns the number of days in the given month.
 */
int clock_get_days_in_month(struct tm * calendar_time);

/**
 * @brief Takes the given raw time and renders its component into strings.
 */
void clock_render_text_parts(time_t raw_time, int meridian, char ** year, char ** month, char ** day, char ** hour, char ** minute, char ** suffix);

/**
 * @brief Formats the current time into a string.
 */
const char * clock_render_text(int meridian);

/**
 * @brief Renders the clock overlay surface.
 */
SDL_Surface * clock_render(int meridian, TTF_Font * font, Uint32 color);

#endif