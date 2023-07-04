#ifndef GUI_LOCALE_CONTEXT_MENU_H
#define GUI_LOCALE_CONTEXT_MENU_H

/**
 * @brief Creates a locale context menu item.
 */
static struct gui_node * gui_create_locale_menu_item(struct gui_node * this, struct gui_node ** first_node, struct gui_node ** previous_node, const char * locale, callback activate, callback deactivate, callback update, callback render, callback invalidate, int type)
{
	// The created node
	struct gui_node * node;

	// Get the locale's display name
	char * display_name = localization_get_language_display_name(locale, locale);

	// We managed to get the locale's display name
	if (display_name != NULL)
	{
		// Find the locale's country suffix
		char * country_suffix = strstr(locale, "_");

		// The locale contains a country suffix
		if (country_suffix != NULL)
		{
			// Skip the separator
			country_suffix++;

			// We have a valid country suffix
			if (strlen(country_suffix) > 0)
			{
				// Resize the display name buffer
				char * resized_display_name = realloc(display_name, strlen(display_name) + 2 /* " (" */ + strlen(country_suffix) + 2 /* ")\0" */);

				// We managed to resize the display name buffer
				if (resized_display_name != NULL)
				{
					// Some libc implementations allow for buffer relocations, thus replace the pointer
					display_name = resized_display_name;

					// Add the country suffix
					sprintf(display_name + strlen(display_name), " (%s)", country_suffix);
				}
			}
		}

		// Allocate the locale menu node data buffer
		struct gui_locale_menu_node_data * locale_menu_node_data = (struct gui_locale_menu_node_data *)calloc(1, sizeof(struct gui_locale_menu_node_data));

		// We managed to allocate the locale menu node data buffer
		if (locale_menu_node_data != NULL)
		{
			// Set the locale
			strcpy(locale_menu_node_data->locale, locale);

			// Create a node for the item
			node = gui_create_node(this->context, activate, deactivate, update, render, invalidate, type, locale_menu_node_data);

			// We managed to create a node for the item
			if (node != NULL)
			{
				// The node population status
				int populated = 0;

				// Figure out which font the given locale prefers
				const char * font_file_path = localization_font_file_path(locale);

				// Open the given locale's font
				TTF_Font * font = TTF_OpenFont(font_file_path, localization_font_height() * SMALL_FONT_SCALE_FACTOR * this->context->surfaces.scale_factor);

				// We managed to open the given locale's font
				if (font != NULL)
				{
					// Pre-render the normal surface
					node->normal_surface = TTF_RenderUTF8_Blended(font, display_name, SDL_ToSDLColor(this->context->colors.base.foreground));

					// We managed to pre-render the normal surface
					if (node->normal_surface != NULL)
					{
						// Pre-render the selected surface
						node->selected_surface = icon_circled_text(font, this->context->colors.legend.foreground, this->context->colors.legend.background, display_name, 0, (localization_font_height() + SCREEN_MARGIN) * this->context->surfaces.scale_factor, (SCREEN_MARGIN >> 1) * this->context->surfaces.scale_factor, NULL, 0);

						// We managed to pre-render the selected surface
						if (node->selected_surface != NULL)
						{
							// Link the node to its parent
							node->parent = this;

							// Link the sibling nodes together
							if (*previous_node != NULL)
							{
								// Link the previous node to the current one
								(*previous_node)->next = node;

								// Link the current node to the previous one
								node->previous = *previous_node;
							}

							// Mark the node as populated
							populated = 1;

							// Update the previous node
							*previous_node = node;

							// We haven't set the first node yet
							if (*first_node == NULL)
							{
								// Keep track of the first node
								*first_node = node;
							}
						}
					}

					// Close the given locale's font
					TTF_CloseFont(font);
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

			// We failed to create a node for the item
			else
			{
				// Free the additional node data
				free(locale_menu_node_data);
			}
		}

		// Free the locale's display name
		free(display_name);
	}

	// Return the created node
	return node;
}

/**
 * @brief Sets the to-be-applied / saved new locale.
 */
static void gui_change_locale(struct gui_node * this)
{
	// Get the selected locale's node data
	struct gui_locale_menu_node_data * locale_node_data = (struct gui_locale_menu_node_data *)this->data;

	// Get the locale menu's node data
	struct gui_locale_menu_node_data * locale_menu_node_data = (struct gui_locale_menu_node_data *)this->parent->data;

	// Set the locale
	strcpy(locale_menu_node_data->locale, locale_node_data->locale);

	// Deactivate the locale menu's node (which will set & flush the locale setting to disk)
	this->parent->deactivate(this->parent);
}

/**
 * @brief Activates the locale context menu.
 */
static void gui_activate_locale_menu(struct gui_node * this)
{
	// Get the locale menu node data
	struct gui_locale_menu_node_data * locale_menu_node_data = (struct gui_locale_menu_node_data *)this->data;

	// Get the current locale
	strcpy(locale_menu_node_data->locale, setlocale(LC_ALL, NULL));

	// Render the title text surface
	gui_activate_menu(this, gettext("Change Language"));

	// The first node
	struct gui_node * first_node = NULL;

	// The previous node
	struct gui_node * previous_node = NULL;

	// The supported locales
	char ** locales = NULL;

	// Get the supported locales
	int num_locales = localization_get_supported_locales(&locales);

	// We managed to get the supported locales
	if (locales != NULL)
	{
		// Iterate all supported locales
		for (int i = 0; i < num_locales; i++)
		{
			// Create a locale menu item
			struct gui_node * locale_node = gui_create_locale_menu_item(this, &first_node, &previous_node, locales[i], gui_change_locale, NULL, NULL, NULL, NULL, NODE_TYPE_CONTEXT_MENU_LOCALE_SETTER);
		}

		// Free the list of supported locales
		io_free_item_list(num_locales, &locales);
	}

	// Link the first & last nodes together
	if (first_node != NULL && previous_node != NULL)
	{
		// Link the first node to the last node
		first_node->previous = previous_node;

		// Link the last node to the first node
		previous_node->next = first_node;

		// Link this node to its first child node
		this->child = first_node;

		// And keep track of it for moving pointer menus
		this->children = this->child;

		// Initialize the node iterator
		struct gui_node * locale_node = this->child;

		// Iterate all nodes
		do
		{
			// Cast the node's locale data
			struct gui_locale_menu_node_data * locale_node_data = (struct gui_locale_menu_node_data *)locale_node->data;

			// We've found the current locale's node
			if (strcmp(locale_node_data->locale, locale_menu_node_data->locale) == 0)
			{
				// Pre-select this node so that the user always starts at his currently selected locale
				this->child = locale_node;

				// No need to iterate the rest
				break;
			}

			// Move to the next node
			locale_node = locale_node->next;
		} while (locale_node != this->child);
	}

	// Make this node take focus on screen
	this->context->menu.active = this;
}

/**
 * @brief Deactivates the locale context menu.
 */
static void gui_deactivate_locale_menu(struct gui_node * this)
{
	// The menu has been activated (aka. its in focus, this check is necessary because deactivate gets called on parent node disposal as well)
	if (this->context->menu.active == this)
	{
		// Get the locale menu node data
		struct gui_locale_menu_node_data * locale_menu_node_data = (struct gui_locale_menu_node_data *)this->data;

		// Set the current locale
		localization_change_locale(locale_menu_node_data->locale);

		// Write the context settings to disk
		gui_write_configuration(this->context);

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
}

/**
 * @brief Re-renders the locale context menu's surfaces.
 */
static void gui_invalidate_locale_menu(struct gui_node * this)
{
	// Create a temporary node
	struct gui_node * node = gui_create_context_menu_item(this, NULL, NULL, gettext("Change Language"), NULL, NULL, NULL, NULL, NULL, NODE_TYPE_CONTEXT_MENU_LOCALE_MENU, NULL);

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

#endif