#ifndef GUI_LIBRARY_MENU_H
#define GUI_LIBRARY_MENU_H

#include "gui_folder_menu.h"

/**
 * @brief Activates the library menu.
 */
static void gui_activate_library_menu(struct gui_node * this)
{
	// Render the title text surface
	gui_activate_menu(this, gettext("Library"));

	// The content folders
	char ** content_folders = NULL;

	// Get the content folder paths
	int content_folder_count = io_get_content_items(LIBRARY_FOLDER_PATH, &content_folders, 0, 1, io_get_display_name);

	// We managed to get the content folder paths
	if (content_folders != NULL)
	{
		// The first node
		struct gui_node * first_node = NULL;

		// The previous node
		struct gui_node * previous_node = NULL;

		// Iterate all content folder paths
		for (int i = 0; i < content_folder_count; i++)
		{
			// Read the folder configuration
			xmlDocPtr configuration = io_read_folder_configuration(content_folders[i]);

			// We managed to read the folder configuration
			if (configuration != NULL)
			{
				// Whether files should be listed
				int list_files = 1;

				// Whether directories should be listed
				int list_directories = 0;

				// Get the directory's file type whitelist settings
				io_get_file_type_whitelist(configuration, &list_files, &list_directories);

				// There's at least one game in the folder
				if (io_does_folder_contain_games(content_folders[i], list_files, list_directories, configuration))
				{
					// Get the content folder file name
					const char * file_name = io_get_file_name(content_folders[i]);

					// Allocate memory for the additional menu data
					struct gui_path_menu_node_data * folder_menu_node_data = calloc(1, sizeof(struct gui_path_menu_node_data));

					// We managed to allocate memory for the additional menu data
					if (folder_menu_node_data != NULL)
					{
						// Copy the folder path into the additional menu data structure
						strcpy(folder_menu_node_data->path, content_folders[i]);

						// Copy the folder configuration into the additional menu data structure
						folder_menu_node_data->configuration = configuration;

						// Create the menu node
						struct gui_node * node = gui_create_node(this->context, gui_activate_folder_menu, gui_deactivate_folder_menu, gui_update_basic_menu, gui_render_vertical_scrolling_list_menu, gui_invalidate_folder_menu, NODE_TYPE_LIBRARY_DIRECTORY_MENU, folder_menu_node_data);

						// We managed to create the menu node
						if (node != NULL)
						{
							// The node population status
							int populated = 0;

							// The menu icon
							SDL_Surface * menu_icon = NULL;

							// The smaller version of the menu icon
							SDL_Surface * small_menu_icon = NULL;

							// Allocate memory for the custom library icon path
							char * icon_path = calloc(1, strlen(content_folders[i]) + 1 + strlen(FOLDER_ICON_FILE_NAME) + 1);

							// We managed to allocate memory for the custom library icon path
							if (icon_path != NULL)
							{
								// Puzzle together the custom library icon path
								sprintf(icon_path, "%s/%s", content_folders[i], FOLDER_ICON_FILE_NAME);

								// Attempt to render the custom library icon
								menu_icon = SVG_Load(icon_path, (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor, (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor, &this->context->colors.icon.foreground);

								// Attempt to render the custom library icon at half size
								small_menu_icon = SVG_Load(icon_path, (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor * UNFOCUSED_PAGING_CARD_WIDTH_SCALE_FACTOR, (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor * UNFOCUSED_PAGING_CARD_WIDTH_SCALE_FACTOR, &this->context->colors.icon.foreground);

								// Free the custom library icon path
								free(icon_path);
							}

							// We failed to render the custom library icon
							if (menu_icon == NULL)
							{
								// Render a fallback library icon instead
								menu_icon = icon_load("folder.svg", (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor, (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor, &this->context->colors.icon.foreground);
							}

							// We failed to render the custom half size library icon
							if (small_menu_icon == NULL)
							{
								// Render the fallback library icon at half size
								small_menu_icon = icon_load("folder.svg", (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor * UNFOCUSED_PAGING_CARD_WIDTH_SCALE_FACTOR, (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor * UNFOCUSED_PAGING_CARD_WIDTH_SCALE_FACTOR, &this->context->colors.icon.foreground);
							}

							// We managed to load the library icons
							if (menu_icon != NULL && small_menu_icon != NULL)
							{
								// Pre-render the normal surface
								node->normal_surface = icon_paging_card(small_menu_icon, VERTICAL_ALIGNMENT_CENTER, localization_font_file_path(NULL), localization_font_height(), this->context->colors.legend.foreground, this->context->colors.legend.background, SCREEN_MARGIN * this->context->surfaces.scale_factor, DEFAULT_PAGING_CARD_WIDTH * this->context->surfaces.scale_factor, this->context->surfaces.screen->h * UNFOCUSED_PAGING_CARD_HEIGHT_SCALE_FACTOR, DEFAULT_PAGING_CARD_ROUNDED_CORNER_RADIUS * this->context->surfaces.scale_factor, file_name);

								// We managed to pre-render the normal surface
								if (node->normal_surface != NULL)
								{
									// Pre-render the selected surface
									node->selected_surface = icon_paging_card(menu_icon, VERTICAL_ALIGNMENT_CENTER, localization_font_file_path(NULL), localization_font_height(), this->context->colors.legend.foreground, this->context->colors.legend.background, SCREEN_MARGIN * this->context->surfaces.scale_factor, DEFAULT_PAGING_CARD_WIDTH * this->context->surfaces.scale_factor, this->context->surfaces.screen->h * FOCUSED_PAGING_CARD_HEIGHT_SCALE_FACTOR, DEFAULT_PAGING_CARD_ROUNDED_CORNER_RADIUS * this->context->surfaces.scale_factor, file_name);

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
							}

							// We have a menu icon that needs to be freed
							if (menu_icon != NULL)
							{
								// Free the library icon surface
								SDL_FreeSurface(menu_icon);
							}

							// We have a half size menu icon that needs to be freed
							if (small_menu_icon != NULL)
							{
								// Free the half size library icon surface
								SDL_FreeSurface(small_menu_icon);
							}

							// We failed to populate the node
							if (!populated)
							{
								// Free the folder configuration
								xmlFreeDoc(configuration);

								// Destroy the node
								gui_destroy_node(node, 1);

								// NULL the node reference
								node = NULL;
							}
						}

						// We failed to create the menu node
						else
						{
							// Free the folder configuration
							xmlFreeDoc(configuration);

							// Free the additional menu data
							free(folder_menu_node_data);
						}
					}

					// We failed to allocate memory for the additional menu data
					else
					{
						// Free the folder configuration
						xmlFreeDoc(configuration);
					}
				}

				// The folder doesn't contain games
				else
				{
					// Free the folder configuration
					xmlFreeDoc(configuration);
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

		// Free the content folder paths
		io_free_item_list(content_folder_count, &content_folders);

		// Make this node take focus on screen
		this->context->menu.active = this;
	}
}

/**
 * @brief Deactivates the library menu.
 */
static void gui_deactivate_library_menu(struct gui_node * this)
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
 * @brief Re-renders the library menu node's surfaces.
 */
static void gui_invalidate_library_menu(struct gui_node * this)
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

	// Get the localized Library text
	const char * title = gettext("Library");

	// Render the title text surface
	gui_activate_menu(this, title);

	// Render the normal size library icon
	SDL_Surface * icon = icon_load("library.svg", (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor, (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor, &this->context->colors.icon.foreground);

	// We managed to render the normal size library icon
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

	// Render the small size library icon
	icon = icon_load("library.svg", (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor * UNFOCUSED_PAGING_CARD_WIDTH_SCALE_FACTOR, (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor * UNFOCUSED_PAGING_CARD_WIDTH_SCALE_FACTOR, &this->context->colors.icon.foreground);

	// We managed to render the small size library icon
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