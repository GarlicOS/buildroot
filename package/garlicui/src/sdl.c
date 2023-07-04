#include "sdl.h"

/**
 * @brief Fills the given SDL surface with a solid color.
 *
 * This function is essentially a QoL one-line helper wrapper around SDL_FillRect.
 * Use SDL_MapRGB to get the color.
 */
void SDL_FillSurface(SDL_Surface * surface, Uint32 color)
{
	// Target the whole surface
	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = surface->w;
	rect.h = surface->h;

	// And fill it with the chosen color
	SDL_FillRect(surface, &rect, color);
}

/**
 * @brief Converts Uint32 colors to SDL_Color structures.
 */
SDL_Color SDL_ToSDLColor(Uint32 color)
{
	// Create a SDL_Color structure
	SDL_Color sdlColor;

	// Populate its red component
	sdlColor.r = (color >> 16) & 0xFF;

	// Populate its green component
	sdlColor.g = (color >> 8) & 0xFF;

	// Populate its blue component
	sdlColor.b = color & 0xFF;

	// Return the SDL_Color structure
	return sdlColor;
}

/**
 * @brief Opens a font, renders the given text in the given text and closes it again.
 *
 * This function is a QoL one-line helper wrapper for text rendering of fonts with a very short life-time.
 * For example icon fonts where icon size changes would require caching of countless font copies.
 * This might not be possible on low-RAM devices, and as such fonts should be discarded as soon as possible.
 */
SDL_Surface * SDL_TTF_RenderUTF8_Blended_OneShot(const char * font_path, int height, const char * text, SDL_Color color)
{
	// The rendered text
	SDL_Surface * surface = NULL;

	// Open the font
	TTF_Font * font = TTF_OpenFont(font_path, height);

	// We managed to open the font
	if (font != NULL)
	{
		// Render the text
		surface = TTF_RenderUTF8_Blended(font, text, color);

		// Close the font
		TTF_CloseFont(font);
	}

	// Return the rendered text
	return surface;
}

/**
 * @brief Renders the given text at a font size that will fit into a surface with the given dimensions.
 */
SDL_Surface * SDL_TTF_RenderUTF8_Blended_MaxWidth(const char * font_path, int maximum_width, int maximum_height, const char * text, SDL_Color color, int trim_transparency)
{
	// The rendered text
	SDL_Surface * surface = NULL;

	// The chosen font size
	int font_size = maximum_height;

	// Keep re-rendering the text until it fits the required boundary
	while (font_size > 0)
	{
		// Render the text
		surface = SDL_TTF_RenderUTF8_Blended_OneShot(font_path, font_size, text, color);

		// The caller wants to trim away all transparent pixels on all sides
		if (trim_transparency && surface != NULL)
		{
			// Trim away all transparent pixels on all sides
			surface = SDL_TrimTransparent(surface, 1);
		}

		// We failed to render the text
		if (surface == NULL)
		{
			// We're likely out of RAM, so stop trying
			break;
		}

		// The rendered text fits
		if (surface->w <= maximum_width /*&& surface->h <= maximum_height*/)
		{
			// We're done
			break;
		}

		// Reduce the font size
		font_size--;

		// Free the surface
		SDL_FreeSurface(surface);

		// And NULL its reference
		surface = NULL;
	}

	// Return the rendered text
	return surface;
}

/**
 * @brief Trims transparent regions off each side of the given surface.
 *
 * This is most useful when rendering glyphs from icon fonts where padding is unwanted.
 */
