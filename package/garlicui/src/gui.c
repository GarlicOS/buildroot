#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <cJSON.h>

#include "gui.h"
#include "gui_main_menu.h"

/**
 * @brief Renders the select button legend surface.
 */
static SDL_Surface * gui_render_select_legend_surface(struct gui_context * context)
{
	// Convert the legend foreground color into a SDL color
	SDL_Color sdl_legend_foreground_color = SDL_ToSDLColor(context->colors.legend.foreground);

	// Render the select button legend surface
	return icon_circled_text(context->fonts.small, context->colors.legend.foreground, context->colors.legend.background, gettext("Select"), 0, (localization_font_height() + SCREEN_MARGIN) * context->surfaces.scale_factor, (SCREEN_MARGIN >> 1) * context->surfaces.scale_factor, SDL_TrimTransparent(TTF_RenderUTF8_Blended(context->fonts.normal, getenv("GARLIC_EAST_BUTTON_LABEL") != NULL ? getenv("GARLIC_EAST_BUTTON_LABEL") : "A", sdl_legend_foreground_color), 1), 1);
}

/**
 * @brief Renders the back button legend surface.
 */
static SDL_Surface * gui_render_back_legend_surface(struct gui_context * context)
{
	// Convert the legend foreground color into a SDL color
	SDL_Color sdl_legend_foreground_color = SDL_ToSDLColor(context->colors.legend.foreground);

	// Render the back button legend surface
	return icon_circled_text(context->fonts.small, context->colors.legend.foreground, context->colors.legend.background, gettext("Back"), 0, (localization_font_height() + SCREEN_MARGIN) * context->surfaces.scale_factor, (SCREEN_MARGIN >> 1) * context->surfaces.scale_factor, SDL_TrimTransparent(TTF_RenderUTF8_Blended(context->fonts.normal, getenv("GARLIC_SOUTH_BUTTON_LABEL") != NULL ? getenv("GARLIC_SOUTH_BUTTON_LABEL") : "B", sdl_legend_foreground_color), 1), 1);
}

/**
 * @brief Renders the menu button legend surface.
 */
static SDL_Surface * gui_render_menu_legend_surface(struct gui_context * context)
{
	// Convert the legend foreground color into a SDL color
	SDL_Color sdl_legend_foreground_color = SDL_ToSDLColor(context->colors.legend.foreground);

	// Render the menu button legend surface
	return icon_circled_text(context->fonts.small, context->colors.legend.foreground, context->colors.legend.background, gettext("Menu"), 0, (localization_font_height() + SCREEN_MARGIN) * context->surfaces.scale_factor, (SCREEN_MARGIN >> 1) * context->surfaces.scale_factor, icon_load("hamburger.svg", DEFAULT_BAR_ICON_HEIGHT * context->surfaces.scale_factor, DEFAULT_BAR_ICON_HEIGHT * context->surfaces.scale_factor, &context->colors.legend.foreground), 1);
}

/**
 * @brief Triggers the invalidate callback.
 */
static void gui_invalidate(struct gui_node * node)
{
	// We've been given a valid node
	if (node != NULL)
	{
		// Create an iterator for the child nodes
		struct gui_node * child = node->child;

		// We have at least one child
		if (child != NULL)
		{
			// Iterate all child nodes
			do
			{
				// Update the child node's locale as well
				gui_invalidate(child);

				// Move to the next child node
				child = child->next;
			} while (child != node->child);
		}

		// Update the node's locale
		node->invalidate(node);
	}
}

/**
 * @brief Returns the selected child node's index.
 */
static int gui_get_child_node_index(struct gui_node * parent)
{
	// The child node index
	int index = 0;

	// Iterate the child nodes until we hit the currently selected child
	for (struct gui_node * node = parent->children; node != NULL && node != parent->child; node = node->next)
	{
		// Increment the child node index
		index++;
	}

	// Return the child node index
	return index;
}

/**
 * @brief Returns the first child node that matches the given type.
 */
static struct gui_node * gui_get_first_child_node_by_type(struct gui_node * parent, int type)
{
	// The child node
	struct gui_node * result = NULL;

	// We've been given a valid parent node
	if (parent != NULL)
	{
		// Start at the first child node
		struct gui_node * child = parent->children;

		// We have at least one child node
		if (child != NULL)
		{
			// Iterate all child nodes
			do
			{
				// The child node type matches
				if (child->type == type)
				{
					// Return the child node
					result = child;

					// No use iterating the rest
					break;
				}

				// Move to the next child node
				child = child->next;
			} while (child != parent->children);
		}
	}

	// Return the child node
	return result;
}

/**
 * @brief Returns the first child node that matches the given path.
 */
static struct gui_node * gui_get_first_child_node_by_path(struct gui_node * parent, const char * path)
{
	// The child node
	struct gui_node * result = NULL;

	// We've been given a valid parent node
	if (parent != NULL)
	{
		// Start at the first child node
		struct gui_node * child = parent->children;

		// We have at least one child node
		if (child != NULL)
		{
			// Iterate all child nodes
			do
			{
				// Cast the child node's path data
				struct gui_path_menu_node_data * data = (struct gui_path_menu_node_data *)child->data;

				// The child node type matches
				if (strcmp(data->path, path) == 0)
				{
					// Return the child node
					result = child;

					// No use iterating the rest
					break;
				}

				// Move to the next child node
				child = child->next;
			} while (child != parent->children);
		}
	}

	// Return the child node
	return result;
}

/**
 * @brief Get's the nth sibling of the given node.
 */
struct gui_node * gui_get_nth_item(struct gui_node * node, int index)
{
	// Iterate up to the given index
	for (int i = 0; i < index && node != NULL; i++)
	{
		// Move to the next node
		node = node->next;
	}

	// Return the node
	return node;
}

/**
 * @brief Serializes the current UI state.
 *
 * The caller is responsible for freeing the returned buffer.
 */
static char * gui_serialize_ui_state(struct gui_context * context)
{
	// The serialized UI state
	char * ui_state = NULL;

	// Start at the root GUI node
	struct gui_node * node = context->menu.root;

	// Iterate the GUI tree
	for (struct gui_node * node = context->menu.root; node != NULL; node = node->child)
	{
		// The node state
		char node_state[64 + PATH_MAX];

		// We're handling with a path based menu node
		if (node->type == NODE_TYPE_LIBRARY_DIRECTORY_MENU || node->type == NODE_TYPE_LIBRARY_FILE)
		{
			// Cast the child node's path data
			struct gui_path_menu_node_data * data = (struct gui_path_menu_node_data *)node->data;

			// Populate the node state
			sprintf(node_state, "%d:%s", node->type, data->path);
		}

		// We're handling a index based menu node
		else
		{
			// Populate the node state
			sprintf(node_state, "%d:%d", node->type, gui_get_child_node_index(node));
		}

		// Resize the UI state buffer
		char * resized_ui_state = realloc(ui_state, (ui_state == NULL ? 0 : (strlen(ui_state) + 1)) + strlen(node_state) + 1);

		// We managed to resize the UI state buffer
		if (resized_ui_state != NULL)
		{
			// We're adding another menu to the state
			if (ui_state != NULL)
			{
				// Put in a separator
				strcat(resized_ui_state, ",");
			}

			// We're adding the first menu to the state
			else
			{
				// Terminate the buffer
				*resized_ui_state = 0;
			}

			// Append the node state
			strcat(resized_ui_state, node_state);

			// Replace the UI state pointer
			ui_state = resized_ui_state;
		}
	}

	// Return the serialized UI state
	return ui_state;
}

