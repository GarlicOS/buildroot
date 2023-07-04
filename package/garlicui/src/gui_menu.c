#include "gui_menu.h"

// The current scroll repeat interval
static Uint32 scroll_repeat_interval = SCROLL_EVENT_INITIAL_REPEAT_INTERVAL;

/**
 * @brief Activates a basic menu, pre-rendering its title text surface.
 */
void gui_activate_menu(struct gui_node * this, const char * title)
{
	// Cast the type specific node data
	struct gui_menu_node_data * data = (struct gui_menu_node_data *)this->data;

	// We have a valid menu node data structure
	if (data != NULL)
	{
		// The menu title surface hasn't been rendered yet
		if (data->title.surface == NULL)
		{
			// Render the menu title surface
			data->title.surface = TTF_RenderUTF8_Blended(this->context->fonts.normal, title, SDL_ToSDLColor(this->context->colors.base.foreground));

			// We managed to render the menu title surface
			if (data->title.surface != NULL)
			{
				// Calculate the menu title surface position
				data->title.position.x = this->context->surfaces.bars.top.logo.position.x + this->context->surfaces.bars.top.logo.position.w + (SCREEN_MARGIN * this->context->surfaces.scale_factor);
				data->title.position.y = ((DEFAULT_TOP_BAR_HEIGHT * this->context->surfaces.scale_factor) - data->title.surface->h) / 2.0f;
				data->title.position.w = data->title.surface->w;
				data->title.position.h = data->title.surface->h;
			}
		}
	}
}

/**
 * @brief Deactivates a basic menu, freeing its title text surface.
 */
void gui_deactivate_menu(struct gui_node * this)
{
	// Cast the type specific node data
	struct gui_menu_node_data * data = (struct gui_menu_node_data *)this->data;

	// We have a menu title surface that needs to be freed
	if (data->title.surface != NULL)
	{
		// Free the menu title surface
		SDL_FreeSurface(data->title.surface);

		// And NULL the reference so it can be re-created on re-activation
		data->title.surface = NULL;
	}
}

/**
 * @brief Handles basic menu related input events.
 *
 * This basic handler is good enough for horizontal / vertical navigation as well as activating and de-activating nodes.
 */
void gui_update_basic_menu(struct gui_node * this)
{
	// The current direction
	static int direction = 0;

	// The time of the last direction change
	static Uint32 last_direction_change = 0;

	// The time of the last scroll event
	static Uint32 last_scroll_event = 0;

	// The menu has at least one child node
	if (this->child != NULL)
	{
		// Use the horizontal directional pad axis by default
		int new_direction = this->context->inputs.internal.current.dpad_x;

		// The horizontal directional pad axis is in its center-state
		if (new_direction == 0)
		{
			// Try using the vertical directional pad axis as a fallback
			new_direction = this->context->inputs.internal.current.dpad_y;
		}

		// The direction has changed
		if (direction != new_direction)
		{
			// Override the current direction
			direction = new_direction;

			// Keep track of when the direction changed
			last_direction_change = SDL_GetTicks();

			// Reset the repeat event timer
			last_scroll_event = 0;

			// Reset the scroll repeat interval
			scroll_repeat_interval = SCROLL_EVENT_INITIAL_REPEAT_INTERVAL;
		}

		// We're still scrolling in the same direction and have room for acceleration
		else if (direction != 0 && scroll_repeat_interval > SCROLL_EVENT_MINIMUM_REPEAT_INTERVAL)
		{
			// Calculate the acceleration multiplier
			Uint32 acceleration_multiplier = (SDL_GetTicks() - last_direction_change) / SCROLL_EVENT_ACCELERATION_TIMESPAN;

			// Calculate the new scroll repeat interval
			int new_scroll_repeat_interval = SCROLL_EVENT_INITIAL_REPEAT_INTERVAL - (SCROLL_EVENT_ACCELERATION * acceleration_multiplier);

			// Set the new scroll repeat interval
			scroll_repeat_interval = new_scroll_repeat_interval > SCROLL_EVENT_MINIMUM_REPEAT_INTERVAL ? new_scroll_repeat_interval : SCROLL_EVENT_MINIMUM_REPEAT_INTERVAL;
		}

		// We fulfill all scroll requirements
		if (direction != 0 && (last_scroll_event == 0 || (SDL_GetTicks() - last_scroll_event) > scroll_repeat_interval))
		{
			// We're scrolling backwards
			if (direction < 0)
			{
				// Move to the previous child
				this->child = this->child->previous;
			}

			// We're scrolling forwards
			else if (direction > 0)
			{
				// Move to the next child
				this->child = this->child->next;
			}

			// Set the scroll event timestamp
			last_scroll_event = SDL_GetTicks();
		}

		// The east-facing face button was pressed
		if (this->context->inputs.internal.previous.east != this->context->inputs.internal.current.east && this->context->inputs.internal.current.east)
		{
			// And the node has an activate callback set
			if (this->child->activate != NULL)
			{
				// Activate the selected child node
				this->child->activate(this->child);
			}
		}
	}

	// The south-facing face button was pressed
	if (this->context->inputs.internal.previous.south != this->context->inputs.internal.current.south && this->context->inputs.internal.current.south)
	{
		// And the current node has a deactivate callback set
		if (this->deactivate != NULL)
		{
			// Deactivate the current node
			this->deactivate(this);
		}
	}
}

