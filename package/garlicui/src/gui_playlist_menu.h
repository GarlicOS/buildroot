#ifndef GUI_PLAYLIST_MENU
#define GUI_PLAYLIST_MENU

/**
 * @brief Creates a thumbnail node.
 */
static struct gui_node * gui_playlist_create_thumbnail_node(struct gui_node * this, struct gui_node ** first_node, struct gui_node ** previous_node, const char * path, double focused_height, double unfocused_height, SDL_Surface * focused_screenshot_fallback, SDL_Surface * unfocused_screenshot_fallback, callback activate, callback deactivate, callback update, callback render, callback invalidate)
{
	// The created node
	struct gui_node * result = NULL;

	// Get the given file's display name
	char * display_name = io_get_display_name(path);

	// We managed to get the given file's display name
	if (display_name != NULL)
	{
		// Get the screenshot path
		char * screenshot_path = retroarch_get_auto_savestate_screenshot_path(path);

		// We managed to get the screenshot path
		if (screenshot_path != NULL)
		{
			// Load the screenshot
			SDL_Surface * screenshot = SDL_ConvertToRGBASurface(IMG_Load(screenshot_path), 1);

			// Free the screenshot path
			free(screenshot_path);

			// We failed to load a auto-savestate screenshot
			if (screenshot == NULL)
			{
				// Try a retroarch title screenshot next
				screenshot_path = retroarch_get_thumbnail_path(path, RETROARCH_NAMED_TITLES);

				// We managed to get the retroarch title screenshot path
				if (screenshot_path != NULL)
				{
					// Load the title screenshot
					screenshot = SDL_ConvertToRGBASurface(IMG_Load(screenshot_path), 1);

					// Free the retroarch title screenshot path
					free(screenshot_path);
				}
			}

			// The focused size screenshot
			SDL_Surface * focused_screenshot = NULL;

			// The unfocused size screenshot
			SDL_Surface * unfocused_screenshot = NULL;

			// We managed to load the screenshot
			if (screenshot != NULL)
			{
				// Calculate the focused scale factor
				double focused_scale_factor = focused_height / screenshot->h;

				// Calculate the unfocused scale factor
				double unfocused_scale_factor = unfocused_height / screenshot->h;

				// Scale the screenshot to the focused height
				focused_screenshot = zoomSurface(screenshot, focused_scale_factor, focused_scale_factor, 1);

				// We managed to scale the screenshot to the focused height
				if (focused_screenshot != NULL)
				{
					// Apply a vertical fade effect to the focused screenshot
					SDL_VerticalFade(focused_screenshot, 0.75, 0.15);
				}

				// Scale the screenshot to the unfocused height
				unfocused_screenshot = zoomSurface(screenshot, unfocused_scale_factor, unfocused_scale_factor, 1);

				// We managed to scale the screenshot to the unfocused height
				if (unfocused_screenshot != NULL)
				{
					// Apply a vertical fade effect to the unfocused screenshot
					SDL_VerticalFade(unfocused_screenshot, 0.75, 0.15);
				}

				// Free the screenshot surface
				SDL_FreeSurface(screenshot);
			}

			// We failed to load the screenshot
			else
			{
				// Set the focused fallback image
				focused_screenshot = focused_screenshot_fallback;

				// Set the unfocused fallback image
				unfocused_screenshot = unfocused_screenshot_fallback;
			}

			// We managed to render the focused and unfocused screenshot
			if (focused_screenshot != NULL && unfocused_screenshot != NULL)
			{
				// Allocate memory for the additional menu data
				struct gui_path_menu_node_data * item_menu_node_data = calloc(1, sizeof(struct gui_path_menu_node_data));

				// We managed to allocate memory for the additional menu data
				if (item_menu_node_data != NULL)
				{
					// Copy the item path into the additional menu data structure
					strcpy(item_menu_node_data->path, path);

					// Get the directory name of the content item
					char * directory_name = io_get_directory_name(path);

					// We managed to get the directory name of the content item
					if (directory_name != NULL)
					{
						// Add the folder configuration to the additional menu data structure
						item_menu_node_data->configuration = io_read_folder_configuration(directory_name);

						// Free the directory name buffer
						free(directory_name);
					}

					// Create the menu node
					struct gui_node * node = gui_create_node(this->context, activate, deactivate, update, render, invalidate, NODE_TYPE_LIBRARY_FILE, item_menu_node_data);

					// We managed to create the menu node
					if (node != NULL)
					{
						// The node population status
						int populated = 0;

						// Pre-render the normal surface
						node->normal_surface = icon_paging_card(unfocused_screenshot, VERTICAL_ALIGNMENT_CENTER, localization_font_file_path(NULL), localization_font_height(), this->context->colors.legend.foreground, this->context->colors.legend.background, SCREEN_MARGIN * this->context->surfaces.scale_factor, unfocused_screenshot->w + (2 * SCREEN_MARGIN * this->context->surfaces.scale_factor), unfocused_screenshot->h + (2 * SCREEN_MARGIN * this->context->surfaces.scale_factor), DEFAULT_PAGING_CARD_ROUNDED_CORNER_RADIUS * this->context->surfaces.scale_factor, display_name);

						// We managed to pre-render the normal surface
						if (node->normal_surface != NULL)
						{
							// Pre-render the selected surface
							node->selected_surface = icon_paging_card(focused_screenshot, VERTICAL_ALIGNMENT_CENTER, localization_font_file_path(NULL), localization_font_height(), this->context->colors.legend.foreground, this->context->colors.legend.background, SCREEN_MARGIN * this->context->surfaces.scale_factor, focused_screenshot->w + (2 * SCREEN_MARGIN * this->context->surfaces.scale_factor), focused_screenshot->h + (2 * SCREEN_MARGIN * this->context->surfaces.scale_factor), DEFAULT_PAGING_CARD_ROUNDED_CORNER_RADIUS * this->context->surfaces.scale_factor, display_name);

							// We managed to pre-render the selected surface
							if (node->selected_surface != NULL)
							{
								// Link the node to its parent
								node->parent = this;

								// Link the sibling nodes together
								if (previous_node != NULL && *previous_node != NULL)
								{
									// Link the previous node to the current one
									(*previous_node)->next = node;

									// Link the current node to the previous one
									node->previous = *previous_node;
								}

								// Mark the node as populated
								populated = 1;

								// We've been handed a valid previous node pointer
								if (previous_node != NULL)
								{
									// Update the previous node
									*previous_node = node;
								}

								// We haven't set the first node yet
								if (first_node != NULL && *first_node == NULL)
								{
									// Keep track of the first node
									*first_node = node;
								}

								// Return the created node
								result = node;
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

					// We failed to create the menu node
					else
					{
						// Free the additional menu data
						free(item_menu_node_data);
					}
				}
			}

			// We have a focused size screenshot surface
			if (focused_screenshot != NULL && focused_screenshot != focused_screenshot_fallback)
			{
				// Free the focused size screenshot surface
				SDL_FreeSurface(focused_screenshot);
			}

			// We have a unfocused size screenshot surface
			if (unfocused_screenshot != NULL && unfocused_screenshot != unfocused_screenshot_fallback)
			{
				// Free the unfocused size screenshot surface
				SDL_FreeSurface(unfocused_screenshot);
			}
		}

		// Free the display name
		free(display_name);
	}

	// Return the created node
	return result;
}

/**
 * @brief Re-renders the playlist thumbnail node's surfaces.
 */
static void gui_invalidate_playlist_thumbnail(struct gui_node * this)
{
	// Cast the type specific node data
	struct gui_path_menu_node_data * data = (struct gui_path_menu_node_data *)this->data;

	// We have valid path data
	if (data != NULL && data->path != NULL)
	{
		// Calculate the focused height
		double focused_height = this->context->surfaces.screen->h * FOCUSED_PAGING_CARD_HEIGHT_SCALE_FACTOR - (2 * SCREEN_MARGIN * this->context->surfaces.scale_factor);

		// Calculate the unfocused height
		double unfocused_height = this->context->surfaces.screen->h * UNFOCUSED_PAGING_CARD_HEIGHT_SCALE_FACTOR - (2 * SCREEN_MARGIN * this->context->surfaces.scale_factor);

		// The focused height fallback icon
		SDL_Surface * focused_screenshot_fallback = NULL;

		// The unfocused height fallback icon
		SDL_Surface * unfocused_screenshot_fallback = NULL;

		// Render the focused height search icon
		SDL_Surface * focused_screenshot_fallback_icon = icon_load("search.svg", focused_height * 0.6f, focused_height * 0.6f, &this->context->colors.legend.foreground);

		// We managed to render the focused height search icon
		if (focused_screenshot_fallback_icon != NULL)
		{
			// Create a focused height canvas surface
			focused_screenshot_fallback = SDL_CreateRGBSurface(SDL_SWSURFACE, focused_height, focused_height, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);

			// Render the search icon in the center of the canvas
			SDL_Rect focused_screenshot_fallback_position;
			focused_screenshot_fallback_position.x = ((int)focused_height - focused_screenshot_fallback_icon->w) >> 1;
			focused_screenshot_fallback_position.y = ((int)focused_height - focused_screenshot_fallback_icon->w) >> 1;
			focused_screenshot_fallback_position.w = focused_screenshot_fallback_icon->w;
			focused_screenshot_fallback_position.h = focused_screenshot_fallback_icon->h;
			SDL_BlitSurfaceAlpha(focused_screenshot_fallback_icon, NULL, focused_screenshot_fallback, &focused_screenshot_fallback_position);

			// Free the focused height search icon
			SDL_FreeSurface(focused_screenshot_fallback_icon);

			// Calculate the shrink factor
			double shrink_factor = unfocused_height / focused_height;

			// Scale the screenshot to the focused height
			unfocused_screenshot_fallback = zoomSurface(focused_screenshot_fallback, shrink_factor, shrink_factor, 1);
		}

		// Create a temporary thumbnail node (from which we'll steal the surfaces from)
		struct gui_node * node = gui_playlist_create_thumbnail_node(this, NULL, NULL, data->path, focused_height, unfocused_height, focused_screenshot_fallback, unfocused_screenshot_fallback, NULL, NULL, NULL, NULL, NULL);

		// We managed to create a temporary thumbnail node
		if (node != NULL)
		{
			// We have an existing selected surface
			if (this->selected_surface != NULL)
			{
				// Free the existing selected surface
				SDL_FreeSurface(this->selected_surface);
			}

			// Replace the existing selected surface
			this->selected_surface = node->selected_surface;

			// We have an existing normal surface
			if (this->normal_surface != NULL)
			{
				// Free the existing normal surface
				SDL_FreeSurface(this->normal_surface);
			}

			// Replace the existing normal surface
			this->normal_surface = node->normal_surface;

			// Free the temporary thumbnail node
			gui_destroy_node(node, 1);
		}

		// We have a focused fallback icon
		if (focused_screenshot_fallback != NULL)
		{
			// Free the focused fallback icon surface
			SDL_FreeSurface(focused_screenshot_fallback);
		}

		// We have a unfocused fallback icon
		if (unfocused_screenshot_fallback != NULL)
		{
			// Free the unfocused fallback icon surface
			SDL_FreeSurface(unfocused_screenshot_fallback);
		}
	}
}

/**
 * @brief Re-renders the no game warning message node's surfaces.
 */
static void gui_invalidate_no_game_warning_message(struct gui_node * this)
{
	// Get the localized warning message
	const char * warning_message = gettext("No games found!");

	// Re-render the normal surface
	SDL_Surface * surface = TTF_RenderUTF8_Blended(this->context->fonts.small, warning_message, SDL_ToSDLColor(this->context->colors.base.foreground));

	// We managed to re-render the normal surface
	if (surface != NULL)
	{
		// We have an existing normal surface
		if (this->normal_surface != NULL)
		{
			// Free the existing surface
			SDL_FreeSurface(this->normal_surface);
		}

		// Replace the existing surface pointer
		this->normal_surface = surface;
	}

	// Re-render the selected surface
	surface = icon_circled_text(this->context->fonts.small, this->context->colors.legend.foreground, this->context->colors.legend.background, warning_message, 0, (localization_font_height() + SCREEN_MARGIN) * this->context->surfaces.scale_factor, (SCREEN_MARGIN >> 1) * this->context->surfaces.scale_factor, NULL, 0);

	// We managed to re-render the selected surface
	if (surface != NULL)
	{
		// We have an existing selected surface
		if (this->selected_surface != NULL)
		{
			// Free the existing surface
			SDL_FreeSurface(this->selected_surface);
		}

		// Replace the existing surface pointer
		this->selected_surface = surface;
	}
}

/**
 * @brief Activates the playlist menu.
 */
static void gui_activate_playlist_menu(struct gui_node * this, const char * title)
{
	// Render the title text surface
	gui_activate_menu(this, title);

	// Cast the continue menu specific data
	struct gui_path_menu_node_data * data = (struct gui_path_menu_node_data *)this->data;

	// The first node
	struct gui_node * first_node = NULL;

	// The previous node
	struct gui_node * previous_node = NULL;

	// The tapped items
	size_t tapped_item_count = 0;
	char ** tapped_items = NULL;

	// Open the playlist file
	FILE * file = fopen(data->path, "r");

	// We managed to open the playlist file
	if (file != NULL)
	{
		// Determine the playlist file size
		fseek(file, 0, SEEK_END);
		long file_size = ftell(file);
		rewind(file);

		// Allocate a buffer for the playlist file content
		char * file_buffer = (char *)malloc(file_size + 1);

		// We managed to allocate a buffer for the playlist file content
		if (file_buffer != NULL)
		{
			// Read the playlist file content
			if (fread(file_buffer, 1, file_size, file) == file_size)
			{
				// Terminate the playlist file content string
				file_buffer[file_size] = 0;

				// Parse the JSON data
				cJSON * json = cJSON_Parse(file_buffer);

				// We managed to parse the JSON data
				if (json != NULL)
				{
					// Get the items object
					cJSON * items = cJSON_GetObjectItem(json, "items");

					// Make sure the items object is an array
					if (cJSON_IsArray(items))
					{
						// Get the array size
						int array_size = cJSON_GetArraySize(items);

						// Calculate the focused height
						double focused_height = this->context->surfaces.screen->h * FOCUSED_PAGING_CARD_HEIGHT_SCALE_FACTOR - (2 * SCREEN_MARGIN * this->context->surfaces.scale_factor);

						// Calculate the unfocused height
						double unfocused_height = this->context->surfaces.screen->h * UNFOCUSED_PAGING_CARD_HEIGHT_SCALE_FACTOR - (2 * SCREEN_MARGIN * this->context->surfaces.scale_factor);

						// The focused height fallback icon
						SDL_Surface * focused_screenshot_fallback = NULL;

						// The unfocused height fallback icon
						SDL_Surface * unfocused_screenshot_fallback = NULL;

						// Render the focused height search icon
						SDL_Surface * focused_screenshot_fallback_icon = icon_load("search.svg", focused_height * 0.6f, focused_height * 0.6f, &this->context->colors.legend.foreground);

						// We managed to render the focused height search icon
						if (focused_screenshot_fallback_icon != NULL)
						{
							// Create a focused height canvas surface
							focused_screenshot_fallback = SDL_CreateRGBSurface(SDL_SWSURFACE, focused_height, focused_height, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);

							// Render the search icon in the center of the canvas
							SDL_Rect focused_screenshot_fallback_position;
							focused_screenshot_fallback_position.x = ((int)focused_height - focused_screenshot_fallback_icon->w) >> 1;
							focused_screenshot_fallback_position.y = ((int)focused_height - focused_screenshot_fallback_icon->w) >> 1;
							focused_screenshot_fallback_position.w = focused_screenshot_fallback_icon->w;
							focused_screenshot_fallback_position.h = focused_screenshot_fallback_icon->h;
							SDL_BlitSurfaceAlpha(focused_screenshot_fallback_icon, NULL, focused_screenshot_fallback, &focused_screenshot_fallback_position);

							// Free the focused height search icon
							SDL_FreeSurface(focused_screenshot_fallback_icon);

							// Calculate the shrink factor
							double shrink_factor = unfocused_height / focused_height;

							// Scale the screenshot to the focused height
							unfocused_screenshot_fallback = zoomSurface(focused_screenshot_fallback, shrink_factor, shrink_factor, 1);
						}

						// Iterate all items
						for (int i = 0; i < array_size; i++)
						{
							// Get the array item
							cJSON * item = cJSON_GetArrayItem(items, i);

							// Get the game path
							cJSON * path = cJSON_GetObjectItem(item, "path");

							// We managed to get the game path
							if (path != NULL && access(path->valuestring, F_OK) == 0)
							{
								// The tapped status of the item
								int tapped = 0;

								// Iterate the tapped item paths
								for (size_t tap_index = 0; tap_index < tapped_item_count; tap_index++)
								{
									// Compare the item paths
									if (strcmp(tapped_items[tap_index], path->valuestring) == 0)
									{
										// Mark the item as tapped
										tapped = 1;
									}
								}

								// We haven't tapped this item yet
								if (!tapped)
								{
									// Create the thumbnail node
									struct gui_node * node = gui_playlist_create_thumbnail_node(this, &first_node, &previous_node, path->valuestring, focused_height, unfocused_height, focused_screenshot_fallback, unfocused_screenshot_fallback, gui_activate_content, NULL, NULL , NULL, gui_invalidate_playlist_thumbnail);

									// Make room for the tapped item pointer
									tapped_items = realloc(tapped_items, sizeof(char *) * ++tapped_item_count);

									// Keep track of the tapped item
									tapped_items[tapped_item_count - 1] = strdup(path->valuestring);
								}
							}
						}

						// We have a focused fallback icon
						if (focused_screenshot_fallback != NULL)
						{
							// Free the focused fallback icon surface
							SDL_FreeSurface(focused_screenshot_fallback);
						}

						// We have a unfocused fallback icon
						if (unfocused_screenshot_fallback != NULL)
						{
							// Free the unfocused fallback icon surface
							SDL_FreeSurface(unfocused_screenshot_fallback);
						}
					}

					// Delete the JSON object
					cJSON_Delete(json);
				}
			}

			// Free the playlist file content buffer
			free(file_buffer);
		}

		// Close the playlist file
		fclose(file);
	}

	// We have no games
	if (first_node == NULL)
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
					node->parent = this;

					// Link the sibling nodes together
					if (previous_node != NULL)
					{
						// Link the previous node to the current one
						previous_node->next = node;

						// Link the current node to the previous one
						node->previous = previous_node;
					}

					// Mark the node as populated
					populated = 1;

					// Update the previous node
					previous_node = node;

					// We haven't set the first node yet
					if (first_node == NULL)
					{
						// Keep track of the first node
						first_node = node;
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

		// Make this node take focus on screen
		this->context->menu.active = this;
	}

	// We have a tap buffer
	if (tapped_items != NULL && tapped_item_count > 0)
	{
		// Iterate the tapped item paths
		for (size_t tap_index = 0; tap_index < tapped_item_count; tap_index++)
		{
			// Free the tapped item path
			free(tapped_items[tap_index]);
		}

		// Free the tapped item array
		free(tapped_items);
	}
}

/**
 * @brief Deactivates the playlist menu.
 */
static void gui_deactivate_playlist_menu(struct gui_node * this)
{
	// The current child node
	struct gui_node * child = this->child;

	// Iterate all children
	while (child != NULL)
	{
		// Cast the child node's additional data structure
		struct gui_path_menu_node_data * data = (struct gui_path_menu_node_data *)child->data;

		// The child node holds a valid folder configuration
		if (data != NULL && data->configuration != NULL)
		{
			// Free the folder configuration
			xmlFreeDoc(data->configuration);

			// And NULL its reference
			data->configuration = NULL;
		}

		// Move to the next child
		child = child->next;

		// We've iterated all children
		if (child == this->child)
		{
			// Break the loop
			break;
		}
	}

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
 * @brief Activates the continue menu.
 */
static void gui_activate_continue_menu(struct gui_node * this)
{
	// Render the title text surface
	gui_activate_playlist_menu(this, gettext("Resume"));
}

/**
 * @brief Re-renders the continue menu node's surfaces.
 */
static void gui_invalidate_continue_menu(struct gui_node * this)
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

	// Get the localized Resume text
	const char * title = gettext("Resume");

	// Render the title text surface
	gui_activate_menu(this, title);

	// Render the normal size resume icon
	SDL_Surface * icon = icon_load("resume.svg", (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor, (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor, &this->context->colors.icon.foreground);

	// We managed to render the normal size resume icon
	if (icon != NULL)
	{
		// Render the paging card
		SDL_Surface * paging_card = icon_paging_card(icon, VERTICAL_ALIGNMENT_CENTER, localization_font_file_path(NULL), localization_font_height(), this->context->colors.legend.foreground, this->context->colors.legend.background, SCREEN_MARGIN * this->context->surfaces.scale_factor, DEFAULT_PAGING_CARD_WIDTH * this->context->surfaces.scale_factor, this->context->surfaces.screen->h * FOCUSED_PAGING_CARD_HEIGHT_SCALE_FACTOR, DEFAULT_PAGING_CARD_ROUNDED_CORNER_RADIUS * this->context->surfaces.scale_factor, title);

		// We managed to render the paging card surface
		if (paging_card != NULL)
		{
			// We have an existing paging card surface
			if (this->selected_surface != NULL)
			{
				// Free the existing paging card surface
				SDL_FreeSurface(this->selected_surface);
			}

			// Replace the existing paging card surface
			this->selected_surface = paging_card;
		}

		// Free the icon surface
		SDL_FreeSurface(icon);
	}

	// Render the small size resume icon
	icon = icon_load("resume.svg", (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor * UNFOCUSED_PAGING_CARD_WIDTH_SCALE_FACTOR, (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor * UNFOCUSED_PAGING_CARD_WIDTH_SCALE_FACTOR, &this->context->colors.icon.foreground);

	// We managed to render the small size resume icon
	if (icon != NULL)
	{
		// Render the paging card
		SDL_Surface * paging_card = icon_paging_card(icon, VERTICAL_ALIGNMENT_CENTER, localization_font_file_path(NULL), localization_font_height(), this->context->colors.legend.foreground, this->context->colors.legend.background, SCREEN_MARGIN * this->context->surfaces.scale_factor, DEFAULT_PAGING_CARD_WIDTH * this->context->surfaces.scale_factor, this->context->surfaces.screen->h * UNFOCUSED_PAGING_CARD_HEIGHT_SCALE_FACTOR, DEFAULT_PAGING_CARD_ROUNDED_CORNER_RADIUS * this->context->surfaces.scale_factor, title);

		// We managed to render the paging card surface
		if (paging_card != NULL)
		{
			// We have an existing paging card surface
			if (this->normal_surface != NULL)
			{
				// Free the existing paging card surface
				SDL_FreeSurface(this->normal_surface);
			}

			// Replace the existing paging card surface
			this->normal_surface = paging_card;
		}

		// Free the icon surface
		SDL_FreeSurface(icon);
	}
}

/**
 * @brief Activates the favorites menu.
 */
static void gui_activate_favorites_menu(struct gui_node * this)
{
	// Render the title text surface
	gui_activate_playlist_menu(this, gettext("Favorites"));
}

/**
 * @brief Re-renders the favorites menu node's surfaces.
 */
static void gui_invalidate_favorites_menu(struct gui_node * this)
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

	// Get the localized Favorites text
	const char * title = gettext("Favorites");

	// Render the title text surface
	gui_activate_menu(this, title);

	// Render the normal size favorite icon
	SDL_Surface * icon = icon_load("favorite.svg", (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor, (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor, &this->context->colors.icon.foreground);

	// We managed to render the normal size favorite icon
	if (icon != NULL)
	{
		// Render the paging card
		SDL_Surface * paging_card = icon_paging_card(icon, VERTICAL_ALIGNMENT_CENTER, localization_font_file_path(NULL), localization_font_height(), this->context->colors.legend.foreground, this->context->colors.legend.background, SCREEN_MARGIN * this->context->surfaces.scale_factor, DEFAULT_PAGING_CARD_WIDTH * this->context->surfaces.scale_factor, this->context->surfaces.screen->h * FOCUSED_PAGING_CARD_HEIGHT_SCALE_FACTOR, DEFAULT_PAGING_CARD_ROUNDED_CORNER_RADIUS * this->context->surfaces.scale_factor, title);

		// We managed to render the paging card surface
		if (paging_card != NULL)
		{
			// We have an existing paging card surface
			if (this->selected_surface != NULL)
			{
				// Free the existing paging card surface
				SDL_FreeSurface(this->selected_surface);
			}

			// Replace the existing paging card surface
			this->selected_surface = paging_card;
		}

		// Free the icon surface
		SDL_FreeSurface(icon);
	}

	// Render the small size favorite icon
	icon = icon_load("favorite.svg", (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor * UNFOCUSED_PAGING_CARD_WIDTH_SCALE_FACTOR, (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor * UNFOCUSED_PAGING_CARD_WIDTH_SCALE_FACTOR, &this->context->colors.icon.foreground);

	// We managed to render the small size favorite icon
	if (icon != NULL)
	{
		// Render the paging card
		SDL_Surface * paging_card = icon_paging_card(icon, VERTICAL_ALIGNMENT_CENTER, localization_font_file_path(NULL), localization_font_height(), this->context->colors.legend.foreground, this->context->colors.legend.background, SCREEN_MARGIN * this->context->surfaces.scale_factor, DEFAULT_PAGING_CARD_WIDTH * this->context->surfaces.scale_factor, this->context->surfaces.screen->h * UNFOCUSED_PAGING_CARD_HEIGHT_SCALE_FACTOR, DEFAULT_PAGING_CARD_ROUNDED_CORNER_RADIUS * this->context->surfaces.scale_factor, title);

		// We managed to render the paging card surface
		if (paging_card != NULL)
		{
			// We have an existing paging card surface
			if (this->normal_surface != NULL)
			{
				// Free the existing paging card surface
				SDL_FreeSurface(this->normal_surface);
			}

			// Replace the existing paging card surface
			this->normal_surface = paging_card;
		}

		// Free the icon surface
		SDL_FreeSurface(icon);
	}
}

#endif