SDL_Surface * SDL_TrimTransparent(SDL_Surface * surface, int free_input_surface)
{
	// We've been given a NULL surface
	if (surface == NULL)
	{
		// Thus return a NULL surface as well
		return NULL;
	}

	// The opaque region boundary
	int minX = surface->w;
	int minY = surface->h;
	int maxX = 0;
	int maxY = 0;

	// The input surface must be locked to access the pixels
	if (SDL_MUSTLOCK(surface))
	{
		// Lock the input surface
		if (SDL_LockSurface(surface) != 0)
		{
			// Return the input surface (its better than nothing)
			return surface;
		}
	}

	// Cast the input pixels
	Uint32 * pixels = (Uint32 *)surface->pixels;

	// Calculate the pixel count
	int pixelCount = surface->w * surface->h;

	// Mask out the RGB channels
	Uint32 pixelMask = surface->format->Amask;

	// Iterate all pixels
	for (int i = 0; i < pixelCount; ++i)
	{
		// Fetch the pixel
		Uint32 pixel = pixels[i];

		// This pixel is at least partially opaque
		if ((pixel & pixelMask) != 0)
		{
			// Calculate the pixel coordinates
			int x = i % surface->w;
			int y = i / surface->w;

			// Update the opaque image boundary
			if (x < minX) minX = x;
			if (y < minY) minY = y;
			if (x > maxX) maxX = x;
			if (y > maxY) maxY = y;
		}
	}

	// Calculate the dimensions of the trimmed surface
	int trimmedWidth = maxX - minX + 1;
	int trimmedHeight = maxY - minY + 1;

	// Special case handling for all transparent surfaces
	if (trimmedWidth < 0 || trimmedHeight < 0)
	{
		// The input surface is currently locked
		if (SDL_MUSTLOCK(surface))
		{
			// Unlock the input surface
			SDL_UnlockSurface(surface);
		}

		// Return the input surface (its better than nothing)
		return surface;
	}

	// Create a new surface with the trimmed dimensions
	SDL_Surface* trimmedSurface = SDL_CreateRGBSurface(surface->flags, trimmedWidth, trimmedHeight, surface->format->BitsPerPixel, surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, surface->format->Amask);

	// We failed to create the trimmed surface
	if (trimmedSurface == NULL)
	{
		// The input surface is currently locked
		if (SDL_MUSTLOCK(surface))
		{
			// Unlock the input surface
			SDL_UnlockSurface(surface);
		}

		// Return the input surface (its better than nothing)
		return surface;
	}

	// The output surface must be locked to access the pixels
	if (SDL_MUSTLOCK(trimmedSurface))
	{
		// Lock the output surface
		if (SDL_LockSurface(trimmedSurface) != 0)
		{
			// The input surface is currently locked
			if (SDL_MUSTLOCK(surface))
			{
				// Unlock the input surface
				SDL_UnlockSurface(surface);
			}

			// Free the output surface
			SDL_FreeSurface(trimmedSurface);

			// Return the input surface (its better than nothing)
			return surface;
		}
	}

	// Cast the output pixels
	Uint32 * trimmedPixels = (Uint32 *)trimmedSurface->pixels;

	// Calculate the output pitch
	int trimmedPitch = trimmedSurface->pitch / sizeof(Uint32);

	// Iterate the rows
	for (int y = minY; y <= maxY; ++y)
	{
		// Iterate the columns
		for (int x = minX; x <= maxX; ++x)
		{
			// Fetch the pixel
			Uint32 pixel = pixels[y * surface->w + x];

			// Calculate the coordinates in the trimmed surface
			int trimmedX = x - minX;
			int trimmedY = y - minY;

			// Write the pixel
			trimmedPixels[trimmedY * trimmedPitch + trimmedX] = pixel;
		}
	}

	// The input surface is currently locked
	if (SDL_MUSTLOCK(surface))
	{
		// Unlock the input surface
		SDL_UnlockSurface(surface);
	}

	// The output surface is currently locked
	if (SDL_MUSTLOCK(trimmedSurface))
	{
		// Unlock the output surface
		SDL_UnlockSurface(trimmedSurface);
	}

	// The caller wants us to free the input surface
	if (free_input_surface)
	{
		// Free the input surface
		SDL_FreeSurface(surface);
	}

	// Return the trimmed surface
	return trimmedSurface;
}

/**
 * @brief A custom implementation of SDL_BlitSurface that works around some SDL 1.2 alpha blending bugs.
 */
