#ifndef GUI_CONTEXT_MENU_H
#define GUI_CONTEXT_MENU_H

/**
 * @brief Creates a context menu item.
 */
static struct gui_node * gui_create_context_menu_item(struct gui_node * this, struct gui_node ** first_node, struct gui_node ** previous_node, const char * text, callback activate, callback deactivate, callback update, callback render, callback invalidate, int type, void * data)
{
	// Create a node for the item
	struct gui_node * node = gui_create_node(this->context, activate, deactivate, update, render, invalidate, type, data);

	// We managed to create a node for the item
	if (node != NULL)
	{
		// The node population status
		int populated = 0;

		// Pre-render the normal surface
		node->normal_surface = TTF_RenderUTF8_Blended(this->context->fonts.small, text, SDL_ToSDLColor(this->context->colors.base.foreground));

		// We managed to pre-render the normal surface
		if (node->normal_surface != NULL)
		{
			// Pre-render the selected surface
			node->selected_surface = icon_circled_text(this->context->fonts.small, this->context->colors.legend.foreground, this->context->colors.legend.background, text, 0, (localization_font_height() + SCREEN_MARGIN) * this->context->surfaces.scale_factor, (SCREEN_MARGIN >> 1) * this->context->surfaces.scale_factor, NULL, 0);

			// We managed to pre-render the selected surface
			if (node->selected_surface != NULL)
			{
				// Link the node to its parent
				node->parent = this;

				// The caller wants us to take care of linking the nodes
				if (previous_node != NULL && *previous_node != NULL)
				{
					// Link the previous node to the current one
					(*previous_node)->next = node;

					// Link the current node to the previous one
					node->previous = *previous_node;
				}

				// Mark the node as populated
				populated = 1;

				// The caller wants us to take care of linking the nodes
				if (previous_node != NULL)
				{
					// Update the previous node
					*previous_node = node;
				}

				// We haven't set the first node yet but need to
				if (first_node != NULL && *first_node == NULL)
				{
					// Keep track of the first node
					*first_node = node;
				}
			}
		}

		// We failed to populate the node
		if (!populated)
		{
			// Destroy the node
			gui_destroy_node(node, 1);

			// NULL the node reference
			node = NULL;
		}
	}

	// Return the node
	return node;
}

#include "gui_content_context_menu.h"
#include "gui_rtc_context_menu.h"
#include "gui_locale_context_menu.h"

/**
 * @brief Activates the context menu.
 */
