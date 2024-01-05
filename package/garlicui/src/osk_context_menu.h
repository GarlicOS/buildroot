#ifndef OSK_CONTEXT_MENU_H
#define OSK_CONTEXT_MENU_H

#include <unicode/ustring.h>

#include "osk.h"
#include "io.h"

// The current scroll repeat interval
static Uint32 scroll_repeat_interval = SCROLL_EVENT_INITIAL_REPEAT_INTERVAL;

/**
 * @brief Gets the Unicode glyph for the given UTF-8 string.
 */
static Uint16 osk_get_glyph(const char * utf8_str)
{
	UChar u_str[1];
	UErrorCode errorCode = U_ZERO_ERROR;

	u_strFromUTF8(u_str, sizeof(u_str) / sizeof(u_str[0]), NULL, utf8_str, -1, &errorCode);

	if (U_FAILURE(errorCode)) {
		fprintf(stderr, "Error converting UTF-8 to Unicode: %s\n", u_errorName(errorCode));
		exit(EXIT_FAILURE);
	}

	return u_str[0];
}

/**
 * @brief Renders the on-screen keyboard key surface.
 */
static SDL_Surface * osk_render_key(struct gui_context * context, const char * key, int index)
{
	SDL_Surface * text_surface;

	if (TTF_GlyphIsProvided(context->fonts.small, osk_get_glyph(key)) > 0)
	{
		text_surface = TTF_RenderUTF8_Blended(context->fonts.small, key, SDL_ToSDLColor(context->colors.osk.foreground));
	}
	else
	{
		char * fallback = malloc(strlen(key) + 4 + 1);
		strcpy(fallback, key);
		strcat(fallback, ".svg");
		text_surface = icon_load(fallback, 20 * context->surfaces.scale_factor, 20 * context->surfaces.scale_factor, &context->colors.osk.foreground);
		free(fallback);
		if (text_surface == NULL)
		{
			return NULL;
		}
	}

	SDL_Surface * surface = context->surfaces.overlays.osk.surface;

	int x = surface->w / OSK_COLS_COUNT;
	int y = surface->h / OSK_ROWS_COUNT;

	// Render the description text onto the surface
	SDL_Rect text_position;
	text_position.x = x * (index % OSK_COLS_COUNT) + ((x - text_surface->w) >> 1);
	text_position.y = y * (index / OSK_COLS_COUNT | 0) + ((y - text_surface->h) >> 1);
	text_position.w = text_surface->w;
	text_position.h = text_surface->h;
	SDL_BlitSurfaceAlpha(text_surface, NULL, surface, &text_position);

	// Free the description text surface
	SDL_FreeSurface(text_surface);

	return surface;
}

/**
 * @brief Renders the on-screen keyboard grid.
 */
static void osk_render_grid(struct gui_context * context)
{
	const char ** grid = osk_grids[context->osk.grid];

    for (int i = 0, n = OSK_GRID_SIZE; i < n; i++)
    {
        osk_render_key(context, grid[i], i);
    }
}

/**
 * @brief Modulo operation that always returns a positive result.
 */
static int osk_mod(int a, int b)
{
   int result = a % b;
   return result < 0 ? result + b : result;
}

/**
 * @brief Compute the on-screen keyboard index
 */
static int osk_index(int index, int x, int y)
{
	if (x != 0)
	{
		int delta = OSK_COLS_COUNT * (index / OSK_COLS_COUNT | 0);

		index = osk_mod(index + (x < 0 ? -1 : 1) - delta, OSK_COLS_COUNT) + delta;
	}

	if (y != 0)
	{
		index = osk_mod(index + (y < 0 ? - OSK_COLS_COUNT : OSK_COLS_COUNT), OSK_GRID_SIZE);
	}

	return index;
}

/**
 * @brief Initializes the on-screen keyboard menu.
 */
