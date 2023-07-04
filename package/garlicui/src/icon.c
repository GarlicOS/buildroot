#include "icon.h"

/**
 * @brief Renders a pill with the given dimensions and color.
 */
SDL_Surface * icon_pill(int width, int height, Uint32 color)
{
	// The rendered surface
	SDL_Surface * surface = NULL;

	// Convert the color into a SDL color
	SDL_Color sdl_color = SDL_ToSDLColor(color);

	// Round up to multiples of 2 (this helps make the half-circles line up perfectly)
	if (height % 2 != 0)
	{
		// +1
		height++;
	}

	// Create the circle surface
	SDL_Surface * circle_surface = SDL_CreateRGBSurface(SDL_SRCALPHA, height, height, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	// We managed to create the circle surface
	if (circle_surface != NULL)
	{
		// Draw the anti-aliased circle
		SDL_aaFilledCircleRGBA(circle_surface, height >> 1, height >> 1, height >> 1, sdl_color.r, sdl_color.g, sdl_color.b, 255);

		// Create the surface
		surface = SDL_CreateRGBSurface(SDL_SRCALPHA, width, height, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

		// We managed to create the surface
		if (surface != NULL)
		{
			// Draw the left half circle
			SDL_Rect left_half_circle;
			left_half_circle.x = 0;
			left_half_circle.y = 0;
			left_half_circle.w = circle_surface->w >> 1;
			left_half_circle.h = circle_surface->h;
			SDL_BlitSurfaceAlpha(circle_surface, &left_half_circle, surface, &left_half_circle);

			// Draw the right half circle
			SDL_Rect right_half_circle_in;
			right_half_circle_in.x = circle_surface->w >> 1;
			right_half_circle_in.y = 0;
			right_half_circle_in.w = circle_surface->w >> 1;
			right_half_circle_in.h = circle_surface->h;
			SDL_Rect right_half_circle_out;
			right_half_circle_out.x = width - (circle_surface->w >> 1);
			right_half_circle_out.y = 0;
			right_half_circle_out.w = circle_surface->w >> 1;
			right_half_circle_out.h = circle_surface->h;
			SDL_BlitSurfaceAlpha(circle_surface, &right_half_circle_in, surface, &right_half_circle_out);

			// Draw the center rectangle
			boxColor(surface, circle_surface->w >> 1, 1 /* This y+1 offset is eye-balled... */, width - (circle_surface->w >> 1), height, SDL_MapRGB(surface->format, sdl_color.r, sdl_color.g, sdl_color.b));
		}

		// Free the circle surface
		SDL_FreeSurface(circle_surface);
	}

	// Return the rendered surface
	return surface;
}

/**
 * @brief Renders a rounded rectangle with the given dimensions, corner radius and color.
 */
SDL_Surface * icon_rounded_rectangle(int width, int height, int radius, Uint32 color)
{
	// The rendered surface
	SDL_Surface * surface = NULL;

	// Calculate the pill height
	int pill_height = radius << 1;

	// The pill height can never be larger than the actual surface height
	if (pill_height > height)
	{
		// Use the actual height instead
		pill_height = height;

		// The surface height ain't a multiple of two
		if (pill_height % 2 != 0)
		{
			// Round down to fit into the surface
			pill_height--;
		}
	}

	// Render a pill
	SDL_Surface * pill = icon_pill(width, pill_height, color);

	// We managed to render the pill
	if (pill != NULL)
	{
		// Create the surface
		surface = SDL_CreateRGBSurface(SDL_SRCALPHA, width, height, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

		// We managed to create the surface
		if (surface != NULL)
		{
			// Render the top half of the pill
			SDL_Rect top_half_pill;
			top_half_pill.x = 0;
			top_half_pill.y = 0;
			top_half_pill.w = pill->w;
			top_half_pill.h = pill->h >> 1;
			SDL_BlitSurfaceAlpha(pill, &top_half_pill, surface, &top_half_pill);

			// Render the bottom half of the pill
			SDL_Rect bottom_half_pill_in;
			bottom_half_pill_in.x = 0;
			bottom_half_pill_in.y = pill->h >> 1;
			bottom_half_pill_in.w = pill->w;
			bottom_half_pill_in.h = pill->h >> 1;
			SDL_Rect bottom_half_pill_out;
			bottom_half_pill_out.x = 0;
			bottom_half_pill_out.y = surface->h - (pill->h >> 1);
			bottom_half_pill_out.w = pill->w;
			bottom_half_pill_out.h = pill->h >> 1;
			SDL_BlitSurfaceAlpha(pill, &bottom_half_pill_in, surface, &bottom_half_pill_out);

			// Convert the color into a SDL color
			SDL_Color sdl_color = SDL_ToSDLColor(color);

			// Draw the center rectangle
			boxColor(surface, 1 /* This x+1 offset is eye-balled... */, pill->h >> 1, surface->w, surface->h - (pill->h >> 1), SDL_MapRGB(surface->format, sdl_color.r, sdl_color.g, sdl_color.b));
		}

		// Free the pill surface
		SDL_FreeSurface(pill);
	}

	// Return the rendered surface
	return surface;
}

/**
 * @brief Renders a circled text button.
 */
SDL_Surface * icon_circled_text(TTF_Font * font, Uint32 foreground_color, Uint32 background_color, const char * text, int minimum_width, int minimum_height, int padding, SDL_Surface * icon, int free_icon)
{
	// The render icon surface
	SDL_Surface * surface = NULL;

	// Convert the foreground color into a SDL color
	SDL_Color sdl_foreground_color = SDL_ToSDLColor(foreground_color);

	// Render the text surface (we intentionally don't trim it because over-height letters like y would shift the center if we did)
	SDL_Surface * text_surface = TTF_RenderUTF8_Blended(font, text, sdl_foreground_color);

	// We managed to render the description text
	if (text_surface != NULL)
	{
		// Initialize the maximum inner surface height with the text surface's height
		int maximum_inner_surface_height = text_surface->h;

		// The given icon's height exceeds that of the text surface
		if (icon != NULL && icon->h > maximum_inner_surface_height)
		{
			// Thus we need to use the icon's height as the new maximum inner surface height
			maximum_inner_surface_height = icon->h;
		}

		// Calculate the final surface height (inner surface height + padding on top & bottom)
		int surface_height = maximum_inner_surface_height + (2 * padding);

		// The user wants a minimum height
		if (surface_height < minimum_height)
		{
			// Set the minimum height
			surface_height = minimum_height;
		}

		// Calculate the icon surface width (title text width + padding + description text width + 1 half circle on each end)
		int surface_width = (icon != NULL ? (icon->w + (2 * padding)) : 0) + text_surface->w + surface_height;

		// The user wants a minimum width
		if (surface_width < minimum_width)
		{
			// Set the minimum width
			surface_width = minimum_width;
		}

		// The rendered pill surface
		surface = icon_pill(surface_width, surface_height, background_color);

		// We managed to render the pill surface
		if (surface != NULL)
		{
			// The icon position
			SDL_Rect icon_position;

			// We've been given an icon
			if (icon != NULL)
			{
				// Render the title text onto the surface
				icon_position.x = (surface->w - icon->w - (2 * padding) - text_surface->w) >> 1;
				icon_position.y = (surface->h - icon->h) >> 1;
				icon_position.w = icon->w;
				icon_position.h = icon->h;
				SDL_BlitSurfaceAlpha(icon, NULL, surface, &icon_position);
			}

			// Render the description text onto the surface
			SDL_Rect text_position;
			text_position.x = icon != NULL ? (icon_position.x + icon_position.w + (2 * padding)) : ((surface->w - text_surface->w) >> 1);
			text_position.y = (surface->h - text_surface->h) >> 1;
			text_position.w = text_surface->w;
			text_position.h = text_surface->h;
			SDL_BlitSurfaceAlpha(text_surface, NULL, surface, &text_position);
		}

		// Free the description text surface
		SDL_FreeSurface(text_surface);
	}

	// We've been asked to free the icon surface
	if (free_icon && icon != NULL)
	{
		// Free the icon surface
		SDL_FreeSurface(icon);
	}

	// Return the rendered icon surface
	return surface;
}

/**
 * @brief Renders a paging card.
 */
SDL_Surface * icon_paging_card(SDL_Surface * icon, int vertical_icon_alignment, const char * font_path, int font_height, Uint32 foreground_color, Uint32 background_color, int padding, int width, int height, int radius, const char * text)
{
	// The rendered surface
	SDL_Surface * surface = NULL;

	// Convert the foreground color into a SDL color
	SDL_Color sdl_foreground_color = SDL_ToSDLColor(foreground_color);

	// Render the text (we intentionally don't trim it because over-height letters like y would shift the center if we did)
	SDL_Surface * text_surface = SDL_TTF_RenderUTF8_Blended_MaxWidth(font_path, width - (2 * padding), font_height, text, sdl_foreground_color, 0);

	// We managed to render the text
	if (text_surface != NULL)
	{
		// Render the rounded rectangle
		surface = icon_rounded_rectangle(width, height, radius, background_color);

		// We managed to render the rounded rectangle
		if (surface != NULL)
		{
			// Calculate the text position
			SDL_Rect text_surface_position;
			text_surface_position.x = (surface->w - text_surface->w) >> 1;
			text_surface_position.y = surface->h - padding - text_surface->h;
			text_surface_position.w = text_surface->w;
			text_surface_position.h = text_surface->h;

			// We've been given an icon
			if (icon != NULL)
			{
				// Blit the icon onto the rounded rectangle
				SDL_Rect icon_position;
				icon_position.x = (surface->w - icon->w) >> 1;
				switch (vertical_icon_alignment)
				{
					case VERTICAL_ALIGNMENT_TEXT_CENTER:
						icon_position.y = (surface->h - padding - text_surface->h - padding - icon->h) >> 1;
						break;
					case VERTICAL_ALIGNMENT_TEXT_BOTTOM:
						icon_position.y = surface->h - padding - text_surface->h - padding - icon->h;
						break;
					case VERTICAL_ALIGNMENT_TEXT_TOP:
					case VERTICAL_ALIGNMENT_TOP:
						icon_position.y = padding;
						break;
					case VERTICAL_ALIGNMENT_BOTTOM:
						icon_position.y = surface->h - padding - icon->h;
						break;
					case VERTICAL_ALIGNMENT_CENTER:
					default:
						icon_position.y = (surface->h - icon->h) >> 1;
				}
				icon_position.w = icon->w;
				icon_position.h = icon->h;
				SDL_BlitSurfaceAlpha(icon, NULL, surface, &icon_position);
			}

			// Blit the text onto the rounded rectangle
			SDL_BlitSurfaceAlpha(text_surface, NULL, surface, &text_surface_position);
		}

		// Free the text surface
		SDL_FreeSurface(text_surface);
	}

	// Return the rendered surface
	return surface;
}