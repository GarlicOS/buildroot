#ifndef GUI_MAIN_MENU_H
#define GUI_MAIN_MENU_H

#include "gui_menu.h"
#include "gui_library_menu.h"
#include "gui_playlist_menu.h"
#include "gui_context_menu.h"
#include "gui_retroarch_menu.h"

/**
 * @brief Activates the main menu.
 */
static void gui_activate_main_menu(struct gui_node * this)
{
	// Allocate a temporary buffer to hold the full menu title
	char * title = malloc(strlen(gettext("GarlicOS %s")) + strlen(VERSION));

	// We managed to allocate a temporary buffer to hold the full menu title
	if (title != NULL)
	{
		// Puzzle together the full menu title
		sprintf(title, gettext("GarlicOS %s"), VERSION);

		// Render the title text surface
		gui_activate_menu(this, title);

		// Free the temporary buffer
		free(title);
	}

	// Create the continue menu
	struct gui_path_menu_node_data * continue_menu_node_data = calloc(1, sizeof(struct gui_path_menu_node_data));
	strcpy(continue_menu_node_data->path, RETROARCH_DEFAULT_HISTORY_PLAYLIST_FILE_PATH);
	struct gui_node * continue_menu = gui_create_node(this->context, gui_activate_continue_menu, gui_deactivate_playlist_menu, gui_update_basic_menu, gui_render_paging_icon_menu, gui_invalidate_continue_menu, NODE_TYPE_PLAYLIST_CONTINUE_MENU, continue_menu_node_data);
	const char * continue_menu_title = gettext("Resume");
	SDL_Surface * continue_menu_icon = SVG_Load("/usr/share/icons/resume.svg", (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor, (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor, &this->context->colors.icon.foreground);
	SDL_Surface * continue_menu_icon_small = SVG_Load("/usr/share/icons/resume.svg", (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor * UNFOCUSED_PAGING_CARD_WIDTH_SCALE_FACTOR, (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor * UNFOCUSED_PAGING_CARD_WIDTH_SCALE_FACTOR, &this->context->colors.icon.foreground);
	continue_menu->normal_surface = icon_paging_card(continue_menu_icon_small, VERTICAL_ALIGNMENT_CENTER, localization_font_file_path(NULL), localization_font_height(), this->context->colors.legend.foreground, this->context->colors.legend.background, SCREEN_MARGIN * this->context->surfaces.scale_factor, DEFAULT_PAGING_CARD_WIDTH * this->context->surfaces.scale_factor, this->context->surfaces.screen->h * UNFOCUSED_PAGING_CARD_HEIGHT_SCALE_FACTOR, DEFAULT_PAGING_CARD_ROUNDED_CORNER_RADIUS * this->context->surfaces.scale_factor, continue_menu_title);
	continue_menu->selected_surface = icon_paging_card(continue_menu_icon, VERTICAL_ALIGNMENT_CENTER, localization_font_file_path(NULL), localization_font_height(), this->context->colors.legend.foreground, this->context->colors.legend.background, SCREEN_MARGIN * this->context->surfaces.scale_factor, DEFAULT_PAGING_CARD_WIDTH * this->context->surfaces.scale_factor, this->context->surfaces.screen->h * FOCUSED_PAGING_CARD_HEIGHT_SCALE_FACTOR, DEFAULT_PAGING_CARD_ROUNDED_CORNER_RADIUS * this->context->surfaces.scale_factor, continue_menu_title);
	continue_menu->parent = this;
	SDL_FreeSurface(continue_menu_icon);
	SDL_FreeSurface(continue_menu_icon_small);

	// Create the favorites menu
	struct gui_path_menu_node_data * favorites_menu_node_data = calloc(1, sizeof(struct gui_path_menu_node_data));
	strcpy(favorites_menu_node_data->path, RETROARCH_DEFAULT_FAVORITES_PLAYLIST_FILE_PATH);
	struct gui_node * favorites_menu = gui_create_node(this->context, gui_activate_favorites_menu, gui_deactivate_playlist_menu, gui_update_basic_menu, gui_render_paging_icon_menu, gui_invalidate_favorites_menu, NODE_TYPE_PLAYLIST_FAVORITES_MENU, favorites_menu_node_data);
	const char * favorites_menu_title = gettext("Favorites");
	SDL_Surface * favorites_menu_icon = SVG_Load("/usr/share/icons/favorite.svg", (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor, (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor, &this->context->colors.icon.foreground);
	SDL_Surface * favorites_menu_icon_small = SVG_Load("/usr/share/icons/favorite.svg", (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor * UNFOCUSED_PAGING_CARD_WIDTH_SCALE_FACTOR, (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor * UNFOCUSED_PAGING_CARD_WIDTH_SCALE_FACTOR, &this->context->colors.icon.foreground);
	favorites_menu->normal_surface = icon_paging_card(favorites_menu_icon_small, VERTICAL_ALIGNMENT_CENTER, localization_font_file_path(NULL), localization_font_height(), this->context->colors.legend.foreground, this->context->colors.legend.background, SCREEN_MARGIN * this->context->surfaces.scale_factor, DEFAULT_PAGING_CARD_WIDTH * this->context->surfaces.scale_factor, this->context->surfaces.screen->h * UNFOCUSED_PAGING_CARD_HEIGHT_SCALE_FACTOR, DEFAULT_PAGING_CARD_ROUNDED_CORNER_RADIUS * this->context->surfaces.scale_factor, favorites_menu_title);
	favorites_menu->selected_surface = icon_paging_card(favorites_menu_icon, VERTICAL_ALIGNMENT_CENTER, localization_font_file_path(NULL), localization_font_height(), this->context->colors.legend.foreground, this->context->colors.legend.background, SCREEN_MARGIN * this->context->surfaces.scale_factor, DEFAULT_PAGING_CARD_WIDTH * this->context->surfaces.scale_factor, this->context->surfaces.screen->h * FOCUSED_PAGING_CARD_HEIGHT_SCALE_FACTOR, DEFAULT_PAGING_CARD_ROUNDED_CORNER_RADIUS * this->context->surfaces.scale_factor, favorites_menu_title);
	favorites_menu->parent = this;
	SDL_FreeSurface(favorites_menu_icon);
	SDL_FreeSurface(favorites_menu_icon_small);

	// Create the library menu
	struct gui_menu_node_data * library_menu_node_data = calloc(1, sizeof(struct gui_menu_node_data));
	struct gui_node * library_menu = gui_create_node(this->context, gui_activate_library_menu, gui_deactivate_library_menu, gui_update_basic_menu, gui_render_paging_icon_menu, gui_invalidate_library_menu, NODE_TYPE_LIBRARY_MENU, library_menu_node_data);
	const char * library_menu_title = gettext("Library");
	SDL_Surface * library_menu_icon = SVG_Load("/usr/share/icons/library.svg", (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor, (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor, &this->context->colors.icon.foreground);
	SDL_Surface * library_menu_icon_small = SVG_Load("/usr/share/icons/library.svg", (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor * UNFOCUSED_PAGING_CARD_WIDTH_SCALE_FACTOR, (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor * UNFOCUSED_PAGING_CARD_WIDTH_SCALE_FACTOR, &this->context->colors.icon.foreground);
	library_menu->normal_surface = icon_paging_card(library_menu_icon_small, VERTICAL_ALIGNMENT_CENTER, localization_font_file_path(NULL), localization_font_height(), this->context->colors.legend.foreground, this->context->colors.legend.background, SCREEN_MARGIN * this->context->surfaces.scale_factor, DEFAULT_PAGING_CARD_WIDTH * this->context->surfaces.scale_factor, this->context->surfaces.screen->h * UNFOCUSED_PAGING_CARD_HEIGHT_SCALE_FACTOR, DEFAULT_PAGING_CARD_ROUNDED_CORNER_RADIUS * this->context->surfaces.scale_factor, library_menu_title);
	library_menu->selected_surface = icon_paging_card(library_menu_icon, VERTICAL_ALIGNMENT_CENTER, localization_font_file_path(NULL), localization_font_height(), this->context->colors.legend.foreground, this->context->colors.legend.background, SCREEN_MARGIN * this->context->surfaces.scale_factor, DEFAULT_PAGING_CARD_WIDTH * this->context->surfaces.scale_factor, this->context->surfaces.screen->h * FOCUSED_PAGING_CARD_HEIGHT_SCALE_FACTOR, DEFAULT_PAGING_CARD_ROUNDED_CORNER_RADIUS * this->context->surfaces.scale_factor, library_menu_title);
	library_menu->parent = this;
	SDL_FreeSurface(library_menu_icon);
	SDL_FreeSurface(library_menu_icon_small);

	// Create the RetroArch menu
	struct gui_node * retroarch_menu = gui_create_node(this->context, gui_activate_retroarch_menu, NULL, NULL, NULL, gui_invalidate_retroarch, NODE_TYPE_RETROARCH_MENU, NULL);
	const char * retroarch_menu_title = gettext("RetroArch");
	SDL_Surface * retroarch_menu_icon = SVG_Load("/usr/share/icons/retroarch.svg", (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor, (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor, &this->context->colors.icon.foreground);
	SDL_Surface * retroarch_menu_icon_small = SVG_Load("/usr/share/icons/retroarch.svg", (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor * UNFOCUSED_PAGING_CARD_WIDTH_SCALE_FACTOR, (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor * UNFOCUSED_PAGING_CARD_WIDTH_SCALE_FACTOR, &this->context->colors.icon.foreground);
	retroarch_menu->normal_surface = icon_paging_card(retroarch_menu_icon_small, VERTICAL_ALIGNMENT_CENTER, localization_font_file_path(NULL), localization_font_height(), this->context->colors.legend.foreground, this->context->colors.legend.background, SCREEN_MARGIN * this->context->surfaces.scale_factor, DEFAULT_PAGING_CARD_WIDTH * this->context->surfaces.scale_factor, this->context->surfaces.screen->h * UNFOCUSED_PAGING_CARD_HEIGHT_SCALE_FACTOR, DEFAULT_PAGING_CARD_ROUNDED_CORNER_RADIUS * this->context->surfaces.scale_factor, retroarch_menu_title);
	retroarch_menu->selected_surface = icon_paging_card(retroarch_menu_icon, VERTICAL_ALIGNMENT_CENTER, localization_font_file_path(NULL), localization_font_height(), this->context->colors.legend.foreground, this->context->colors.legend.background, SCREEN_MARGIN * this->context->surfaces.scale_factor, DEFAULT_PAGING_CARD_WIDTH * this->context->surfaces.scale_factor, this->context->surfaces.screen->h * FOCUSED_PAGING_CARD_HEIGHT_SCALE_FACTOR, DEFAULT_PAGING_CARD_ROUNDED_CORNER_RADIUS * this->context->surfaces.scale_factor, retroarch_menu_title);
	retroarch_menu->parent = this;
	SDL_FreeSurface(retroarch_menu_icon);
	SDL_FreeSurface(retroarch_menu_icon_small);

	// Link the menus together
	continue_menu->next = favorites_menu;
	continue_menu->previous = retroarch_menu;
	favorites_menu->next = library_menu;
	favorites_menu->previous = continue_menu;
	library_menu->next = retroarch_menu;
	library_menu->previous = favorites_menu;
	retroarch_menu->next = continue_menu;
	retroarch_menu->previous = library_menu;

	// Auto-select the continue menu by default
	this->child = continue_menu;

	// And keep track of it for moving pointer menus
	this->children = this->child;
}

/**
 * @brief Deactivates the main menu.
 */
static void gui_deactivate_main_menu(struct gui_node * this)
{
	// TODO: Clean up here maybe...
	// It's questionable how useful this will be as the only times we land here is when the application exits.
	// And when it does the OS garbage collects all heaps anyway.
}

/**
 * @brief Re-renders the main menu node's surfaces.
 */
static void gui_invalidate_main_menu(struct gui_node * this)
{
	// Cast the type specific node data
	struct gui_menu_node_data * data = (struct gui_menu_node_data *)this->data;

	// We have a valid menu node data structure
	if (data != NULL)
	{
		// We have a no longer guaranteed to be valid menu title surface
		if (data->title.surface != NULL)
		{
			// Free the menu title surface
			SDL_FreeSurface(data->title.surface);

			// And NULL its reference
			data->title.surface = NULL;
		}
	}

	// Allocate a temporary buffer to hold the full menu title
	char * title = malloc(strlen(gettext("GarlicOS %s")) + strlen(VERSION));

	// We managed to allocate a temporary buffer to hold the full menu title
	if (title != NULL)
	{
		// Puzzle together the full menu title
		sprintf(title, gettext("GarlicOS %s"), VERSION);

		// Render the title text surface
		gui_activate_menu(this, title);

		// Free the temporary buffer
		free(title);
	}
}

#endif