static int osk_initialize_menu(struct gui_context * context)
{
	// The RGB color components
	Uint8 r, g, b;

	// Set the on-screen keyboard background color
	GET_RGB_COMPONENTS(context->colors.osk.background, r, g, b);
	context->colors.osk.background = SDL_MapRGB(context->surfaces.screen->format, r, g, b);

	// Set the on-screen keyboard foreground color
	GET_RGB_COMPONENTS(context->colors.osk.foreground, r, g, b);
	context->colors.osk.foreground = SDL_MapRGB(context->surfaces.screen->format, r, g, b);

	// Render the on-screen keyboard overlay
	context->surfaces.overlays.osk.surface = icon_rounded_rectangle(context->surfaces.screen->w, context->surfaces.screen->h >> 1, 1, context->colors.osk.background);

	// We failed to render the on-screen keyboard overlay
	if (context->surfaces.overlays.osk.surface == NULL)
	{
		return -1;
	}

	// Initialize the on-screen keyboard grid index
	context->osk.grid = OSK_GRID_COUNT - 1;

	// Render the on-screen keyboard grids
	osk_render_grid(context);

	// Render the on-screen keyboard active key overlay
	context->surfaces.overlays.osk_active_key.surface = icon_load("rect.svg", context->surfaces.screen->w / OSK_COLS_COUNT, (context->surfaces.screen->h >> 1) / OSK_ROWS_COUNT, &context->colors.osk.foreground);

	// We failed to render the on-screen keyboard active key overlay
	if (context->surfaces.overlays.osk_active_key.surface == NULL)
	{
		return -1;
	}

	// Calculate the on-screen keyboard overlay position
	context->surfaces.overlays.osk.position.x = 0;
	context->surfaces.overlays.osk.position.y = context->surfaces.screen->h - context->surfaces.overlays.osk.surface->h;
	context->surfaces.overlays.osk.position.w = context->surfaces.overlays.osk.surface->w;
	context->surfaces.overlays.osk.position.h = context->surfaces.overlays.osk.surface->h;

	// Calculate the on-screen keyboard active key overlay position
	context->surfaces.overlays.osk_active_key.position.x = 0;
	context->surfaces.overlays.osk_active_key.position.y = 0;
	context->surfaces.overlays.osk_active_key.position.w = context->surfaces.overlays.osk_active_key.surface->w;
	context->surfaces.overlays.osk_active_key.position.h = context->surfaces.overlays.osk_active_key.surface->h;

	// Calculate the on-screen keyboard buffer overlay position
	context->surfaces.overlays.osk_buffer.position.x = 0;
	context->surfaces.overlays.osk_buffer.position.y = context->surfaces.screen->h >> 2;
	context->surfaces.overlays.osk_buffer.position.w = context->surfaces.screen->w;
	context->surfaces.overlays.osk_buffer.position.h = context->surfaces.screen->h >> 1;

	return 0;
}

/**
 * @brief Configures the on-screen keyboard menu.
 */
static void osk_configure_menu(struct gui_context * context, xmlDocPtr document)
{
	context->colors.osk.background = 0x000000;
	context->colors.osk.foreground = 0xffffff;

	if (document != NULL)
	{
		// Get the on-screen keyboard background color
		context->colors.osk.background = io_get_skin_color(document, "Keyboard", "Background", 0x000000);

		// Get the on-screen keyboard foreground color
		context->colors.osk.foreground = io_get_skin_color(document, "Keyboard", "Foreground", 0xffffff);
	}
}

/**
 * @brief Activates the on-screen keyboard menu.
 */