/**
 * @brief Deserializes the given UI state.
 */
static void gui_restore_ui_state(struct gui_context * context, const char * ui_state)
{
	// Copy the UI state buffer
	char * copy = strdup(ui_state);

	// We managed to copy the UI state buffer
	if (copy != NULL)
	{
		// The "," split save pointer
		char * saveptr1 = NULL;

		// The ":" split save pointer
		char * saveptr2 = NULL;

		// Split the string by ","
		char * token = strtok_r(copy, ",", &saveptr1);

		// Iterate all "," split tokens
		while (token != NULL)
		{
			// Copy the "," split token buffer
			char * pair = strdup(token);

			// We managed to copy the "," split token buffer
			if (pair != NULL)
			{
				// Split each pair by ":" to get the node type and child index
				char * node_type_text = strtok_r(pair, ":", &saveptr2);
				char * child_index_text = strtok_r(NULL, ":", &saveptr2);

				// We managed to acquire the node type and child index
				if (node_type_text != NULL && child_index_text != NULL)
				{
					// Parse the node type and child index
					int node_type = atoi(node_type_text);
					int child_index = atoi(child_index_text);

					// We need to move to another node
					if (context->menu.active->type != node_type)
					{
						// The to-be-selected child node
						struct gui_node * child_node = NULL;

						// We're handling a path based menu
						if (node_type == NODE_TYPE_LIBRARY_DIRECTORY_MENU || node_type == NODE_TYPE_LIBRARY_FILE)
						{
							// Find the child node by its path
							child_node = gui_get_first_child_node_by_path(context->menu.active, child_index_text);
						}

						// We're handling a menu who's selection can be deserialized from its type
						else
						{
							// Find the child node by its node type
							child_node = gui_get_first_child_node_by_type(context->menu.active, node_type);
						}

						// We found a matching child node
						if (child_node != NULL)
						{
							// Set it as the selected node
							context->menu.active->child = child_node;

							// The child node has an activation callback
							if (child_node->activate != NULL)
							{
								// And isn't a content node
								if (node_type != NODE_TYPE_LIBRARY_FILE && node_type != NODE_TYPE_RETROARCH_MENU)
								{
									// Activate the child node
									child_node->activate(child_node);
								}
							}
						}
					}
				}

				// Free the ":" split token copy buffer
				free(pair);
			}

			// Fetch the next ":" split token
			token = strtok_r(NULL, ",", &saveptr1);
		}

		// Free the UI state copy buffer
		free(copy);
	}
}

/**
 * @brief Creates a new GUI context.
 *
 * This function allocates and initializes a new GUI context structure.
 * The generated context object must be freed explicitly via gui_destroy_context.
 */
struct gui_context * gui_create_context(int argc, char * argv[])
{
	// Unpack the library template and system icons (for themers)
	io_unpack_resources();

	// Allocate memory for the context
	struct gui_context * context = (struct gui_context *)calloc(1, sizeof(struct gui_context));

	// The additional main menu data
	struct gui_menu_node_data * main_menu_node_data = NULL;

	// We managed to allocate enough memory
	if (context != NULL)
	{
		// Restore the settings
		gui_read_configuration(context);

		// Garlic has been started with arguments
		if (argc > 1)
		{
			// RetroArch has asked us to cleanly shutdown the system
			if (strcmp(argv[1], "shutdown") == 0)
			{
				// Set the resume flag
				context->settings.resume = 1;

				// Write the configuration to disk
				gui_write_configuration(context);

				// Shutdown the device
				io_shutdown();

				// Kill the process (should we survive this long)
				exit(0);
			}
		}

		// Garlic has been started without arguments
		else
		{
			// We have the resume flag set
			if (context->settings.resume)
			{
				// Erase the resume flag
				context->settings.resume = 0;

				// Write the configuration to disk
				gui_write_configuration(context);

				// Try resuming the last played game
				retroarch_resume_last_game(context);
			}
		}

		// Initialize SDL
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0)
		{
			// We failed to initialize SDL
			goto free_context;
		}

		// Initialize SDL_img
		int expected_file_format_support = IMG_INIT_JPG | IMG_INIT_PNG;
		if ((IMG_Init(expected_file_format_support) & expected_file_format_support) != expected_file_format_support)
		{
			// We failed to initialize SDL_img
			goto quit_sdl;
		}

		// Initialize SDL_ttf
		if (TTF_Init() != 0)
		{
			// We failed to initialize SDL_ttf
			goto quit_img;
		}

		// Hide the mouse cursor
		SDL_ShowCursor(SDL_DISABLE);

		// Get the primary monitor's native screen resolution and bit-depth
		const SDL_VideoInfo * info = SDL_GetVideoInfo();

		// We failed to get the primary monitor's native screen resolution and bit-depth
		if (info == NULL)
		{
			// We can't continue without a known safe to use resolution
			goto quit_ttf;
		}

		// Create a 32bpp double-buffered screen surface
		context->surfaces.screen = SDL_SetVideoMode(info->current_w, info->current_h, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);

