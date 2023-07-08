#ifndef GUI_FOLDER_MENU_H
#define GUI_FOLDER_MENU_H

#include "gui_content.h"

/**
 * @brief Re-renders a library file node's surfaces.
 */
static void gui_invalidate_library_file(struct gui_node * this)
{
	// Cast the path data
	struct gui_path_menu_node_data * data = (struct gui_path_menu_node_data *)this->data;

	// We have valid path data
	if (data != NULL && data->path != NULL)
	{
		// Get the content item display name
		char * title = io_get_display_name(data->path);

		// We managed to get the content item display name
		if (title != NULL)
		{
			// Render the normal surface
			SDL_Surface * surface = TTF_RenderUTF8_Blended(this->context->fonts.small, title, SDL_ToSDLColor(this->context->colors.base.foreground));

			// We managed to render the normal surface
			if (surface != NULL)
			{
				// We have a possibly no longer valid surface
				if (this->normal_surface != NULL)
				{
					// Free the surface
					SDL_FreeSurface(this->normal_surface);
				}

				// Link in the new surface
				this->normal_surface = surface;
			}

			// Render the selected surface
			surface = icon_circled_text(this->context->fonts.small, this->context->colors.legend.foreground, this->context->colors.legend.background, title, 0, (localization_font_height() + SCREEN_MARGIN) * this->context->surfaces.scale_factor, (SCREEN_MARGIN >> 1) * this->context->surfaces.scale_factor, NULL, 0);

			// We managed to render the selected surface
			if (surface != NULL)
			{
				// We have a possibly no longer valid surface
				if (this->selected_surface != NULL)
				{
					// Free the surface
					SDL_FreeSurface(this->selected_surface);
				}

				// Link in the new surface
				this->selected_surface = surface;
			}

			// Free the content item display name
			free(title);
		}
	}
}

/**
 * @brief Activates a library folder menu.
 */
static void gui_activate_folder_menu(struct gui_node * this)
{
	// Cast the folder menu specific data
	struct gui_path_menu_node_data * data = (struct gui_path_menu_node_data *)this->data;

	// Get the content folder file name
	const char * folder_file_name = io_get_file_name(data->path);

	// Render the title text surface
	gui_activate_menu(this, folder_file_name);

	// The content items
	char ** content_items = NULL;

	// Whether files should be listed
	int list_files = 1;

	// Whether directories should be listed
	int list_directories = 0;

	// Get the directory's file type whitelist settings
	io_get_file_type_whitelist(data->configuration, &list_files, &list_directories);

	// Get the content items
	int content_item_count = io_get_content_items(data->path, &content_items, list_files, list_directories, io_get_display_name);

	// We managed to get the content item paths
	if (content_items != NULL)
	{
		// The first node
		struct gui_node * first_node = NULL;

		// The previous node
		struct gui_node * previous_node = NULL;

		// Iterate all content item paths
		for (int i = 0; i < content_item_count; i++)
		{
			// Ignore unwanted paths
			if (io_path_matches_configuration(content_items[i], data->configuration))
			{
				// Get the content item display name
				char * display_name = io_get_display_name(content_items[i]);

				// We managed to get the content item display name
				if (display_name != NULL)
				{
					// Allocate memory for the additional menu data
					struct gui_path_menu_node_data * item_menu_node_data = calloc(1, sizeof(struct gui_path_menu_node_data));

					// We managed to allocate memory for the additional menu data
					if (item_menu_node_data != NULL)
					{
						// Copy the item path into the additional menu data structure
						strcpy(item_menu_node_data->path, content_items[i]);

						// Add a reference to the folder configuration to the additional menu data structure
						item_menu_node_data->configuration = data->configuration;

						// Create the menu node
						struct gui_node * node = gui_create_node(this->context, gui_activate_content, NULL, NULL, NULL, gui_invalidate_library_file, NODE_TYPE_LIBRARY_FILE, item_menu_node_data);

						// We managed to create the menu node
						if (node != NULL)
						{
							// The node population status
							int populated = 0;

							// Pre-render the normal surface
							node->normal_surface = TTF_RenderUTF8_Blended(this->context->fonts.small, display_name, SDL_ToSDLColor(this->context->colors.base.foreground));

							// We managed to pre-render the normal surface
							if (node->normal_surface != NULL)
							{
								// Pre-render the selected surface
								node->selected_surface = icon_circled_text(this->context->fonts.small, this->context->colors.legend.foreground, this->context->colors.legend.background, display_name, 0, (localization_font_height() + SCREEN_MARGIN) * this->context->surfaces.scale_factor, (SCREEN_MARGIN >> 1) * this->context->surfaces.scale_factor, NULL, 0);

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

						// We failed to create the menu node
						else
						{
							// Free the additional menu data
							free(item_menu_node_data);
						}
					}

					// Free the content item display name
					free(display_name);
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

		// Free the content item paths
		io_free_item_list(content_item_count, &content_items);

		// Make this node take focus on screen
		this->context->menu.active = this;
	}
}

/**
 * @brief Deactivates a library folder menu.
 */
static void gui_deactivate_folder_menu(struct gui_node * this)
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
 * @brief Re-renders a folder menu node's surfaces.
 */
static void gui_invalidate_folder_menu(struct gui_node * this)
{
	// Cast the type specific node data
	struct gui_path_menu_node_data * data = (struct gui_path_menu_node_data *)this->data;

	// We have valid path data
	if (data != NULL && data->path != NULL)
	{
		// We have a no longer guaranteed to be valid menu title surface
		if (data->base.title.surface != NULL)
		{
			// Free the menu title surface
			SDL_FreeSurface(data->base.title.surface);

			// And NULL its reference
			data->base.title.surface = NULL;
		}

		// Get the content folder file name and use it as the title
		const char * title = io_get_file_name(data->path);

		// Render the title text surface
		gui_activate_menu(this, title);

		// Render the normal size folder icon
		SDL_Surface * icon = icon_load("folder.svg", (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor, (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor, &this->context->colors.icon.foreground);

		// We managed to render the normal size folder icon
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

		// Render the small size folder icon
		icon = icon_load("folder.svg", (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor * UNFOCUSED_PAGING_CARD_WIDTH_SCALE_FACTOR, (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor * UNFOCUSED_PAGING_CARD_WIDTH_SCALE_FACTOR, &this->context->colors.icon.foreground);

		// We managed to render the small size folder icon
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
}

#endif