#ifndef SDL_H
#define SDL_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_rotozoom.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <librsvg/rsvg.h>

// https://github.com/OpenDingux/SDL/blob/dd7260f1d7f79a58aba95a03fd6532729181eadb/include/SDL_main.h#L54
#if defined(main)
#undef main
#endif

/**
 * @brief Fills the given SDL surface with a solid color.
 *
 * This function is essentially a QoL one-line helper wrapper around SDL_FillRect.
 * Use SDL_MapRGB to get the color.
 */
void SDL_FillSurface(SDL_Surface * surface, Uint32 color);

/**
 * @brief Converts Uint32 colors to SDL_Color structures.
 */
SDL_Color SDL_ToSDLColor(Uint32 color);

/**
 * @brief Opens a font, renders the given text in the given text and closes it again.
 *
 * This function is a QoL one-line helper wrapper for text rendering of fonts with a very short life-time.
 * For example icon fonts where icon size changes would require caching of countless font copies.
 * This might not be possible on low-RAM devices, and as such fonts should be discarded as soon as possible.
 *
 * Another example where this is extremely useful is for text rendering within a known limited-width boundary.
 * Doing so in SDL requires re-opening the same font at different sizes in quick succession where caching just makes no sense.
 */
SDL_Surface * SDL_TTF_RenderUTF8_Blended_OneShot(const char * font_path, int height, const char * text, SDL_Color color);

/**
 * @brief Renders the given text at a font size that will fit into a surface with the given dimensions.
 */
SDL_Surface * SDL_TTF_RenderUTF8_Blended_MaxWidth(const char * font_path, int maximum_width, int maximum_height, const char * text, SDL_Color color, int trim_transparency);

/**
 * @brief Trims transparent regions off each side of the given surface.
 *
 * This is most useful when rendering glyphs from icon fonts where padding is unwanted.
 */
SDL_Surface * SDL_TrimTransparent(SDL_Surface * surface, int free_input_surface);

/**
 * @brief A custom implementation of SDL_BlitSurface that works around some SDL 1.2 alpha blending bugs.
 */
void SDL_BlitSurfaceAlpha(SDL_Surface * source, SDL_Rect * srcrect, SDL_Surface * destination, SDL_Rect * dstrect);

/**
 * @brief Loads a SVG vector graphic from the given file path and renders it at the given resolution.
 *
 * This function is essentially a vector graphic version of IMG_Load.
 * The returned SDL_Surface must be freed explicitly.
 */
SDL_Surface * SVG_Load(const char * file_path, int width, int height, Uint32 * default_fill_color);

/**
 * @brief Converts the given surface into a 32bit RGBA surface.
 */
SDL_Surface * SDL_ConvertToRGBASurface(SDL_Surface * surface, int free_input_surface);

/**
 * @brief Applies a vertical fade effect to the given surface.
 */
void SDL_VerticalFade(SDL_Surface * surface, double fadePercentage, double transparentPercentage);

/**
 * @brief Renders an anti-aliased filled circle.
 */
void SDL_aaFilledCircleRGBA(SDL_Surface * surface, Sint16 x, Sint16 y, Sint16 radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

#endif