		// If 32bpp isn't available...
		if (context->surfaces.screen == NULL)
		{
			// Try going with whatever SDL reports to us (even if we know this won't work on Spreadtrum devices)
			context->surfaces.screen = SDL_SetVideoMode(info->current_w, info->current_h, info->vfmt->BitsPerPixel, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
		}

		// We failed to create a double-buffered screen surface
		if (context->surfaces.screen == NULL)
		{
			// We can't continue without a screen surface
			goto quit_ttf;
		}

		// The RGB color components
		Uint8 r, g, b;

		// Set the background color
		GET_RGB_COMPONENTS(context->colors.base.background, r, g, b);
		context->colors.base.background = SDL_MapRGB(context->surfaces.screen->format, r, g, b);

		// Set the foreground color
		GET_RGB_COMPONENTS(context->colors.base.foreground, r, g, b);
		context->colors.base.foreground = SDL_MapRGB(context->surfaces.screen->format, r, g, b);

		// Set the legend icon background color
		GET_RGB_COMPONENTS(context->colors.legend.background, r, g, b);
		context->colors.legend.background = SDL_MapRGB(context->surfaces.screen->format, r, g, b);

		// Set the legend icon foreground color
		GET_RGB_COMPONENTS(context->colors.legend.foreground, r, g, b);
		context->colors.legend.foreground = SDL_MapRGB(context->surfaces.screen->format, r, g, b);

		// Set the regular icon background color
		GET_RGB_COMPONENTS(context->colors.icon.background, r, g, b);
		context->colors.icon.background = SDL_MapRGB(context->surfaces.screen->format, r, g, b);

		// Set the regular icon foreground color
		GET_RGB_COMPONENTS(context->colors.icon.foreground, r, g, b);
		context->colors.icon.foreground = SDL_MapRGB(context->surfaces.screen->format, r, g, b);

		// Set the notch background color
		GET_RGB_COMPONENTS(context->colors.notch.background, r, g, b);
		context->colors.notch.background = SDL_MapRGB(context->surfaces.screen->format, r, g, b);

		// Set the notch foreground color
		GET_RGB_COMPONENTS(context->colors.notch.foreground, r, g, b);
		context->colors.notch.foreground = SDL_MapRGB(context->surfaces.screen->format, r, g, b);

		// Calculate the scale factor
		context->surfaces.scale_factor = SCALE_FACTOR_HEIGHT(context->surfaces.screen->h, SCALING_REFERENCE_HEIGHT);

		// Open the text font
		context->fonts.normal = TTF_OpenFont(localization_font_file_path(NULL), localization_font_height() * context->surfaces.scale_factor);

		// We failed to open the text font
		if (context->fonts.normal == NULL)
		{
			// Without the text font we can't render any text
			goto free_screen_surface;
		}

		// Open the small text font
		context->fonts.small = TTF_OpenFont(localization_font_file_path(NULL), localization_font_height() * SMALL_FONT_SCALE_FACTOR * context->surfaces.scale_factor);

		// We failed to open the small text font
		if (context->fonts.small == NULL)
		{
			// Without the text font we can't render any text
			goto free_text_font;
		}

		// Open the large text font
		context->fonts.large = TTF_OpenFont(localization_font_file_path(NULL), localization_font_height() * LARGE_FONT_SCALE_FACTOR * context->surfaces.scale_factor);

		// We failed to open the small text font
		if (context->fonts.large == NULL)
		{
			// Without the text font we can't render any text
			goto free_small_text_font;
		}

		// Open the gigantic text font
		context->fonts.gigantic = TTF_OpenFont(localization_font_file_path(NULL), localization_font_height() * GIGANTIC_FONT_SCALE_FACTOR * context->surfaces.scale_factor);

		// We failed to open the small text font
		if (context->fonts.gigantic == NULL)
		{
			// Without the text font we can't render any text
			goto free_large_text_font;
		}

		// Render the garlic icon
		context->surfaces.bars.top.logo.surface = icon_load("garlic.svg", DEFAULT_BAR_ICON_HEIGHT * context->surfaces.scale_factor, DEFAULT_BAR_ICON_HEIGHT * context->surfaces.scale_factor, &context->colors.base.foreground);

		// We failed to render the garlic icon
		if (context->surfaces.bars.top.logo.surface == NULL)
		{
			// No use continuing if we're missing icons
			goto free_gigantic_text_font;
		}

		// Render the east face button icon
		context->surfaces.bars.bottom.actions.select.surface = gui_render_select_legend_surface(context);
		
		// We failed to render the east face button icon
		if (context->surfaces.bars.bottom.actions.select.surface == NULL)
		{
			// No use continuing if we're missing icons
			goto free_logo;
		}

		// Render the south face button icon
		context->surfaces.bars.bottom.actions.back.surface = gui_render_back_legend_surface(context);

		// We failed to render the south face button icon
		if (context->surfaces.bars.bottom.actions.back.surface == NULL)
		{
			// No use continuing if we're missing icons
			goto free_select_action_surface;
		}

		// Render the menu button icon
		context->surfaces.bars.bottom.actions.menu.surface = gui_render_menu_legend_surface(context);

		// We failed to render the menu button icon
		if (context->surfaces.bars.bottom.actions.menu.surface == NULL)
		{
			// No use continuing if we're missing icons
			goto free_back_action_surface;
		}

		// Render the notch overlay
		context->surfaces.overlays.notch.surface = icon_load("notch.svg", DEFAULT_NOTCH_WIDTH * context->surfaces.scale_factor, DEFAULT_NOTCH_HEIGHT * context->surfaces.scale_factor, &context->colors.notch.background);

		// We failed to render the notch overlay
		if (context->surfaces.overlays.notch.surface == NULL)
		{
			// No use continuing if we're missing overlays
			goto free_menu_action_surface;
		}

		// Calculate the garlic icon position
		context->surfaces.bars.top.logo.position.x = SCREEN_MARGIN * context->surfaces.scale_factor;
		context->surfaces.bars.top.logo.position.y = SCREEN_MARGIN * context->surfaces.scale_factor;
		context->surfaces.bars.top.logo.position.w = context->surfaces.bars.top.logo.surface->w;
		context->surfaces.bars.top.logo.position.h = context->surfaces.bars.top.logo.surface->h;

		// Calculate the east button icon position
		context->surfaces.bars.bottom.actions.select.position.x = SCREEN_MARGIN * context->surfaces.scale_factor;
		context->surfaces.bars.bottom.actions.select.position.y = context->surfaces.screen->h - (SCREEN_MARGIN * context->surfaces.scale_factor) - context->surfaces.bars.bottom.actions.select.surface->h;
		context->surfaces.bars.bottom.actions.select.position.w = context->surfaces.bars.bottom.actions.select.surface->w;
		context->surfaces.bars.bottom.actions.select.position.h = context->surfaces.bars.bottom.actions.select.surface->h;

		// Calculate the south button icon position
		context->surfaces.bars.bottom.actions.back.position.x = context->surfaces.bars.bottom.actions.select.position.x + context->surfaces.bars.bottom.actions.select.position.w + (SCREEN_MARGIN * context->surfaces.scale_factor);
		context->surfaces.bars.bottom.actions.back.position.y = context->surfaces.bars.bottom.actions.select.position.y;
		context->surfaces.bars.bottom.actions.back.position.w = context->surfaces.bars.bottom.actions.back.surface->w;
		context->surfaces.bars.bottom.actions.back.position.h = context->surfaces.bars.bottom.actions.back.surface->h;

		// Calculate the menu button icon position
		context->surfaces.bars.bottom.actions.menu.position.x = context->surfaces.screen->w - (SCREEN_MARGIN * context->surfaces.scale_factor) - context->surfaces.bars.bottom.actions.menu.surface->w;
		context->surfaces.bars.bottom.actions.menu.position.y = context->surfaces.bars.bottom.actions.select.position.y;
		context->surfaces.bars.bottom.actions.menu.position.w = context->surfaces.bars.bottom.actions.menu.surface->w;
		context->surfaces.bars.bottom.actions.menu.position.h = context->surfaces.bars.bottom.actions.menu.surface->h;

		// Calculate the notch overlay position
		context->surfaces.overlays.notch.position.x = 0;
		context->surfaces.overlays.notch.position.y = (context->surfaces.screen->h - context->surfaces.overlays.notch.surface->h) >> 1;
		context->surfaces.overlays.notch.position.w = context->surfaces.overlays.notch.surface->w;
		context->surfaces.overlays.notch.position.h = context->surfaces.overlays.notch.surface->h;

		// We couldn't enumerate the joysticks
		if (SDL_NumJoysticks() <= 0)
		{
			// No use going further if we have no way of controlling the UI
			goto free_notch_overlay_surface;
		}

		// Open the internal joystick
		context->inputs.internal.joystick = SDL_JoystickOpen(0);

		// We failed to open the internal joystick
		if (context->inputs.internal.joystick == NULL)
		{
			// No use going further if we have no way of controlling the UI
			goto free_menu_action_surface;
		}

		// Allocate memory for the additional main menu data
		main_menu_node_data = calloc(1, sizeof(struct gui_menu_node_data));

		// We failed to allocate memory for the additional main menu data
		if (main_menu_node_data == NULL)
		{
			// Close the internal joystick
			goto close_internal_joystick;
		}

		// Create a node for the main menu and make it the application's current root node
		context->menu.root = gui_create_node(context, gui_activate_main_menu, gui_deactivate_main_menu, gui_update_basic_menu, gui_render_paging_icon_menu, gui_invalidate_main_menu, NODE_TYPE_MAIN_MENU, main_menu_node_data);

		// We failed to create the main menu node
		if (context->menu.root == NULL)
		{
			// Free the additional main menu data
			goto free_main_menu_node_data;
		}

		// Focus the main menu
		context->menu.active = context->menu.root;

		// Activate the root node
		context->menu.root->activate(context->menu.root);

		// Enable joystick input
		SDL_JoystickEventState(SDL_ENABLE);

		// Restore the previous UI state
		gui_restore_ui_state(context, (const char *)context->settings.ui_state);

		// And activate the context
		context->status.application.running = 1;
	}