static void gui_activate_context_menu(struct gui_node * this)
{
	// Render the title text surface
	gui_activate_menu(this, gettext("Menu"));

	// The first node
	struct gui_node * first_node = NULL;

	// The previous node
	struct gui_node * previous_node = NULL;

	// Add context specific settings
	switch (this->parent->type)
	{
		// We're inside a content-serving menu (library directory or a continue / favorites playlist)
		case NODE_TYPE_LIBRARY_DIRECTORY_MENU:
		case NODE_TYPE_PLAYLIST_CONTINUE_MENU:
		case NODE_TYPE_PLAYLIST_FAVORITES_MENU:
		{
			// We're currently highlighting content
			if (this->parent->child != NULL)
			{
				// Cast the content item's additional menu data
				struct gui_path_menu_node_data * item_menu_node_data = (struct gui_path_menu_node_data *)this->parent->child->data;

				// We've got valid path data
				if (item_menu_node_data != NULL && item_menu_node_data->path != NULL)
				{
					// Check whether the item is favorited or not
					struct retroarch_playlist_item * favorited_item = retroarch_get_playlist_item(RETROARCH_DEFAULT_FAVORITES_PLAYLIST_FILE_PATH, item_menu_node_data->path);

					// Create a node for resuming
					struct gui_node * resume_node = gui_create_context_menu_item(this, &first_node, &previous_node, gettext("Resume"), gui_activate_resume, NULL, NULL, NULL, gui_invalidate_resume, NODE_TYPE_CONTEXT_MENU_RESUME_ACTION, NULL);

					// Create a node for resetting
					struct gui_node * reset_node = gui_create_context_menu_item(this, &first_node, &previous_node, gettext("Restart"), gui_activate_restart, NULL, NULL, NULL, gui_invalidate_restart, NODE_TYPE_CONTEXT_MENU_RESTART_ACTION, NULL);

					// We're in the resume menu
					if (this->parent->type == NODE_TYPE_PLAYLIST_CONTINUE_MENU)
					{
						// Create a node for removing items from the history playlist
						struct gui_node * remove_from_history_playlist_node = gui_create_context_menu_item(this, &first_node, &previous_node, gettext("Remove"), gui_activate_history_remove, NULL, NULL, NULL, gui_invalidate_history_remove, NODE_TYPE_CONTEXT_MENU_REMOVE_FROM_HISTORY_ACTION, NULL);
					}

					// Create a node for toggling favorites
					struct gui_node * favorite_toggle_node = gui_create_context_menu_item(this, &first_node, &previous_node, gettext(favorited_item != NULL ? "Unfavorite" : "Favorite"), gui_activate_favorite_toggle, NULL, NULL, NULL, gui_invalidate_favorite_toggle, NODE_TYPE_CONTEXT_MENU_FAVORITE_ACTION, NULL);

					// The highlighted item is favorited
					if (favorited_item != NULL)
					{
						// Free the playlist item
						retroarch_free_playlist_item(favorited_item);
					}
				}
			}
		}

		// Add global settings (date & time, language, etc.)
		default:
		{
			// Allocate the additional RTC menu data
			struct gui_rtc_menu_node_data * rtc_menu_node_data = calloc(1, sizeof(struct gui_rtc_menu_node_data));

			// We managed to allocate the additional RTC menu data
			if (rtc_menu_node_data != NULL)
			{
				// Create a node for the date & time menu
				struct gui_node * rtc_menu_node = gui_create_context_menu_item(this, &first_node, &previous_node, gettext("Change Date & Time"), gui_activate_rtc_menu, gui_deactivate_rtc_menu, gui_update_rtc_menu, gui_render_paging_icon_menu, gui_invalidate_rtc_menu, NODE_TYPE_CONTEXT_MENU_DATE_AND_TIME_MENU, rtc_menu_node_data);

				// We failed to create a node for the date & time menu
				if (rtc_menu_node == NULL)
				{
					// Free the additional menu data
					free(rtc_menu_node_data);
				}
			}

			// Allocate the additional locale menu data
			struct gui_locale_menu_node_data * locale_menu_node_data = calloc(1, sizeof(struct gui_locale_menu_node_data));

			// We managed to allocate the additional locale menu data
			if (locale_menu_node_data != NULL)
			{
				// Create a node for the locale menu
				struct gui_node * locale_menu_node = gui_create_context_menu_item(this, &first_node, &previous_node, gettext("Change Language"), gui_activate_locale_menu, gui_deactivate_locale_menu, gui_update_basic_menu, gui_render_vertical_scrolling_list_menu, gui_invalidate_locale_menu, NODE_TYPE_CONTEXT_MENU_LOCALE_MENU, locale_menu_node_data);

				// We failed to create a node for the locale menu
				if (locale_menu_node == NULL)
				{
					// Free the additional menu data
					free(locale_menu_node_data);
				}
			}
		}
	}

	// Link the first & last nodes together
	if (first_node != NULL)
	{
		// Link the first node to the last node
		first_node->previous = previous_node;

		// Link the last node to the first node
		previous_node->next = first_node;

		// Link this node to its first child node
		this->child = first_node;

		// And keep track of it for moving pointer menus
		this->children = this->child;
	}

	// Make this node take focus on screen
	this->context->menu.active = this;
}

/**
 * @brief Deactivates the context menu.
 */
static void gui_deactivate_context_menu(struct gui_node * this)
{
	// Destroy the node's children
	gui_destroy_node(this, 0);

	// Free the title text surface
	gui_deactivate_menu(this);

	// We have a parent node to return to
	if (this->parent != NULL)
	{
		// And return on screen focus to the parent node
		this->context->menu.active = this->parent;
	}
}

/**
 * @brief Re-renders the context menu's surfaces.
 */
static void gui_invalidate_context_menu(struct gui_node * this)
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

	// Re-render the title text surface
	gui_activate_menu(this, gettext("Menu"));
}

#endif
