#ifndef ICON_H
#define ICON_H

#include "sdl.h"
#include "io.h"

/**
 * @brief The unscaled default icon height (based on a 640x480 reference implementation).
 */
#define DEFAULT_BAR_ICON_HEIGHT 24

/**
 * @brief The unscaled default paging card width (based on a 640x480 reference implementation).
 */
#define DEFAULT_PAGING_CARD_WIDTH 140

/**
 * @brief The unscaled default paging card rounded corner radius (based on a 640x480 reference implementation).
 */
#define DEFAULT_PAGING_CARD_ROUNDED_CORNER_RADIUS 10

/**
 * @brief The top vertical alignment.
 */
#define VERTICAL_ALIGNMENT_TOP 0

/**
 * @brief The center vertical alignment.
 */
#define VERTICAL_ALIGNMENT_CENTER 1

/**
 * @brief The bottom vertical alignment.
 */
#define VERTICAL_ALIGNMENT_BOTTOM 2

/**
 * @brief The top vertical alignment (considering text).
 */
#define VERTICAL_ALIGNMENT_TEXT_TOP 3

/**
 * @brief The center vertical alignment (considering text).
 */
#define VERTICAL_ALIGNMENT_TEXT_CENTER 4

/**
 * @brief The bottom vertical alignment (considering text).
 */
#define VERTICAL_ALIGNMENT_TEXT_BOTTOM 5

/**
 * @brief Renders a pill with the given dimensions and color.
 */
SDL_Surface * icon_pill(int width, int height, Uint32 color);

/**
 * @brief Renders a rounded rectangle with the given dimensions, corner radius and color.
 */
SDL_Surface * icon_rounded_rectangle(int width, int height, int radius, Uint32 color);

/**
 * @brief Renders a circled text button.
 */
SDL_Surface * icon_circled_text(TTF_Font * font, Uint32 foreground_color, Uint32 background_color, const char * text, int minimum_width, int minimum_height, int padding, SDL_Surface * icon, int free_icon);

/**
 * @brief Renders a paging card.
 */
SDL_Surface * icon_paging_card(SDL_Surface * icon, int vertical_icon_alignment, const char * font_path, int font_height, Uint32 foreground_color, Uint32 background_color, int padding, int width, int height, int radius, const char * text);

/**
 * @brief Loads a SVG vector graphic from the system icon folder and renders it at the given resolution.
 *
 * This function is essentially a vector graphic version of IMG_Load.
 * The returned SDL_Surface must be freed explicitly.
 */
SDL_Surface * icon_load(const char * relative_file_path, int width, int height, Uint32 * default_fill_color);

#endif