	// Return the context
	return context;

free_main_menu_node_data:
	// Free the additional main menu data
	free(main_menu_node_data);

close_internal_joystick:
	// Close the internal joystick
	SDL_JoystickClose(context->inputs.internal.joystick);

free_notch_overlay_surface:
	// Free the notch overlay
	SDL_FreeSurface(context->surfaces.overlays.notch.surface);

free_menu_action_surface:
	// Free the start button icon
	SDL_FreeSurface(context->surfaces.bars.bottom.actions.menu.surface);

free_back_action_surface:
	// Free the south face button icon
	SDL_FreeSurface(context->surfaces.bars.bottom.actions.back.surface);

free_select_action_surface:
	// Free the east face button icon
	SDL_FreeSurface(context->surfaces.bars.bottom.actions.select.surface);

free_logo:
	// Free the garlic icon
	SDL_FreeSurface(context->surfaces.bars.top.logo.surface);

free_gigantic_text_font:
	// Free the large text font
	TTF_CloseFont(context->fonts.gigantic);

free_large_text_font:
	// Free the large text font
	TTF_CloseFont(context->fonts.large);

free_small_text_font:
	// Free the small text font
	TTF_CloseFont(context->fonts.small);

free_text_font:
	// Free the text font
	TTF_CloseFont(context->fonts.normal);

free_screen_surface:
	// Free the screen surface
	SDL_FreeSurface(context->surfaces.screen);

quit_ttf:
	// Quit SDL_ttf
	TTF_Quit();

quit_img:
	// Quit SDL_img
	IMG_Quit();

quit_sdl:
	// Quit SDL
	SDL_Quit();

free_context:
	// Free the context buffer
	free(context);

	// No context to return
	return NULL;
}

/**
 * @brief Frees the given GUI context.
 *
 * This function shuts down and frees the given GUI context structure.
 */
void gui_destroy_context(struct gui_context * context)
{
	// We've been given a NULL context
	if (context == NULL)
	{
		// Nothing to do here
		return;
	}

	// Destroy the context's root node
	gui_destroy_node(context->menu.root, 1);

	// This context contains a garlic icon
	if (context->surfaces.bars.top.logo.surface != NULL)
	{
		// Free the garlic icon surface
		SDL_FreeSurface(context->surfaces.bars.top.logo.surface);
	}

	// This context contains a battery icon
	if (context->surfaces.bars.top.battery.surface != NULL)
	{
		// Free the battery icon surface
		SDL_FreeSurface(context->surfaces.bars.top.battery.surface);
	}

	// This context contains a east button icon
	if (context->surfaces.bars.bottom.actions.select.surface != NULL)
	{
		// Free the east button icon surface
		SDL_FreeSurface(context->surfaces.bars.bottom.actions.select.surface);
	}

	// This context contains a south button icon
	if (context->surfaces.bars.bottom.actions.back.surface != NULL)
	{
		// Free the south button icon surface
		SDL_FreeSurface(context->surfaces.bars.bottom.actions.back.surface);
	}

	// This context contains a start button icon
	if (context->surfaces.bars.bottom.actions.menu.surface != NULL)
	{
		// Free the start button icon surface
		SDL_FreeSurface(context->surfaces.bars.bottom.actions.menu.surface);
	}

	// This context contains a notch overlay
	if (context->surfaces.overlays.notch.surface != NULL)
	{
		// Free the notch overlay
		SDL_FreeSurface(context->surfaces.overlays.notch.surface);
	}

	// This context contains an opened gigantic font
	if (context->fonts.gigantic != NULL)
	{
		// Close the gigantic font
		TTF_CloseFont(context->fonts.gigantic);
	}

	// This context contains an opened large font
	if (context->fonts.large != NULL)
	{
		// Close the large font
		TTF_CloseFont(context->fonts.large);
	}

	// This context contains an opened small font
	if (context->fonts.small != NULL)
	{
		// Close the small font
		TTF_CloseFont(context->fonts.small);
	}

	// This context contains an opened font
	if (context->fonts.normal != NULL)
	{
		// Close the font
		TTF_CloseFont(context->fonts.normal);
	}

	// This context contains a screen surface
	if (context->surfaces.screen != NULL)
	{
		// Free the screen surface
		SDL_FreeSurface(context->surfaces.screen);
	}

	// This context contains an opened internal joystick
	if (context->inputs.internal.joystick != NULL)
	{
		// Close the internal joystick
		SDL_JoystickClose(context->inputs.internal.joystick);
	}

	// Quit the IMG library
	IMG_Quit();

	// Quit the TTF library
	TTF_Quit();

	// Quit SDL itself
	SDL_Quit();

	// Free the context itself
	free(context);
}

/**
 * @brief Creates a basic node.
 *
 * This function allocates and populates a basic GUI node structure.
 */
struct gui_node * gui_create_node(struct gui_context * context, callback activate, callback deactivate, callback update, callback render, callback invalidate, int type, void * data)
{
	// Allocate memory for the node
	struct gui_node * node = (struct gui_node *)calloc(1, sizeof(struct gui_node));

	// We managed to allocate enough memory
	if (node != NULL)
	{
		// Initialize the node properties
		node->context = context;
		node->activate = activate;
		node->deactivate = deactivate;
		node->update = update;
		node->render = render;
		node->invalidate = invalidate;
		node->type = type;
		node->data = data;
	}

	// Return the populated node
	return node;
}

/**
 * @brief Frees the given node.
 *
 * This function deactivates and frees the given GUI node structure.
 */
void gui_destroy_node(struct gui_node * node, int free_root)
{
	// We've been given a NULL node
	if (node == NULL)
	{
		// Nothing to do here
		return;
	}

	// This node contains children
	if (node->child != NULL)
	{
		// Start at the currently selected child
		struct gui_node * child = node->child;

		do
		{
			// Backup the next child pointer (as gui_destroy_node releases it)
			struct gui_node * next_child = child->next;

			// Destroy the child node
			gui_destroy_node(child, 1);

			// And move on to the next child
			child = next_child;
		} while (child != node->child);

		// NULL the child reference
		node->child = NULL;
	}

	// The caller wants to close this node
	if (free_root)
	{
		// This node contains a deactivation callback
		if (node->deactivate != NULL)
		{
			// Call the deactivation callback
			node->deactivate(node);
		}

		// This node contains node-specific data
		if (node->data != NULL)
		{
			// Free the node specific data
			free(node->data);
		}

		// Free the node itself
		free(node);
	}

	// The caller only wants to discard the child nodes
	else
	{
		// Which means we should NULL the child references to ensure proper re-population on re-activation
		node->child = NULL;
		node->children = NULL;
	}
}