void SDL_BlitSurfaceAlpha(SDL_Surface * source, SDL_Rect * srcrect, SDL_Surface * destination, SDL_Rect * dstrect)
{
	// The source surface rectangle
	SDL_Rect source_rectangle;

	// Initialize the source surface rectangle with safe whole-surface defaults
	source_rectangle.x = 0;
	source_rectangle.y = 0;
	source_rectangle.w = source->w;
	source_rectangle.h = source->h;

	// The caller provides a custom source rectangle
	if (srcrect != NULL)
	{
		// Override the defaults
		source_rectangle = *srcrect;
	}

	// The destination surface rectangle
	SDL_Rect destination_rectangle;

	// Initialize the destination surface rectangle with safe whole-surface defaults
	destination_rectangle.x = 0;
	destination_rectangle.y = 0;
	destination_rectangle.w = destination->w;
	destination_rectangle.h = destination->h;

	// The caller provides a custom destination rectangle
	if (dstrect != NULL)
	{
		// Override the defaults
		destination_rectangle = *dstrect;
	}

	// Manually blend alpha (because the SDL code is bugged)
	uint8_t * destinationPixels = destination->pixels;
	uint8_t * sourcePixels = source->pixels;

	// Find the shared minimum width
	int width = source_rectangle.w < destination_rectangle.w ? source_rectangle.w : destination_rectangle.w;

	// Find the shared minimum height
	int height = source_rectangle.h < destination_rectangle.h ? source_rectangle.h : destination_rectangle.h;

	// Iterate the pixel rows
	for (int y = 0; y < height; y++)
	{
		// Ignore off-screen rows
		if (y < 0 || (source_rectangle.y + y) >= source->h || (destination_rectangle.y + y) >= destination->h)
			continue;

		// Calculate the destination pixel row pointer
		uint8_t * destinationPixelRow = destinationPixels + (destination_rectangle.y + y) * destination->pitch;

		// Calculate the source pixel row pointer
		uint8_t * sourcePixelRow = sourcePixels + (source_rectangle.y + y) * source->pitch;

		// Iterate the in-row pixels
		for (int x = 0; x < width; x++)
		{
			// Ignore off-screen pixels
			if (x < 0 || (source_rectangle.x + x) >= source->w || (destination_rectangle.x + x) >= destination->w)
				continue;

			// Calculate the destination pixel pointer
			uint8_t * destinationPixel = destinationPixelRow + (destination_rectangle.x + x) * 4;

			// Calculate the source pixel pointer
			uint8_t * sourcePixel = sourcePixelRow + (source_rectangle.x + x) * 4;

			// Extract the source pixel components
			float aA = (float)sourcePixel[3];
			float rA = (float)sourcePixel[2];
			float gA = (float)sourcePixel[1];
			float bA = (float)sourcePixel[0];

			// Extract the destination pixel components
			float aB = (float)destinationPixel[3];
			float rB = (float)destinationPixel[2];
			float gB = (float)destinationPixel[1];
			float bB = (float)destinationPixel[0];

			// Blend the pixels together
			float aOut = aA + (aB * (255.0f - aA) / 255.0f);
			float rOut = (rA * aA + rB * aB * (255.0f - aA) / 255.0f) / aOut;
			float gOut = (gA * aA + gB * aB * (255.0f - aA) / 255.0f) / aOut;
			float bOut = (bA * aA + bB * aB * (255.0f - aA) / 255.0f) / aOut;

			// And replace the destination pixel
			destinationPixel[3] = (uint8_t)aOut;
			destinationPixel[2] = (uint8_t)rOut;
			destinationPixel[1] = (uint8_t)gOut;
			destinationPixel[0] = (uint8_t)bOut;
		}
	}
}

/**
 * @brief Loads a SVG vector graphic from the given file path and renders it at the given resolution.
 *
 * This function is essentially a vector graphic version of IMG_Load.
 * The returned SDL_Surface must be freed explicitly.
 */
