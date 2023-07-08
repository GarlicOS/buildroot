#ifndef GUI_CONTENT_CONTEXT_MENU_H
#define GUI_CONTENT_CONTEXT_MENU_H

/**
 * @brief Resumes the selected game.
 */
static void gui_activate_resume(struct gui_node * this)
{
	// Start the game
	gui_activate_content(this->parent->parent->child);
}

/**
 * @brief Re-renders the resume action's surfaces.
 */
static void gui_invalidate_resume(struct gui_node * this)
{
	// Create a temporary node
	struct gui_node * node = gui_create_context_menu_item(this, NULL, NULL, gettext("Resume"), NULL, NULL, NULL, NULL, NULL, NODE_TYPE_CONTEXT_MENU_LOCALE_MENU, NULL);

	// We managed to create a temporary node
	if (node != NULL)
	{
		// We have a no longer guaranteed to be valid normal surface
		if (this->normal_surface != NULL)
		{
			// Free the normal surface
			SDL_FreeSurface(this->normal_surface);
		}

		// We have a no longer guaranteed to be valid selected surface
		if (this->selected_surface != NULL)
		{
			// Free the selected surface
			SDL_FreeSurface(this->selected_surface);
		}

		// Steal the temporary node's display surfaces
		this->normal_surface = node->normal_surface;
		this->selected_surface = node->selected_surface;

		// NULL the temporary node's display surfaces (so they won't get freed with the rest of the node)
		node->normal_surface = NULL;
		node->selected_surface = NULL;

		// Free the temporay node
		gui_destroy_node(node, 1);
	}
}

/**
 * @brief Restarts the selected game (deletes the auto savestate & starts the game).
 */
static void gui_activate_restart(struct gui_node * this)
{
	// Get the selected item's node data
	struct gui_path_menu_node_data * item_menu_node_data = (struct gui_path_menu_node_data *)this->parent->parent->child->data;

	// We've got valid path data
	if (item_menu_node_data != NULL && item_menu_node_data->path != NULL)
	{
		// Get the item's savestate path
		char * savestate_path = retroarch_get_auto_savestate_path(item_menu_node_data->path);

		// We managed to get the item's savestate path
		if (savestate_path != NULL)
		{
			// Delete the item's savestate
			remove(savestate_path);

			// Flush all pending write operations to disk
			io_sync();

			// Start the game
			gui_activate_content(this->parent->parent->child);
		}
	}
}

/**
 * @brief Re-renders the restart action's surfaces.
 */
static void gui_invalidate_restart(struct gui_node * this)
{
	// Create a temporary node
	struct gui_node * node = gui_create_context_menu_item(this, NULL, NULL, gettext("Restart"), NULL, NULL, NULL, NULL, NULL, NODE_TYPE_CONTEXT_MENU_LOCALE_MENU, NULL);

	// We managed to create a temporary node
	if (node != NULL)
	{
		// We have a no longer guaranteed to be valid normal surface
		if (this->normal_surface != NULL)
		{
			// Free the normal surface
			SDL_FreeSurface(this->normal_surface);
		}

		// We have a no longer guaranteed to be valid selected surface
		if (this->selected_surface != NULL)
		{
			// Free the selected surface
			SDL_FreeSurface(this->selected_surface);
		}

		// Steal the temporary node's display surfaces
		this->normal_surface = node->normal_surface;
		this->selected_surface = node->selected_surface;

		// NULL the temporary node's display surfaces (so they won't get freed with the rest of the node)
		node->normal_surface = NULL;
		node->selected_surface = NULL;

		// Free the temporay node
		gui_destroy_node(node, 1);
	}
}

/**
 * @brief (Un-)favorites the selected game.
 */