/**
 * @brief Polls for input and executes logic-related tasks.
 *
 * This is essentially the brain of the operation.
 * It reads joystick input and executes actions.
 */
void gui_update(struct gui_context * context)
{
	// We can't update an invalid context
	if (context == NULL)
	{
		// Exit
		return;
	}

	// Get the current charger status
	int charging = battery_is_charging();

	// Get the current battery percentage
	int percentage = battery_percentage();

	// The battery status has changed
	if (context->status.battery.charging != charging || context->status.battery.percentage != percentage)
	{
		// Update the context's battery status
		context->status.battery.charging = charging;
		context->status.battery.percentage = percentage;

		// We are holding onto a no longer valid battery icon
		if (context->surfaces.bars.top.battery.surface != NULL)
		{
			// Free the battery icon surface
			SDL_FreeSurface(context->surfaces.bars.top.battery.surface);

			// And NULL its reference so it can be re-rendered
			context->surfaces.bars.top.battery.surface = NULL;
		}
	}

	// Format the current time into a human-readable string
	const char * current_time = clock_render_text(context->settings.meridian_time);

	// The current time has changed (either by toggling a time-related setting or by natural progression of time)
	if (strcmp(context->status.clock.current_time, current_time) != 0)
	{
		// Update the context's current time string
		strcpy(context->status.clock.current_time, current_time);

		// We are holding onto a no longer valid current time overlay
		if (context->surfaces.bars.top.clock.surface != NULL)
		{
			// Free the current time overlay surface
			SDL_FreeSurface(context->surfaces.bars.top.clock.surface);

			// And NULL its reference so it can be re-rendered
			context->surfaces.bars.top.clock.surface = NULL;
		}
	}

	// Backup the previous input state
	context->inputs.internal.previous = context->inputs.internal.current;

	// The last polled SDL input event
	SDL_Event event;

	// Read all pending input events
	while (SDL_PollEvent(&event) != 0)
	{
		// Differentiate input event types
		switch (event.type)
		{
			// Regular button events
			case SDL_JOYBUTTONUP:
			case SDL_JOYBUTTONDOWN:
			{
				// Determine the pressed state
				int pressed = event.type == SDL_JOYBUTTONDOWN;

				// Differentiate between buttons
				switch (event.jbutton.button)
				{
					// The power button
					case POWER_BUTTON:
					{
						// Update the internal button state
						context->inputs.internal.current.power = pressed;

						// Break
						break;
					}

					// The south-facing face button (A on XBOX, B on Nintendo, Cross on PlayStation)
					case SOUTH_BUTTON:
					{
						// Update the internal button state
						context->inputs.internal.current.south = pressed;

						// Break
						break;
					}

					// The east-facing face button (B on XBOX, A on Nintendo, Circle on PlayStation)
					case EAST_BUTTON:
					{
						// Update the internal button state
						context->inputs.internal.current.east = pressed;

						// Break
						break;
					}

					// The volume down button (-)
					case VOLUME_DOWN_BUTTON:
					{
						// Update the internal button state
						context->inputs.internal.current.volume_down = pressed;

						// Break
						break;
					}

					// The north-facing face button (Y on XBOX, X on Nintendo, Triangle on PlayStation)
					case NORTH_BUTTON:
					{
						// Update the internal button state
						context->inputs.internal.current.north = pressed;

						// Break
						break;
					}

					// The west-facing face button (X on XBOX, Y on Nintendo, Square on PlayStation)
					case WEST_BUTTON:
					{
						// Update the internal button state
						context->inputs.internal.current.west = pressed;

						// Break
						break;
					}

					// The volume up button (+)
					case VOLUME_UP_BUTTON:
					{
						// Update the internal button state
						context->inputs.internal.current.volume_up = pressed;

						// Break
						break;
					}

					// The left bumper button (L1)
					case LEFT_BUMPER_BUTTON:
					{
						// Update the internal button state
						context->inputs.internal.current.left_bumper = pressed;

						// Break
						break;
					}

					// The right bumper button (R1)
					case RIGHT_BUMPER_BUTTON:
					{
						// Update the internal button state
						context->inputs.internal.current.right_bumper = pressed;

						// Break
						break;
					}

					// The select button
					case SELECT_BUTTON:
					{
						// Update the internal button state
						context->inputs.internal.current.select = pressed;

						// Break
						break;
					}

					// The start button
					case START_BUTTON:
					{
						// Update the internal button state
						context->inputs.internal.current.start = pressed;

						// Break
						break;
					}

					// The mode button (home / menu / mode)
					case MODE_BUTTON:
					{
						// Update the internal button state
						context->inputs.internal.current.mode = pressed;

						// Break
						break;
					}

					// The left thumb button (L3)
					case LEFT_THUMB_BUTTON:
					{
						// Update the internal button state
						context->inputs.internal.current.left_thumb = pressed;

						// Break
						break;
					}

					// The right thumb button (R3)
					case RIGHT_THUMB_BUTTON:
					{
						// Update the internal button state
						context->inputs.internal.current.right_thumb = pressed;

						// Break
						break;
					}

					// Other buttons
					default:
					{
						// Break
						break;
					}
				}

				// Log the event
				// printf("button %d state %d\n", event.jbutton.button, pressed);

				// Break
				break;
			}

			// D-pad events
			case SDL_JOYHATMOTION:
			{
				// Calculate the X & Y axis values
				int x = event.jhat.value & SDL_HAT_LEFT ? -1 : event.jhat.value & SDL_HAT_RIGHT ? 1 : 0;
				int y = event.jhat.value & SDL_HAT_UP ? -1 : event.jhat.value & SDL_HAT_DOWN ? 1 : 0;

				// Update the internal d-pad axes
				context->inputs.internal.current.dpad_x = x;
				context->inputs.internal.current.dpad_y = y;

				// Log the event
				// printf("dpad x = %d, y = %d\n", x, y);

				// Break
				break;
			}

			// Analog stick & trigger events
			case SDL_JOYAXISMOTION:
			{
				// Differentiate between axes
				switch (event.jaxis.axis)
				{
					// The left analog stick's X-axis
					case LEFT_ANALOG_X_AXIS:
					{
						// Update the internal axis
						context->inputs.internal.current.left_analog_x = event.jaxis.value;

						// Break
						break;
					}

					// The left analog stick's Y-axis
					case LEFT_ANALOG_Y_AXIS:
					{
						// Update the internal axis
						context->inputs.internal.current.left_analog_y = event.jaxis.value;

						// Break
						break;
					}

					// The left trigger axis
					case LEFT_TRIGGER_AXIS:
					{
						// Update the internal axis
						context->inputs.internal.current.left_trigger = event.jaxis.value;

						// Break
						break;
					}

					// The right analog stick's X-axis
					case RIGHT_ANALOG_X_AXIS:
					{
						// Update the internal axis
						context->inputs.internal.current.right_analog_x = event.jaxis.value;

						// Break
						break;
					}

					// The right analog stick's Y-axis
					case RIGHT_ANALOG_Y_AXIS:
					{
						// Update the internal axis
						context->inputs.internal.current.right_analog_y = event.jaxis.value;

						// Break
						break;
					}

					// The right trigger axis
					case RIGHT_TRIGGER_AXIS:
					{
						// Update the internal axis
						context->inputs.internal.current.right_trigger = event.jaxis.value;

						// Break
						break;
					}

					// Other axes
					default:
					{
						// Break
						break;
					}
				}

				// Log the event
				// printf("axis %d value %d\n", event.jaxis.axis, event.jaxis.value);

				// Break
				break;
			}

			// Exit event
			case SDL_QUIT:
			{
				// Set the exit code
				context->status.application.exit_code = 0;

				// Mark the context as stopped
				context->status.application.running = 0;

				// Break
				break;
			}

			// Other events
			default:
			{
				// Break
				break;
			}
		}
	}

	// The mode button swallow flag (used to prevent the context menu from popping up on brightness changes)
	static int swallow_mode_button = 0;

	// The application is still running
	if (context->status.application.running)
	{
		// The volume up button was pressed
		if (context->inputs.internal.current.volume_up && context->inputs.internal.previous.volume_up != context->inputs.internal.current.volume_up)
		{
			// While the mode button was pressed
			if (context->inputs.internal.current.mode)
			{
				// Which means we should swallow the mode button release event
				swallow_mode_button = 1;
			}
		}

		// The volume down button was pressed
		if (context->inputs.internal.current.volume_down && context->inputs.internal.previous.volume_down != context->inputs.internal.current.volume_down)
		{
			// While the mode button was pressed
			if (context->inputs.internal.current.mode)
			{
				// Which means we should swallow the mode button release event
				swallow_mode_button = 1;
			}
		}

		// The mode button was released
		if (!context->inputs.internal.current.mode && context->inputs.internal.previous.mode != context->inputs.internal.current.mode)
		{
			// The button isn't being swallowed
			if (!swallow_mode_button)
			{
				// The active context menu
				struct gui_node * active_context_menu = context->menu.active;

				// Iterate upwards to find the active context menu
				while (active_context_menu != NULL)
				{
					// We've found an active context menu
					if (active_context_menu->type == NODE_TYPE_CONTEXT_MENU)
					{
						// Stop moving upwards
						break;
					}

					// Move upwards
					active_context_menu = active_context_menu->parent;
				}

				// We're currently inside a context menu
				if (active_context_menu != NULL)
				{
					// Get the context menu
					struct gui_node * context_menu = context->menu.active;

					// Deactivate the context menu (which will focus its parent)
					context_menu->deactivate(context_menu);

					// We're closing the context menu root node
					if (context_menu == active_context_menu)
					{
						// Context menus aren't part of the regular UI tree which means they won't be covered by the tree's garbage collector and thus need to be freed explicitly here
						free(context_menu->data);
						free(context_menu);
					}
				}

				// We're in a regular menu
				else
				{
					// Allocate the additional menu data
					struct gui_menu_node_data * context_menu_node_data = calloc(1, sizeof(struct gui_menu_node_data));

					// We managed to allocate the additional menu data
					if (context_menu_node_data != NULL)
					{
						// Create a context menu node
						struct gui_node * context_menu = gui_create_node(context, gui_activate_context_menu, gui_deactivate_context_menu, gui_update_basic_menu, gui_render_vertical_scrolling_list_menu, gui_invalidate_context_menu, NODE_TYPE_CONTEXT_MENU, context_menu_node_data);

						// We managed to create the context menu node
						if (context_menu != NULL)
						{
							// Set the parent
							context_menu->parent = context->menu.active;

							// Activate the context menu (which will populate and focus the context menu)
							context_menu->activate(context_menu);
						}

						// We failed to create the context menu node
						else
						{
							// Free the additional menu data
							free(context_menu_node_data);
						}
					}
				}
			}

			// Reset the swallow button flag
			swallow_mode_button = 0;
		}

		// We have an active node that can be updated
		if (context->status.application.running && context->menu.active != NULL && context->menu.active->update != NULL)
		{
			// Update the active node
			context->menu.active->update(context->menu.active);
		}

		// Get the current locale
		const char * locale = setlocale(LC_ALL, NULL);

		// The locale has been changed
		if (strcmp(context->settings.locale, locale) != 0)
		{
			// Update the context's last known locale
			sprintf(context->settings.locale, locale);

			// We have an open small size text font
			if (context->fonts.small != NULL)
			{
				// Close the small size text font
				TTF_CloseFont(context->fonts.small);
			}

			// We have an open normal size text font
			if (context->fonts.normal != NULL)
			{
				// Close the normal size text font
				TTF_CloseFont(context->fonts.normal);
			}

			// We have an open large size text font
			if (context->fonts.large != NULL)
			{
				// Close the large size text font
				TTF_CloseFont(context->fonts.large);
			}

			// We have an open gigantic size text font
			if (context->fonts.gigantic != NULL)
			{
				// Close the gigantic size text font
				TTF_CloseFont(context->fonts.gigantic);
			}

			// Open the small text font
			context->fonts.small = TTF_OpenFont(localization_font_file_path(NULL), localization_font_height() * SMALL_FONT_SCALE_FACTOR * context->surfaces.scale_factor);

			// Open the normal text font
			context->fonts.normal = TTF_OpenFont(localization_font_file_path(NULL), localization_font_height() * context->surfaces.scale_factor);

			// Open the large text font
			context->fonts.large = TTF_OpenFont(localization_font_file_path(NULL), localization_font_height() * LARGE_FONT_SCALE_FACTOR * context->surfaces.scale_factor);

			// Open the gigantic text font
			context->fonts.gigantic = TTF_OpenFont(localization_font_file_path(NULL), localization_font_height() * GIGANTIC_FONT_SCALE_FACTOR * context->surfaces.scale_factor);

			// We are holding onto a no longer valid current time overlay
			if (context->surfaces.bars.top.clock.surface != NULL)
			{
				// Free the current time overlay surface
				SDL_FreeSurface(context->surfaces.bars.top.clock.surface);

				// And NULL its reference so it can be re-rendered
				context->surfaces.bars.top.clock.surface = NULL;
			}

			// We have a left-over select button legend surface with the wrong locale's text on it
			if (context->surfaces.bars.bottom.actions.select.surface != NULL)
			{
				// Free the select button legend surface
				SDL_FreeSurface(context->surfaces.bars.bottom.actions.select.surface);
			}

			// Re-render the select button legend surface
			context->surfaces.bars.bottom.actions.select.surface = gui_render_select_legend_surface(context);

			// We managed to re-render the select button legend surface
			if (context->surfaces.bars.bottom.actions.select.surface != NULL)
			{
				// Re-calculate the select button legend surface position
				context->surfaces.bars.bottom.actions.select.position.x = SCREEN_MARGIN * context->surfaces.scale_factor;
				context->surfaces.bars.bottom.actions.select.position.y = context->surfaces.screen->h - (SCREEN_MARGIN * context->surfaces.scale_factor) - context->surfaces.bars.bottom.actions.select.surface->h;
				context->surfaces.bars.bottom.actions.select.position.w = context->surfaces.bars.bottom.actions.select.surface->w;
				context->surfaces.bars.bottom.actions.select.position.h = context->surfaces.bars.bottom.actions.select.surface->h;
			}

			// We have a left-over back button legend surface with the wrong locale's text on it
			if (context->surfaces.bars.bottom.actions.back.surface != NULL)
			{
				// Free the back button legend surface
				SDL_FreeSurface(context->surfaces.bars.bottom.actions.back.surface);
			}

			// Re-render the back button legend surface
			context->surfaces.bars.bottom.actions.back.surface = gui_render_back_legend_surface(context);

			// We managed to re-render the back button legend surface
			if (context->surfaces.bars.bottom.actions.back.surface != NULL)
			{
				// Re-calculate the back button legend surface position
				context->surfaces.bars.bottom.actions.back.position.x = context->surfaces.bars.bottom.actions.select.position.x + context->surfaces.bars.bottom.actions.select.position.w + (SCREEN_MARGIN * context->surfaces.scale_factor);
				context->surfaces.bars.bottom.actions.back.position.y = context->surfaces.bars.bottom.actions.select.position.y;
				context->surfaces.bars.bottom.actions.back.position.w = context->surfaces.bars.bottom.actions.back.surface->w;
				context->surfaces.bars.bottom.actions.back.position.h = context->surfaces.bars.bottom.actions.back.surface->h;
			}

			// We have a left-over menu button legend surface with the wrong locale's text on it
			if (context->surfaces.bars.bottom.actions.menu.surface != NULL)
			{
				// Free the menu button legend surface
				SDL_FreeSurface(context->surfaces.bars.bottom.actions.menu.surface);
			}

			// Re-render the menu button legend surface
			context->surfaces.bars.bottom.actions.menu.surface = gui_render_menu_legend_surface(context);

			// We managed to re-render the menu button legend surface
			if (context->surfaces.bars.bottom.actions.menu.surface != NULL)
			{
				// Re-calculate the menu button legend surface position
				context->surfaces.bars.bottom.actions.menu.position.x = context->surfaces.screen->w - (SCREEN_MARGIN * context->surfaces.scale_factor) - context->surfaces.bars.bottom.actions.menu.surface->w;
				context->surfaces.bars.bottom.actions.menu.position.y = context->surfaces.bars.bottom.actions.select.position.y;
				context->surfaces.bars.bottom.actions.menu.position.w = context->surfaces.bars.bottom.actions.menu.surface->w;
				context->surfaces.bars.bottom.actions.menu.position.h = context->surfaces.bars.bottom.actions.menu.surface->h;
			}

			// Trigger the invalidate callback
			gui_invalidate(context->menu.root);
			gui_invalidate(context->menu.active);
		}

		// The power button was pressed
		if (context->inputs.internal.current.power && context->inputs.internal.previous.power != context->inputs.internal.current.power)
		{
			// Erase the resume flag
			context->settings.resume = 0;

			// Write the settings to disk
			gui_write_configuration(context);

			// Shutdown the device
			io_shutdown();
		}
	}
}