SDL_Surface * SVG_Load(const char * file_path, int width, int height, Uint32 * default_fill_color)
{
	// The rendered SDL surface
	SDL_Surface * result = NULL;

	// Load and parse the SVG file
	RsvgHandle * svg_handle = rsvg_handle_new_from_file(file_path, NULL);

	// We managed to load and parse the SVG file
	if (svg_handle != NULL)
	{
		// Get the SVG dimensions
		RsvgDimensionData svg_dimensions;
		rsvg_handle_get_dimensions(svg_handle, &svg_dimensions);

		// Calculate the scaling factors
		double scale_x = (double)width / svg_dimensions.width;
		double scale_y = (double)height / svg_dimensions.height;

		// Create a SDL surface with the desired dimensions
		SDL_Surface * surface = SDL_CreateRGBSurface(SDL_SRCALPHA, width, height, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

		// We managed to create a SDL surface with the desired dimensions
		if (surface != NULL)
		{
			// Create a Cairo surface from the SDL surface
			cairo_surface_t * cairo_surface = cairo_image_surface_create_for_data(surface->pixels, CAIRO_FORMAT_ARGB32, surface->w, surface->h, surface->pitch);

			// We managed to create a Cairo surface from the SDL surface
			if (cairo_surface != NULL)
			{
				// Create Cairo rendering context
				cairo_t* cairo_rendering_context = cairo_create(cairo_surface);

				// We managed to create a Cairo render context
				if (cairo_rendering_context != NULL)
				{
					// Scale the Cairo rendering context
					cairo_scale(cairo_rendering_context, scale_x, scale_y);

					// We've been given a default fill color
					if (default_fill_color != NULL)
					{
						// Convert the Uint32 color to RGB components
						Uint8 alpha = (*default_fill_color & 0xFF000000) >> 24;
						Uint8 red = (*default_fill_color & 0xFF0000) >> 16;
						Uint8 green = (*default_fill_color & 0x00FF00) >> 8;
						Uint8 blue = *default_fill_color & 0x0000FF;

						// Convert color values to the CSS color format
						char color[32];
						snprintf(color, sizeof(color), "rgb(%d,%d,%d)", red, green, blue, alpha);

						// Generate the CSS rule for setting the fill color
						char css_rule[64];
						snprintf(css_rule, sizeof(css_rule), "svg { fill: %s; }", color);

						// Set the CSS stylesheet with the fill color
						rsvg_handle_set_stylesheet(svg_handle, (const guint8*)css_rule, strlen(css_rule), NULL);
					}

					// Render SVG onto SDL surface
					if (rsvg_handle_render_cairo(svg_handle, cairo_rendering_context))
					{
						// We managed to render the SVG into the SDL surface
						result = surface;
					}

					// Destroy the Cairo render context
					cairo_destroy(cairo_rendering_context);
				}

				// Destroy the Cairo surface
				cairo_surface_destroy(cairo_surface);
			}

			// We failed to render the SVG into the SDL surface
			if (result == NULL)
			{
				// Free the SDL surface
				SDL_FreeSurface(surface);
			}
		}

		// Free the SVG handle
		g_object_unref(svg_handle);
	}

	// Return the rendered SDL surface
	return result;
}

/**
 * @brief Converts the given surface into a 32bit RGBA surface.
 */
SDL_Surface * SDL_ConvertToRGBASurface(SDL_Surface * surface, int free_input_surface)
{
	// The converted surface
	SDL_Surface * convertedSurface = NULL;

	// We've been given a valid input surface
	if (surface != NULL)
	{
		// Create a new surface with the desired format
		convertedSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, surface->w, surface->h, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);

		// We managed to create a new surface
		if (convertedSurface != NULL)
		{
			// We failed to blit the original surface onto the new surface
			if (SDL_BlitSurface(surface, NULL, convertedSurface, NULL) != 0)
			{
				// Free the new surface
				SDL_FreeSurface(convertedSurface);

				// And NULL its reference
				convertedSurface = NULL;
			}
		}

		// We were asked to free the input surface
		if (free_input_surface)
		{
			// Free the input surface
			SDL_FreeSurface(surface);
		}
	}

	// Return the converted surface
	return convertedSurface;
}