/**
 * @brief Renders the given node's children as a paging icon menu.
 *
 * A paging icon menu is a horizontal endlessly scrolling list of item.
 */
void gui_render_paging_icon_menu(struct gui_node * this)
{
	// Cast the type specific node data
	struct gui_menu_node_data * data = (struct gui_menu_node_data *)this->data;

	// We have a valid menu node data structure
	if (data != NULL)
	{
		// Blit the menu title surface
		SDL_BlitSurface(data->title.surface, NULL, this->context->surfaces.screen, &data->title.position);
	}

	// We start at the currently selected node
	struct gui_node * node = this->child;

	// This node contains at least one child node
	if (node != NULL)
	{
		// Render the currently selected item dead center
		SDL_Rect selected_item_position;
		selected_item_position.x = (this->context->surfaces.screen->w - node->selected_surface->w) >> 1;
		selected_item_position.y = (DEFAULT_TOP_BAR_HEIGHT * this->context->surfaces.scale_factor) + ((int)(this->context->surfaces.screen->h - (DEFAULT_TOP_BAR_HEIGHT * this->context->surfaces.scale_factor) - (DEFAULT_BOTTOM_BAR_HEIGHT * this->context->surfaces.scale_factor) - node->selected_surface->h) >> 1);
		selected_item_position.w = node->selected_surface->w;
		selected_item_position.h = node->selected_surface->h;
		SDL_BlitSurface(node->selected_surface, NULL, this->context->surfaces.screen, &selected_item_position);

		// Start with the selected item position
		SDL_Rect unselected_item_position = selected_item_position;

		// We have previous items
		if (node->previous != node)
		{
			// Iterate left
			while (1)
			{
				// Move to the previous node
				node = node->previous;

				// Calculate the node position
				unselected_item_position.x = unselected_item_position.x - (SCREEN_MARGIN * this->context->surfaces.scale_factor) - node->normal_surface->w;
				unselected_item_position.y = (DEFAULT_TOP_BAR_HEIGHT * this->context->surfaces.scale_factor) + ((int)(this->context->surfaces.screen->h - (DEFAULT_TOP_BAR_HEIGHT * this->context->surfaces.scale_factor) - (DEFAULT_BOTTOM_BAR_HEIGHT * this->context->surfaces.scale_factor) - node->normal_surface->h) >> 1);
				unselected_item_position.w = node->normal_surface->w;
				unselected_item_position.h = node->normal_surface->h;

				// Render the node
				SDL_BlitSurface(node->normal_surface, NULL, this->context->surfaces.screen, &unselected_item_position);

				// We're rendering off-screen
				if (unselected_item_position.x <= 0)
				{
					// Which means we have no more space for additional cards
					break;
				}
			}
		}

		// Reset the starting node
		node = this->child;

		// Start with the selected item position
		unselected_item_position = selected_item_position;

		// We have next items
		if (node->next != node)
		{
			// Iterate right
			while (1)
			{
				// Move to the next node
				node = node->next;

				// Calculate the node position
				unselected_item_position.x = unselected_item_position.x + unselected_item_position.w + (SCREEN_MARGIN * this->context->surfaces.scale_factor);
				unselected_item_position.y = (DEFAULT_TOP_BAR_HEIGHT * this->context->surfaces.scale_factor) + ((int)(this->context->surfaces.screen->h - (DEFAULT_TOP_BAR_HEIGHT * this->context->surfaces.scale_factor) - (DEFAULT_BOTTOM_BAR_HEIGHT * this->context->surfaces.scale_factor) - node->normal_surface->h) >> 1);
				unselected_item_position.w = node->normal_surface->w;
				unselected_item_position.h = node->normal_surface->h;

				// Render the node
				SDL_BlitSurface(node->normal_surface, NULL, this->context->surfaces.screen, &unselected_item_position);

				// We're rendering off-screen
				if ((unselected_item_position.x + unselected_item_position.w) >= this->context->surfaces.screen->w)
				{
					// Which means we have no more space for additional cards
					break;
				}
			}
		}
	}
}