/**
 * @brief Renders the user interface.
 *
 * This is essentially the brawn of the operation.
 * It renders the current user interface state.
 */
void gui_render(struct gui_context * context)
{
	// We can't render an invalid context
	if (context == NULL)
	{
		// Exit
		return;
	}

	// We can't render without a screen surface
	if (context->surfaces.screen == NULL)
	{
		// Exit
		return;
	}

	// We need to re-render the battery icon
	if (context->surfaces.bars.top.battery.surface == NULL)
	{
		// Render the battery icon
		context->surfaces.bars.top.battery.surface = battery_render(context->surfaces.scale_factor, context->colors.base.foreground);

		// We managed to render the battery icon
		if (context->surfaces.bars.top.battery.surface != NULL)
		{
			// Calculate the battery icon position
			context->surfaces.bars.top.battery.position.x = context->surfaces.screen->w - context->surfaces.bars.top.battery.surface->w - (SCREEN_MARGIN * context->surfaces.scale_factor);
			context->surfaces.bars.top.battery.position.y = SCREEN_MARGIN * context->surfaces.scale_factor;
			context->surfaces.bars.top.battery.position.w = context->surfaces.bars.top.battery.surface->w;
			context->surfaces.bars.top.battery.position.h = context->surfaces.bars.top.battery.surface->h;
		}
	}

	// We need to re-render the current time overlay
	if (context->surfaces.bars.top.clock.surface == NULL && context->fonts.normal != NULL)
	{
		// Render the current time overlay
		context->surfaces.bars.top.clock.surface = clock_render(context->settings.meridian_time, context->fonts.normal, context->colors.base.foreground);

		// We managed to render the current time overlay
		if (context->surfaces.bars.top.clock.surface != NULL)
		{
			// Calculate the current time overlay icon position
			context->surfaces.bars.top.clock.position.x = context->surfaces.bars.top.battery.position.x - (SCREEN_MARGIN * context->surfaces.scale_factor) - context->surfaces.bars.top.clock.surface->w;
			context->surfaces.bars.top.clock.position.y = ((DEFAULT_TOP_BAR_HEIGHT * context->surfaces.scale_factor) - context->surfaces.bars.top.clock.surface->h) / 2.0f;
			context->surfaces.bars.top.clock.position.w = context->surfaces.bars.top.clock.surface->w;
			context->surfaces.bars.top.clock.position.h = context->surfaces.bars.top.clock.surface->h;
		}
	}

	// Fill the screen with the chosen background color
	SDL_FillSurface(context->surfaces.screen, context->colors.base.background);

	// We have an active node that can be rendered
	if (context->menu.active != NULL && context->menu.active->render != NULL)
	{
		// Render the active node
		context->menu.active->render(context->menu.active);
	}

	// We need to render the garlic icon
	if (context->surfaces.bars.top.logo.surface != NULL)
	{
		// Render the garlic icon
		SDL_BlitSurface(context->surfaces.bars.top.logo.surface, NULL, context->surfaces.screen, &context->surfaces.bars.top.logo.position);
	}

	// We need to render the current time overlay
	if (context->surfaces.bars.top.clock.surface != NULL)
	{
		// Render the current time overlay
		SDL_BlitSurface(context->surfaces.bars.top.clock.surface, NULL, context->surfaces.screen, &context->surfaces.bars.top.clock.position);
	}

	// We need to render the battery icon
	if (context->surfaces.bars.top.battery.surface != NULL)
	{
		// Render the battery icon
		SDL_BlitSurface(context->surfaces.bars.top.battery.surface, NULL, context->surfaces.screen, &context->surfaces.bars.top.battery.position);
	}

	// We need to render the east button icon
	if (context->surfaces.bars.bottom.actions.select.surface != NULL)
	{
		// Render the east button icon
		SDL_BlitSurface(context->surfaces.bars.bottom.actions.select.surface, NULL, context->surfaces.screen, &context->surfaces.bars.bottom.actions.select.position);
	}

	// We need to render the south button icon
	if (context->surfaces.bars.bottom.actions.back.surface != NULL)
	{
		// Render the south button icon
		SDL_BlitSurface(context->surfaces.bars.bottom.actions.back.surface, NULL, context->surfaces.screen, &context->surfaces.bars.bottom.actions.back.position);
	}

	// We need to render the start button icon
	if (context->surfaces.bars.bottom.actions.menu.surface != NULL)
	{
		// Render the start button icon
		SDL_BlitSurface(context->surfaces.bars.bottom.actions.menu.surface, NULL, context->surfaces.screen, &context->surfaces.bars.bottom.actions.menu.position);
	}

	// Flip the front and back buffer
	SDL_Flip(context->surfaces.screen);
}