static void osk_activate_menu(struct gui_node * this)
{
	// Get the on-screen keyboard menu node data
	struct gui_osk_menu_node_data * osk_menu_node_data = (struct gui_osk_menu_node_data *)this->data;

	// Render the title text surface
	gui_activate_menu(this, osk_menu_node_data->title);

	// Reset the on-screen keyboard grid index
	this->context->osk.grid = OSK_GRID_COUNT - 1;

	// Lock the on-screen keyboard if buttons are pressed
	this->context->osk.lock = this->context->inputs.internal.current.east || this->context->inputs.internal.current.south;
	int lock = this->context->osk.lock;

	// Reset the on-screen keyboard sync flag
	this->context->osk.sync = true;

	// Reset the on-screen keyboard apply flag
	this->context->osk.apply = false;

	// Initialize the on-screen keyboard buffer
	this->context->osk.buffer = malloc(osk_menu_node_data->buffer != NULL ? strlen(osk_menu_node_data->buffer) : sizeof(char));

	// We have a buffer to copy
	if (osk_menu_node_data->buffer != NULL)
	{
		// Copy the on-screen keyboard buffer
		strcpy(this->context->osk.buffer, osk_menu_node_data->buffer);
	}

	// Make this node take focus on screen
	this->context->menu.active = this;
}

/**
 * @brief Deactivates the on-screen keyboard menu.
 */
static void osk_deactivate_menu(struct gui_node * this)
{
	// The menu has been activated (aka. its in focus, this check is necessary because deactivate gets called on parent node disposal as well)
	if (this->context->menu.active == this)
	{
		// Get the on-screen keyboard menu node data
		struct gui_osk_menu_node_data * osk_menu_node_data = (struct gui_osk_menu_node_data *)this->data;

		// We have an on-screen keyboard buffer to apply
		if (this->context->osk.apply)
		{
			// Save the on-screen keyboard buffer
			osk_menu_node_data->buffer = strdup(this->context->osk.buffer);
		}

		// Free the on-screen keyboard buffer
		free(this->context->osk.buffer);

		// NULL the on-screen keyboard buffer reference
		this->context->osk.buffer = NULL;

		// We have a parent node to return to
		if (this->parent != NULL)
		{
			// And return on screen focus to the parent node
			this->context->menu.active = this->parent;
		}
	}
}

/**
 * @brief Updates the on-screen keyboard menu.
 */