/**
 * @brief Applies a vertical fade effect to the given surface.
 */
void SDL_VerticalFade(SDL_Surface * surface, double fadePercentage, double transparentPercentage)
{
	// This surface requires locking to access its pixels
	if (SDL_MUSTLOCK(surface))
	{
		// Lock the surface
		if (SDL_LockSurface(surface) != 0)
		{
			// We failed to lock the surface
			return;
		}
	}

	// Cast the surface pixels
	Uint32 * pixels = (Uint32 *)surface->pixels;

	// Calculate the number of rows for the fade effect
	int fadeStartRow = surface->h - (surface->h * fadePercentage);
	int fadeEndRow = surface->h - (surface->h * transparentPercentage);

	// Iterate through the rows to apply the fade effect
	for (int y = fadeStartRow; y < surface->h; y++)
	{
		// Get the address of the current row
		Uint32* row = pixels + y * (surface->pitch / sizeof(Uint32));

		// Calculate the alpha value for the current row
		Uint8 alpha = y < fadeEndRow ? (Uint8)((1.0 - (double)(y - fadeStartRow) / (double)(fadeEndRow - fadeStartRow)) * 255) : (Uint8)0;

		// Iterate through the pixels in the row
		for (int x = 0; x < surface->w; x++)
		{
			// Get the pixel value at the current position
			Uint32 pixel = row[x];

			// Set the alpha value of the pixel
			pixel = (pixel & 0x00FFFFFF) | (alpha << 24);

			// Update the pixel value in the row
			row[x] = pixel;
		}
	}

	// This surface requires locking to access its pixels
	if (SDL_MUSTLOCK(surface))
	{
		// Unlock the surface
		SDL_UnlockSurface(surface);
	}
}

/**
 * @brief Renders an anti-aliased filled circle.
 */
void SDL_aaFilledCircleRGBA(SDL_Surface * surface, Sint16 x, Sint16 y, Sint16 radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	// Cast the surface pixels
    Uint32 * pixels = (Uint32 *)surface->pixels;

	// Calculate the pitch in pixels
    int pitch = surface->pitch / sizeof(Uint32);

	// Iterate the rows
    for (int dy = -radius; dy <= radius; dy++)
	{
		// Iterate the row pixels
        for (int dx = -radius; dx <= radius; dx++)
		{
			// Calculate the distance to the circle center point
            float distance = sqrtf(dx * dx + dy * dy);

			// Clamp distance to radius
            if (distance <= radius)
			{
				// Calculate the pixel alpha value (used to anti-alias the outer ring)
                float alpha = (distance >= radius - 1.0f) ? 1.0f - (distance - (radius - 1.0f)) : 1.0f;

				// Calculate the pixel position
                int px = x + dx;
                int py = y + dy;

				// Clamp pixel position to the surface dimensions
                if (px >= 0 && px < surface->w && py >= 0 && py < surface->h)
				{
					// Calculate the pixel index
                    int index = py * pitch + px;

					// Grab the current pixel value
                    Uint32 pixel = pixels[index];

					// Blend the current pixel and the new pixel value together
                    Uint8 pr, pg, pb, pa;
                    SDL_GetRGBA(pixel, surface->format, &pr, &pg, &pb, &pa);
                    Uint8 newR = (Uint8)((alpha * r) + ((1 - alpha) * pr));
                    Uint8 newG = (Uint8)((alpha * g) + ((1 - alpha) * pg));
                    Uint8 newB = (Uint8)((alpha * b) + ((1 - alpha) * pb));
                    Uint8 newA = (Uint8)a;

					// Write back the blended pixel
                    pixels[index] = SDL_MapRGBA(surface->format, newR, newG, newB, newA);
                }
            }
        }
    }
}