/**
 * @brief Reads the current configuration from disk.
 */
void gui_read_configuration(struct gui_context * context)
{
	// Load the XML document
	xmlDocPtr document = io_read_folder_configuration(FOLDER_CONFIGURATION_BOOT_FOLDER);

	// Whether the locale was set or not
	int locale_set = 0;

	// Set the default colors
	context->colors.base.background = 0x2d2d2d;
	context->colors.base.foreground = 0xffffff;
	context->colors.legend.background = 0xffffff;
	context->colors.legend.foreground = 0x757575;
	context->colors.icon.background = 0xffffff;
	context->colors.icon.foreground = 0x000000;
	context->colors.notch.background = 0x000000;
	context->colors.notch.foreground = 0xffffff;

	// We managed to load the XML document
	if (document != NULL)
	{
		// Get the root element <Configuration>
		xmlNodePtr root = xmlDocGetRootElement(document);

		// We managed to get the root element <Configuration>
		if (root != NULL)
		{
			// Iterate all child nodes
			for (xmlNodePtr child = root->children; child != NULL; child = child->next)
			{
				// We've found the ClockStyle node
				if (xmlStrcmp(child->name, BAD_CAST "ClockStyle") == 0)
				{
					// Get the ClockStyle value
					xmlChar * clockStyleValue = xmlNodeGetContent(child);

					// We managed to get the ClockStyle value
					if (clockStyleValue != NULL)
					{
						// Toggle the ClockStyle according to the setting file
						context->settings.meridian_time = strcmp((const char *)clockStyleValue, "12H") == 0;

						// Free the ClockStyle value
						xmlFree(clockStyleValue);
					}
				}

				// We've found the Locale node
				else if (xmlStrcmp(child->name, BAD_CAST "Locale") == 0)
				{
					// Get the Locale value
					xmlChar * localeValue = xmlNodeGetContent(child);

					// We managed to get the Locale value
					if (localeValue != NULL)
					{
						// Set the context's last known locale
						strcpy(context->settings.locale, (const char *)localeValue);

						// Set the locale
						localization_change_locale((const char *)localeValue);

						// Set the flag
						locale_set = 1;

						// Free the Locale value
						xmlFree(localeValue);
					}
				}

				// We've found the Resume node
				else if (xmlStrcmp(child->name, BAD_CAST "Resume") == 0)
				{
					// Get the Resume value
					xmlChar * resumeValue = xmlNodeGetContent(child);

					// We managed to get the Resume value
					if (resumeValue != NULL)
					{
						// Set the context's resume flag
						context->settings.resume = xmlStrcmp(resumeValue, BAD_CAST "true") == 0;

						// Free the Resume value
						xmlFree(resumeValue);
					}
				}

				// We've found the UI node
				else if (xmlStrcmp(child->name, BAD_CAST "UI") == 0)
				{
					// Get the UI value
					xmlChar * uiValue = xmlNodeGetContent(child);

					// We managed to get the UI value
					if (uiValue != NULL)
					{
						// Restore the context's UI state
						strcpy(context->settings.ui_state, (const char *)uiValue);

						// Free the UI value
						xmlFree(uiValue);
					}
				}
			}
		}

		// Get the background color
		context->colors.base.background = io_get_skin_color(document, "Base", "Background", 0x2d2d2d);

		// Get the foreground color
		context->colors.base.foreground = io_get_skin_color(document, "Base", "Foreground", 0xffffff);

		// Get the legend icon background color
		context->colors.legend.background = io_get_skin_color(document, "Legend", "Background", 0xffffff);

		// Get the legend icon foreground color
		context->colors.legend.foreground = io_get_skin_color(document, "Legend", "Foreground", 0x757575);

		// Get the regular icon background color
		context->colors.icon.background = io_get_skin_color(document, "Icon", "Background", 0xffffff);

		// Get the regular icon foreground color
		context->colors.icon.foreground = io_get_skin_color(document, "Icon", "Foreground", 0x000000);

		// Get the notch background color
		context->colors.notch.background = io_get_skin_color(document, "Notch", "Background", 0x000000);

		// Get the notch foreground color
		context->colors.notch.foreground = io_get_skin_color(document, "Notch", "Foreground", 0xffffff);

		// Free the XML document
		xmlFreeDoc(document);
	}

	// The locale wasn't set yet
	if (!locale_set)
	{
		// Set the default locale
		localization_change_locale("en");
	}

	// The global configuration file hasn't been created yet
	if (document == NULL)
	{
		// Write a default configuration file to disk
		gui_write_configuration(context);
	}
}

