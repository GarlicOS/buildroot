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

		// The item wasn't favorited
		if (favorited_item == NULL)
		{
			// Create a playlist item skeleton
			struct retroarch_playlist_item favorite_item_skeleton;

			// Zero its buffer
			memset(&favorite_item_skeleton, 0, sizeof(struct retroarch_playlist_item));

			// Set the playlist item path
			favorite_item_skeleton.path = item_menu_node_data->path;

			// The favorites playlist file stat structure
			struct stat playlist_file_stat;

			// Check if the favorites playlist file exists by populating its file stat structure
			if (stat(RETROARCH_DEFAULT_FAVORITES_PLAYLIST_FILE_PATH, &playlist_file_stat) != 0)
			{
				// Open the favorites playlist file
				FILE * playlist_file = fopen(RETROARCH_DEFAULT_FAVORITES_PLAYLIST_FILE_PATH, "w");

				// We managed to open the favorites playlist file for writing
				if (playlist_file != NULL)
				{
					// Seed a new favorites playlist file
					fputs("{\"version\":\"1.5\",\"default_core_path\":\"\",\"default_core_name\":\"\",\"label_display_mode\":0,\"right_thumbnail_mode\":0,\"left_thumbnail_mode\":0,\"sort_mode\":0,\"items\":[]}", playlist_file);

					// Close the favorites playlist file
					fclose(playlist_file);
				}
			}

			// Add the item to the favorites playlist
			retroarch_add_playlist_item(RETROARCH_DEFAULT_FAVORITES_PLAYLIST_FILE_PATH, &favorite_item_skeleton);
		}

		// The item was favorited
		else
		{
			// Get the context menu's parent menu node
			struct gui_node * menu = this->parent->parent;

			// The context menu is running on top of the favorites playlist menu
			if (menu->type == NODE_TYPE_PLAYLIST_FAVORITES_MENU)
			{
				// Get the currently selected playlist item
				struct gui_node * unfavorited_item = menu->child;

				// We have more than one item in the playlist menu
				if (unfavorited_item->next != unfavorited_item)
				{
					// Unlink the unfavorited item
					unfavorited_item->previous->next = unfavorited_item->next;
					unfavorited_item->next->previous = unfavorited_item->previous;

					// Select the previous item
					menu->child = menu->child->previous;
				}

				// This was the last item in the playlist menu
				else
				{
					// Create the menu node
					struct gui_node * node = gui_create_node(this->context, NULL, NULL, NULL, NULL, gui_invalidate_no_game_warning_message, NODE_TYPE_NO_GAME_WARNING_MESSAGE, NULL);

					// We managed to create the menu node
					if (node != NULL)
					{
						// The node population status
						int populated = 0;

						// Get the localized warning message
						const char * warning_message = gettext("No games found!");

						// Pre-render the normal surface
						node->normal_surface = TTF_RenderUTF8_Blended(this->context->fonts.small, warning_message, SDL_ToSDLColor(this->context->colors.base.foreground));

						// We managed to pre-render the normal surface
						if (node->normal_surface != NULL)
						{
							// Pre-render the selected surface
							node->selected_surface = icon_circled_text(this->context->fonts.small, this->context->colors.legend.foreground, this->context->colors.legend.background, warning_message, 0, (localization_font_height() + SCREEN_MARGIN) * this->context->surfaces.scale_factor, (SCREEN_MARGIN >> 1) * this->context->surfaces.scale_factor, NULL, 0);

							// We managed to pre-render the selected surface
							if (node->selected_surface != NULL)
							{
								// Link the node to its parent
								node->parent = menu;

								// Loop it around itself
								node->next = node;
								node->previous = node;

								// And set it up to be the sole child of the playlist menu
								menu->child = node;

								// Mark the node as populated
								populated = 1;
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
				}

				// Destroy the unfavorited item
				gui_destroy_node(unfavorited_item, 1);
			}
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

/**
 * @brief Removes the selected game from the history.
 */
static void gui_activate_history_remove(struct gui_node * this)
{
	// Get the selected item's node data
	struct gui_path_menu_node_data * item_menu_node_data = (struct gui_path_menu_node_data *)this->parent->parent->child->data;

	// We've got valid path data
	if (item_menu_node_data != NULL && item_menu_node_data->path != NULL)
	{
		// Check whether the item is part of the history playlist
		struct retroarch_playlist_item * history_item = retroarch_get_playlist_item(RETROARCH_DEFAULT_HISTORY_PLAYLIST_FILE_PATH, item_menu_node_data->path);

		// Remove all playlist items that match the given path from the history playlist
		retroarch_remove_playlist_items_by_path(RETROARCH_DEFAULT_HISTORY_PLAYLIST_FILE_PATH, item_menu_node_data->path);

		// The item was on the history playlist
		if (history_item != NULL)
		{
			// Get the context menu's parent menu node
			struct gui_node * menu = this->parent->parent;

			// The context menu is running on top of the resume playlist menu
			if (menu->type == NODE_TYPE_PLAYLIST_CONTINUE_MENU)
			{
				// Get the currently selected playlist item
				struct gui_node * removed_item = menu->child;

				// We have more than one item in the playlist menu
				if (removed_item->next != removed_item)
				{
					// Unlink the removed item
					removed_item->previous->next = removed_item->next;
					removed_item->next->previous = removed_item->previous;

					// Select the previous item
					menu->child = menu->child->previous;
				}

				// This was the last item in the playlist menu
				else
				{
					// Create the menu node
					struct gui_node * node = gui_create_node(this->context, NULL, NULL, NULL, NULL, gui_invalidate_no_game_warning_message, NODE_TYPE_NO_GAME_WARNING_MESSAGE, NULL);

					// We managed to create the menu node
					if (node != NULL)
					{
						// The node population status
						int populated = 0;

						// Get the localized warning message
						const char * warning_message = gettext("No games found!");

						// Pre-render the normal surface
						node->normal_surface = TTF_RenderUTF8_Blended(this->context->fonts.small, warning_message, SDL_ToSDLColor(this->context->colors.base.foreground));

						// We managed to pre-render the normal surface
						if (node->normal_surface != NULL)
						{
							// Pre-render the selected surface
							node->selected_surface = icon_circled_text(this->context->fonts.small, this->context->colors.legend.foreground, this->context->colors.legend.background, warning_message, 0, (localization_font_height() + SCREEN_MARGIN) * this->context->surfaces.scale_factor, (SCREEN_MARGIN >> 1) * this->context->surfaces.scale_factor, NULL, 0);

							// We managed to pre-render the selected surface
							if (node->selected_surface != NULL)
							{
								// Link the node to its parent
								node->parent = menu;

								// Loop it around itself
								node->next = node;
								node->previous = node;

								// And set it up to be the sole child of the playlist menu
								menu->child = node;

								// Mark the node as populated
								populated = 1;
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
				}

				// Destroy the unfavorited item
				gui_destroy_node(removed_item, 1);
			}
		}

		// Close the context menu
		this->parent->deactivate(this->parent);
	}
}

/**
 * @brief Re-renders the remove game from history action's surfaces.
 */
static void gui_invalidate_history_remove(struct gui_node * this)
{
	// Cast the content item's additional menu data
	struct gui_path_menu_node_data * item_menu_node_data = (struct gui_path_menu_node_data *)this->parent->parent->child->data;

	// We've got valid path data
	if (item_menu_node_data != NULL && item_menu_node_data->path != NULL)
	{
		// Create a temporary node
		struct gui_node * node = gui_create_context_menu_item(this, NULL, NULL, gettext("Remove"), NULL, NULL, NULL, NULL, NULL, NODE_TYPE_CONTEXT_MENU_LOCALE_MENU, NULL);

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