static void osk_update_menu(struct gui_node * this)
{
	// The current state
	static int state = 0;

	// The time of the last state change
	static Uint32 last_state_change = 0;

	// The time of the last scroll event
	static Uint32 last_scroll_event = 0;

	// The current on-screen keyboard index
	int index = this->context->osk.index;

	// The current on-screen keyboard grid
	int grid = this->context->osk.grid;

	// The current on-screen keyboard buffer
	char * buffer = strdup(this->context->osk.buffer);

	// The east-facing face button is pressed
	bool add = this->context->inputs.internal.current.east;

	// The south-facing face button is pressed
	bool delete = this->context->inputs.internal.current.south;

	// The left bumper button is pressed
	bool previous_grid = this->context->inputs.internal.current.left_bumper;

	// The right bumper button is pressed
	bool next_grid = this->context->inputs.internal.current.right_bumper;

	// The select button was pressed
	bool clear = this->context->inputs.internal.previous.select != this->context->inputs.internal.current.select && this->context->inputs.internal.current.select;

	// The start button was pressed
	bool apply = this->context->inputs.internal.previous.start != this->context->inputs.internal.current.start && this->context->inputs.internal.current.start;

	// The on-screen keyboard is locked and buttons are released
	if (this->context->osk.lock && this->context->inputs.internal.current.east == 0 && this->context->inputs.internal.current.south == 0)
	{
		// Unlock the on-screen keyboard
		this->context->osk.lock = false;
	}

	int new_state = 0;

	// The on-screen keyboard is unlocked
	if (!this->context->osk.lock)
	{
		if (new_state == 0)
		{
			// Try using the horizontal direction pad axis
			new_state = this->context->inputs.internal.current.dpad_x;
		}

		// The horizontal direction pad axis is in its center-state
		if (new_state == 0)
		{
			// Try using the vertical direction pad axis
			new_state = this->context->inputs.internal.current.dpad_y;
		}

		// The vertical direction pad axis is in its center-state
		if (new_state == 0)
		{
			// Try using the east-facing face button
			new_state = this->context->inputs.internal.current.east;
		}

		// The east-facing face button is in its unpressed-state
		if (new_state == 0)
		{
			// Try using the south-facing face button
			new_state = this->context->inputs.internal.current.south;
		}

		// The south-facing face button is in its unpressed-state
		if (new_state == 0)
		{
			// Try using the left bumper button
			new_state = this->context->inputs.internal.current.left_bumper;
		}

		// The left bumper button is in its unpressed-state
		if (new_state == 0)
		{
			// Try using the right bumper button
			new_state = this->context->inputs.internal.current.right_bumper;
		}
	}

	// The state has changed
	if (state != new_state)
	{
		// Override the current state
		state = new_state;

		// Keep track of when the state changed
		last_state_change = SDL_GetTicks();

		// Reset the repeat event timer
		last_scroll_event = 0;

		// Reset the scroll repeat interval
		scroll_repeat_interval = SCROLL_EVENT_INITIAL_REPEAT_INTERVAL;
	}

	// We're still scrolling in the same state and have room for acceleration
	else if (state != 0 && scroll_repeat_interval > SCROLL_EVENT_MINIMUM_REPEAT_INTERVAL)
	{
		// Calculate the acceleration multiplier
		Uint32 acceleration_multiplier = (SDL_GetTicks() - last_state_change) / SCROLL_EVENT_ACCELERATION_TIMESPAN;

		// Calculate the new scroll repeat interval
		int new_scroll_repeat_interval = SCROLL_EVENT_INITIAL_REPEAT_INTERVAL - (SCROLL_EVENT_ACCELERATION * acceleration_multiplier);

		// Set the new scroll repeat interval
		scroll_repeat_interval = new_scroll_repeat_interval > SCROLL_EVENT_MINIMUM_REPEAT_INTERVAL ? new_scroll_repeat_interval : SCROLL_EVENT_MINIMUM_REPEAT_INTERVAL;
	}

	// We fulfill all scroll requirements
	if (state != 0 && (last_scroll_event == 0 || (SDL_GetTicks() - last_scroll_event) > scroll_repeat_interval))
	{
		// The direction pad axis is pressed
		if (this->context->inputs.internal.current.dpad_x != 0 || this->context->inputs.internal.current.dpad_y != 0)
		{
			// Compute the new on-screen keyboard index
			this->context->osk.index = osk_index(this->context->osk.index, this->context->inputs.internal.current.dpad_x, this->context->inputs.internal.current.dpad_y);
		}

		if (add)
		{
			const char * key = osk_grids[this->context->osk.grid][this->context->osk.index];

			if (strcmp(key, "⇦") == 0)
			{
				delete = true;
			}
			else if (strcmp(key, "⇧") == 0)
			{
				this->context->osk.grid = 1;
			}
			else if (strcmp(key, "⇩") == 0)
			{
				this->context->osk.grid = 2;
			}
			else if (strcmp(key, "⊕") == 0)
			{
				this->context->osk.grid = 0;
			}
			else if (strcmp(key, "⏎") == 0)
			{
				apply = true;
			}
			else
			{
				size_t size = strlen(this->context->osk.buffer);
				char * resized_buffer = realloc(this->context->osk.buffer, sizeof(char) * (size + 1));
				resized_buffer[size] = *key;
				resized_buffer[size + 1] = '\0';
				this->context->osk.buffer = resized_buffer;
			}
		}

		if (delete)
		{
			size_t size = strlen(this->context->osk.buffer);
			if (size > 0)
			{
				char * resized_buffer = realloc(this->context->osk.buffer, sizeof(char) * (size - 1));
				resized_buffer[size - 1] = '\0';
				this->context->osk.buffer = resized_buffer;
			}
		}

		if (previous_grid)
		{
			this->context->osk.grid = osk_mod(this->context->osk.grid - 1, OSK_GRID_COUNT);
		}

		if (next_grid)
		{
			this->context->osk.grid = osk_mod(this->context->osk.grid + 1, OSK_GRID_COUNT);
		}

		// Set the scroll event timestamp
		last_scroll_event = SDL_GetTicks();
	}

	if (clear)
	{
		// Free the on-screen keyboard buffer
		free(this->context->osk.buffer);

		// Get the on-screen keyboard menu node data
		struct gui_osk_menu_node_data * osk_menu_node_data = (struct gui_osk_menu_node_data *)this->data;

		// Initialize the on-screen keyboard buffer
		this->context->osk.buffer = malloc(osk_menu_node_data->buffer != NULL ? strlen(osk_menu_node_data->buffer) : sizeof(char));
	}

	if (apply)
	{
		// Apply the on-screen keyboard buffer
		this->context->osk.apply = true;
	}

	// The current on-screen keyboard index has changed or the grid has changed
	if (index != this->context->osk.index || grid != this->context->osk.grid)
	{
		// We are holding onto a no longer valid on-screen keyboard overlay
		if (this->context->surfaces.overlays.osk.surface != NULL)
		{
			// Free the on-screen keyboard overlay surface
			SDL_FreeSurface(this->context->surfaces.overlays.osk.surface);

			// And NULL its reference so it can be re-rendered
			this->context->surfaces.overlays.osk.surface = NULL;
		}
	}

	// The current on-screen keyboard index has changed
	if (index != this->context->osk.index)
	{
		// We are holding onto a no longer valid on-screen keyboard active key overlay
		if (this->context->surfaces.overlays.osk_active_key.surface != NULL)
		{
			// Free the on-screen keyboard active key overlay surface
			SDL_FreeSurface(this->context->surfaces.overlays.osk_active_key.surface);

			// And NULL its reference so it can be re-rendered
			this->context->surfaces.overlays.osk_active_key.surface = NULL;
		}
	}

	// The current on-screen keyboard buffer has changed or we need to sync the on-screen keyboard
	if (strcmp(buffer, this->context->osk.buffer) != 0 || this->context->osk.sync)
	{
		// We are holding onto a no longer valid on-screen keyboard buffer overlay
		if (this->context->surfaces.overlays.osk_buffer.surface != NULL)
		{
			// Free the on-screen keyboard buffer overlay surface
			SDL_FreeSurface(this->context->surfaces.overlays.osk_buffer.surface);

			// And NULL its reference so it can be re-rendered
			this->context->surfaces.overlays.osk_buffer.surface = NULL;
		}
	}

	// Sync the on-screen keyboard once
	if (this->context->osk.sync)
	{
		// Reset the on-screen keyboard sync flag
		this->context->osk.sync = false;
	}

	// Verify if we should apply changes
	if (this->context->osk.apply)
	{
		// Deactivate the on-screen keyboard menu
		osk_deactivate_menu(this);
	}
}