/**
 * @brief Writes the current configuration to disk.
 */
void gui_write_configuration(struct gui_context * context)
{
	// Create the XML document
	xmlDocPtr document = xmlNewDoc(BAD_CAST "1.0");

	// We managed to create the XML document
	if (document != NULL)
	{
		// Create the root node <Configuration>
		xmlNodePtr root = xmlNewNode(NULL, BAD_CAST "Configuration");

		// We managed to create the root node <Configuration>
		if (root != NULL)
		{
			// Set the document's root element
			xmlDocSetRootElement(document, root);

			// Create and attach the child node <ClockStyle>
			xmlNodePtr clockStyleNode = xmlNewTextChild(root, NULL, BAD_CAST "ClockStyle", BAD_CAST (context->settings.meridian_time ? "12H" : "24H"));

			// We managed to create and attach the child node <ClockStyle>
			if (clockStyleNode != NULL)
			{
				// Create and attach the child node <Locale>
				xmlNodePtr localeNode = xmlNewTextChild(root, NULL, BAD_CAST "Locale", BAD_CAST setlocale(LC_ALL, NULL));

				// We managed to create and attach the child node <Locale>
				if (localeNode != NULL)
				{
					// Create and attach the child node <Resume>
					xmlNodePtr resumeNode = xmlNewTextChild(root, NULL, BAD_CAST "Resume", BAD_CAST (context->settings.resume ? "true" : "false"));

					// We managed to create and attach the child node <Resume>
					if (resumeNode != NULL)
					{
						// Serialize the UI state
						char * ui_state = gui_serialize_ui_state(context);

						// We managed to serialize the UI state
						if (ui_state != NULL)
						{
							// Create and attach the child node <UI>
							xmlNodePtr uiNode = xmlNewTextChild(root, NULL, BAD_CAST "UI", BAD_CAST ui_state);

							// Free the UI state
							free(ui_state);
						}

						// Save the document to a file
						io_write_folder_configuration(document, FOLDER_CONFIGURATION_BOOT_FOLDER);
					}
				}
			}
		}

		// Cleanup
		xmlFreeDoc(document);
	}
}