/**
 * @brief Calculate the given node's height if it was rendered as a same-height-padded vertical scroll list menu.
 */
static int gui_calculate_vertical_scrolling_list_height(struct gui_node * this)
{
	// The vertical scrolling list height
	int height = 0;

	// We start at the first node
	struct gui_node * node = this->children;

	// We have at least one child node
	if (node != NULL)
	{
		// Iterate all nodes
		do
		{
			// Add the selected display surface height
			height += node->selected_surface->h;

			// This node needs padding
			if (node->next != NULL && node != this->children)
			{
				// Add the padding
				height += SCREEN_MARGIN * this->context->surfaces.scale_factor;
			}

			// Move to the next item
			node = node->next;
		} while (node != NULL && node != this->children);
	}

	// Return the vertical scrolling list height
	return height;
}

/**
 * @brief Renders the given node's children as a vertical scroll list menu.
 */
void gui_render_vertical_scrolling_list_menu(struct gui_node * this)
{
	// Cast the type specific node data
	struct gui_menu_node_data * data = (struct gui_menu_node_data *)this->data;

	// We have a valid menu node data structure
	if (data != NULL)
	{
		// Blit the menu title surface
		SDL_BlitSurface(data->title.surface, NULL, this->context->surfaces.screen, &data->title.position);
	}

	// Calculate the vertical scrolling list height
	int vertical_scrolling_list_height = gui_calculate_vertical_scrolling_list_height(this);

	// The vertical scrolling list fits on screen
	if (vertical_scrolling_list_height <= (this->context->surfaces.screen->h - (DEFAULT_TOP_BAR_HEIGHT * this->context->surfaces.scale_factor) - (DEFAULT_BOTTOM_BAR_HEIGHT * this->context->surfaces.scale_factor)))
	{
		// We start at the first node
		struct gui_node * node = this->children;

		// This node contains at least one child node
		if (node != NULL)
		{
			// The node position rectangle
			SDL_Rect node_position;

			// Set a vertical starting position that will result in the nodes being centered on screen
			node_position.y = (this->context->surfaces.screen->h - vertical_scrolling_list_height) >> 1;

			// Loop all nodes
			do
			{
				// Get the node's display surface
				SDL_Surface * node_surface = node == this->child ? node->selected_surface : node->normal_surface;

				// Backup the previous node's Y position
				int previous_y = node_position.y;

				// Update the item position rectangle
				node_position.x = (this->context->surfaces.screen->w - node_surface->w) >> 1;
				node_position.w = node_surface->w;
				node_position.h = node_surface->h;

				// Unselected nodes need their Y position shifted
				if (node != this->child)
				{
					// Center the normal surface
					node_position.y += (node->selected_surface->h - node->normal_surface->h) >> 1;
				}

				// Render the node
				SDL_BlitSurface(node_surface, NULL, this->context->surfaces.screen, &node_position);

				// Shift the Y position for the next node
				node_position.y = previous_y + node->selected_surface->h + (SCREEN_MARGIN * this->context->surfaces.scale_factor);

				// Move to the next item
				node = node->next;
			} while (node != this->children);
		}
	}

	// The vertical scrolling list doesn't fit on screen
	else
	{
		// We start at the currently selected node
		struct gui_node * node = this->child;

		// This node contains at least one child node
		if (node != NULL)
		{
			// Render the currently selected item dead center
			SDL_Rect selected_item_position;
			selected_item_position.x = (this->context->surfaces.screen->w - node->selected_surface->w) >> 1;
			selected_item_position.y = (DEFAULT_TOP_BAR_HEIGHT * this->context->surfaces.scale_factor) + ((int)(this->context->surfaces.screen->h - (DEFAULT_TOP_BAR_HEIGHT * this->context->surfaces.scale_factor) - (DEFAULT_BOTTOM_BAR_HEIGHT * this->context->surfaces.scale_factor) - node->selected_surface->h) >> 1);
			selected_item_position.w = node->selected_surface->w;
			selected_item_position.h = node->selected_surface->h;
			SDL_BlitSurface(node->selected_surface, NULL, this->context->surfaces.screen, &selected_item_position);

			// Start with the selected item position
			SDL_Rect unselected_item_position = selected_item_position;

			// Iterate up
			while (1)
			{
				// Move to the previous node
				node = node->previous;

				// Calculate the node position
				unselected_item_position.x = (this->context->surfaces.screen->w - node->normal_surface->w) >> 1;
				unselected_item_position.y = unselected_item_position.y - (SCREEN_MARGIN * this->context->surfaces.scale_factor) - node->normal_surface->h;
				unselected_item_position.w = node->normal_surface->w;
				unselected_item_position.h = node->normal_surface->h;

				// We're cutting into the top bar
				if (unselected_item_position.y <= (DEFAULT_TOP_BAR_HEIGHT * this->context->surfaces.scale_factor))
				{
					// Which means we have no more space for additional entries
					break;
				}

				// Render the node
				SDL_BlitSurface(node->normal_surface, NULL, this->context->surfaces.screen, &unselected_item_position);
			}

			// Reset the starting node
			node = this->child;

			// Start with the selected item position
			unselected_item_position = selected_item_position;

			// Iterate down
			while (1)
			{
				// Move to the next node
				node = node->next;

				// Calculate the node position
				unselected_item_position.x = (this->context->surfaces.screen->w - node->normal_surface->w) >> 1;
				unselected_item_position.y = unselected_item_position.y + unselected_item_position.h + (SCREEN_MARGIN * this->context->surfaces.scale_factor);
				unselected_item_position.w = node->normal_surface->w;
				unselected_item_position.h = node->normal_surface->h;

				// We're cutting into the bottom bar
				if ((unselected_item_position.y + unselected_item_position.h) >= (this->context->surfaces.screen->h - (DEFAULT_BOTTOM_BAR_HEIGHT * this->context->surfaces.scale_factor)))
				{
					// Which means we have no more space for additional entries
					break;
				}

				// Render the node
				SDL_BlitSurface(node->normal_surface, NULL, this->context->surfaces.screen, &unselected_item_position);
			}

			// We are scrolling at maximum speed
			if (scroll_repeat_interval == SCROLL_EVENT_MINIMUM_REPEAT_INTERVAL)
			{
				// Cast the item's additional data
				struct gui_path_menu_node_data * item_menu_node_data = (struct gui_path_menu_node_data *)this->child->data;

				// We're dealing with a library file
				if (this->child->type == NODE_TYPE_LIBRARY_FILE && item_menu_node_data != NULL)
				{
					// Render the notch
					SDL_BlitSurface(this->context->surfaces.overlays.notch.surface, NULL, this->context->surfaces.screen, &this->context->surfaces.overlays.notch.position);

					// Render the current letter
					static char letter[2];
					letter[0] = 0;
					char * display_name = io_get_display_name(item_menu_node_data->path);
					if (display_name != NULL)
					{
						letter[0] = display_name[0];
						free(display_name);
					}
					SDL_Color sdl_legend_foreground_color = SDL_ToSDLColor(this->context->colors.notch.foreground);
					SDL_Surface * letter_surface = TTF_RenderUTF8_Blended(this->context->fonts.large, letter, sdl_legend_foreground_color);
					if (letter_surface != NULL)
					{
						SDL_Rect letter_surface_position;
						letter_surface_position.x = this->context->surfaces.overlays.notch.position.x + ((this->context->surfaces.overlays.notch.surface->w - letter_surface->w) >> 1);
						letter_surface_position.y = this->context->surfaces.overlays.notch.position.y + ((this->context->surfaces.overlays.notch.surface->h - letter_surface->h) >> 1);
						letter_surface_position.w = letter_surface->w;
						letter_surface_position.h = letter_surface->h;
						SDL_BlitSurface(letter_surface, NULL, this->context->surfaces.screen, &letter_surface_position);
						SDL_FreeSurface(letter_surface);
					}
				}
			}
		}
	}
}