/**
 * @brief Renders the on-screen keyboard menu.
 */
static void osk_render_menu(struct gui_node * this)
{
	// Cast the type specific node data
	struct gui_menu_node_data * data = (struct gui_menu_node_data *)this->data;

	// We have a valid menu node data structure
	if (data != NULL)
	{
		// Blit the menu title surface
		SDL_BlitSurface(data->title.surface, NULL, this->context->surfaces.screen, &data->title.position);
	}

	// We need to re-render the on-screen keyboard
	if (this->context->surfaces.overlays.osk.surface == NULL)
	{
		// Render the on-screen keyboard
		this->context->surfaces.overlays.osk.surface = icon_rounded_rectangle(this->context->surfaces.screen->w, this->context->surfaces.screen->h >> 1, 1, this->context->colors.osk.background);

		// We managed to render the on-screen keyboard
		if (this->context->surfaces.overlays.osk.surface != NULL)
		{
			// Render the on-screen keyboard grids
			osk_render_grid(this->context);
		}
	}

	// We need to re-render the on-screen keyboard active key
	if (this->context->surfaces.overlays.osk_active_key.surface == NULL)
	{
		// Render the on-screen keyboard active key
		this->context->surfaces.overlays.osk_active_key.surface = icon_load("rect.svg", this->context->surfaces.screen->w / OSK_COLS_COUNT, (this->context->surfaces.screen->h >> 1) / OSK_ROWS_COUNT, &this->context->colors.osk.foreground);

		// We managed to render the on-screen keyboard active key
		if (this->context->surfaces.overlays.osk_active_key.surface != NULL)
		{
			this->context->surfaces.overlays.osk_active_key.position.x = (this->context->osk.index % OSK_COLS_COUNT) * this->context->surfaces.overlays.osk_active_key.surface->w;
			this->context->surfaces.overlays.osk_active_key.position.y = (this->context->osk.index / OSK_COLS_COUNT | 0) * this->context->surfaces.overlays.osk_active_key.surface->h;
			this->context->surfaces.overlays.osk_active_key.position.w = this->context->surfaces.overlays.osk_active_key.surface->w;
			this->context->surfaces.overlays.osk_active_key.position.h = this->context->surfaces.overlays.osk_active_key.surface->h;
		}
	}

	// We need to re-render the on-screen keyboard buffer
	if (this->context->surfaces.overlays.osk_buffer.surface == NULL)
	{
		// Render the on-screen keyboard buffer
		this->context->surfaces.overlays.osk_buffer.surface = TTF_RenderUTF8_Blended(this->context->fonts.small, this->context->osk.buffer, SDL_ToSDLColor(this->context->colors.osk.foreground));

		// We managed to render the on-screen keyboard buffer
		if (this->context->surfaces.overlays.osk_buffer.surface != NULL)
		{
			this->context->surfaces.overlays.osk_buffer.position.x = (this->context->surfaces.screen->w >> 1) - (this->context->surfaces.overlays.osk_buffer.surface->w >> 1);
		}
	}

	// We need to render the on-screen keyboard
	if (this->context->surfaces.overlays.osk.surface != NULL)
	{
		// Render the on-screen keyboard
		SDL_BlitSurface(this->context->surfaces.overlays.osk.surface, NULL, this->context->surfaces.screen, &this->context->surfaces.overlays.osk.position);
	}

	// We need to render the on-screen keyboard active key
	if (this->context->surfaces.overlays.osk_active_key.surface != NULL)
	{
		// Render the on-screen keyboard active key
		SDL_BlitSurfaceAlpha(this->context->surfaces.overlays.osk_active_key.surface, NULL, this->context->surfaces.overlays.osk.surface, &this->context->surfaces.overlays.osk_active_key.position);
	}

	// We need to render the on-screen keyboard buffer
	if (this->context->surfaces.overlays.osk_buffer.surface != NULL)
	{
		// Render the on-screen keyboard buffer
		SDL_BlitSurface(this->context->surfaces.overlays.osk_buffer.surface, NULL, this->context->surfaces.screen, &this->context->surfaces.overlays.osk_buffer.position);
	}
}