static void gui_activate_favorite_toggle(struct gui_node * this)
{
	// Get the selected item's node data
	struct gui_path_menu_node_data * item_menu_node_data = (struct gui_path_menu_node_data *)this->parent->parent->child->data;

	// We've got valid path data
	if (item_menu_node_data != NULL && item_menu_node_data->path != NULL)
	{
		// Check whether the item is favorited or not
		struct retroarch_playlist_item * favorited_item = retroarch_get_playlist_item(RETROARCH_DEFAULT_FAVORITES_PLAYLIST_FILE_PATH, item_menu_node_data->path);

		// Remove all playlist items that match the given path from the favorites playlist
		retroarch_remove_playlist_items_by_path(RETROARCH_DEFAULT_FAVORITES_PLAYLIST_FILE_PATH, item_menu_node_data->path);

		// The item isn't favorited
		if (favorited_item == NULL)
		{
			// Create a playlist item skeleton
			struct retroarch_playlist_item favorite_item_skeleton;

			// Zero its buffer
			memset(&favorite_item_skeleton, 0, sizeof(struct retroarch_playlist_item));

			// Set the playlist item path
			favorite_item_skeleton.path = item_menu_node_data->path;

			// Add the item to the favorites playlist
			retroarch_add_playlist_item(RETROARCH_DEFAULT_FAVORITES_PLAYLIST_FILE_PATH, &favorite_item_skeleton);
		}

		// We have an existing normal surface
		if (this->normal_surface != NULL)
		{
			// Free the normal surface
			SDL_FreeSurface(this->normal_surface);

			// NULL the surface reference
			this->normal_surface = NULL;
		}

		// We have an existing selected surface
		if (this->selected_surface != NULL)
		{
			// Free the selected surface
			SDL_FreeSurface(this->selected_surface);

			// NULL the surface reference
			this->selected_surface = NULL;
		}

		// Pre-render the normal surface
		this->normal_surface = TTF_RenderUTF8_Blended(this->context->fonts.small, gettext(favorited_item != NULL ? "Favorite" : "Unfavorite"), SDL_ToSDLColor(this->context->colors.base.foreground));

		// Pre-render the selected surface
		this->selected_surface = icon_circled_text(this->context->fonts.small, this->context->colors.legend.foreground, this->context->colors.legend.background, gettext(favorited_item != NULL ? "Favorite" : "Unfavorite"), 0, (localization_font_height() + SCREEN_MARGIN) * this->context->surfaces.scale_factor, (SCREEN_MARGIN >> 1) * this->context->surfaces.scale_factor, NULL, 0);

		// The highlighted item is favorited
		if (favorited_item != NULL)
		{
			// Free the playlist item
			retroarch_free_playlist_item(favorited_item);
		}
	}
}

/**
 * @brief Re-renders the (un-)favorite action's surfaces.
 */
static void gui_invalidate_favorite_toggle(struct gui_node * this)
{
	// Cast the content item's additional menu data
	struct gui_path_menu_node_data * item_menu_node_data = (struct gui_path_menu_node_data *)this->parent->parent->child->data;

	// We've got valid path data
	if (item_menu_node_data != NULL && item_menu_node_data->path != NULL)
	{
		// Check whether the item is favorited or not
		struct retroarch_playlist_item * favorited_item = retroarch_get_playlist_item(RETROARCH_DEFAULT_FAVORITES_PLAYLIST_FILE_PATH, item_menu_node_data->path);

		// The highlighted item is favorited
		if (favorited_item != NULL)
		{
			// Free the playlist item
			retroarch_free_playlist_item(favorited_item);
		}

		// Create a temporary node
		struct gui_node * node = gui_create_context_menu_item(this, NULL, NULL, gettext(favorited_item != NULL ? "Unfavorite" : "Favorite"), NULL, NULL, NULL, NULL, NULL, NODE_TYPE_CONTEXT_MENU_LOCALE_MENU, NULL);

		// We managed to create a temporary node
		if (node != NULL)
		{
			// We have a no longer guaranteed to be valid normal surface
			if (this->normal_surface != NULL)
			{
				// Free the normal surface
				SDL_FreeSurface(this->normal_surface);
			}

			// We have a no longer guaranteed to be valid selected surface
			if (this->selected_surface != NULL)
			{
				// Free the selected surface
				SDL_FreeSurface(this->selected_surface);
			}

			// Steal the temporary node's display surfaces
			this->normal_surface = node->normal_surface;
			this->selected_surface = node->selected_surface;

			// NULL the temporary node's display surfaces (so they won't get freed with the rest of the node)
			node->normal_surface = NULL;
			node->selected_surface = NULL;

			// Free the temporay node
			gui_destroy_node(node, 1);
		}
	}
}

#endif