/**
 * @brief Creates a on-screen keyboard context menu.
 */
static struct gui_node * osk_create_menu(struct gui_node * this, struct gui_node ** first_node, struct gui_node ** previous_node, const char * title, const char * buffer)
{
	// The created node
	struct gui_node * osk_menu_node = NULL;

	// Allocate the additional on-screen keyboard menu data
	struct gui_osk_menu_node_data * osk_menu_node_data = calloc(1, sizeof(struct gui_osk_menu_node_data));

	// We managed to allocate the additional on-screen keyboard menu data
	if (osk_menu_node_data != NULL)
	{
		// Assign the title
		osk_menu_node_data->title = title;

		// Assign the buffer
		osk_menu_node_data->buffer = buffer;

		// Create a node for the on-screen keyboard menu
		osk_menu_node = gui_create_context_menu_item(this, first_node, previous_node, title, osk_activate_menu, osk_deactivate_menu, osk_update_menu, osk_render_menu, NULL, NODE_TYPE_CONTEXT_MENU_OSK_MENU, osk_menu_node_data);

		// We failed to create a node for the on-screen keyboard menu
		if (osk_menu_node == NULL)
		{
			// Free the additional menu data
			free(osk_menu_node_data);
		}
	}

	// Return the created node
	return osk_menu_node;
}

#endif
