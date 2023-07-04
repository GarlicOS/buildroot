#ifndef GUI_RTC_CONTEXT_MENU_H
#define GUI_RTC_CONTEXT_MENU_H

/**
 * @brief The current horizontal scroll repeat interval of the RTC menu.
 */
static Uint32 horizontal_scroll_repeat_interval = SCROLL_EVENT_INITIAL_REPEAT_INTERVAL;

/**
 * @brief The current vertical scroll repeat interval of the RTC menu.
 */
static Uint32 vertical_scroll_repeat_interval = SCROLL_EVENT_INITIAL_REPEAT_INTERVAL;

/**
 * @brief Creates a RTC page surface.
 */
static SDL_Surface * gui_create_rtc_page_surface(struct gui_node * this, const char * title, const char * text, int focused)
{
	// The rendered page surface
	SDL_Surface * page_surface = NULL;

	// Render the text surface (the current year, month, day, hour, minute, meridian setting)
	SDL_Surface * text_surface = TTF_RenderUTF8_Blended(focused ? this->context->fonts.gigantic : this->context->fonts.large, text, SDL_ToSDLColor(this->context->colors.legend.foreground));

	// We managed to render the text surface
	if (text_surface != NULL)
	{
		// Render the page surface
		page_surface = icon_paging_card(text_surface, VERTICAL_ALIGNMENT_CENTER, localization_font_file_path(NULL), localization_font_height(), this->context->colors.legend.foreground, this->context->colors.legend.background, SCREEN_MARGIN * this->context->surfaces.scale_factor, DEFAULT_PAGING_CARD_WIDTH * this->context->surfaces.scale_factor, this->context->surfaces.screen->h * (focused ? FOCUSED_PAGING_CARD_HEIGHT_SCALE_FACTOR : UNFOCUSED_PAGING_CARD_HEIGHT_SCALE_FACTOR), DEFAULT_PAGING_CARD_ROUNDED_CORNER_RADIUS * this->context->surfaces.scale_factor, title);

		// We managed to render the page surface
		if (page_surface != NULL)
		{
			// We're rendering a hour picker page surface
			if (this->type == NODE_TYPE_CONTEXT_MENU_DATE_AND_TIME_HOUR_SETTER)
			{
				// Get the RTC menu node data
				struct gui_rtc_menu_node_data * rtc_menu_node_data = (struct gui_rtc_menu_node_data *)this->parent->data;

				// The top icon text
				char top_icon_text[128];

				// Default to 24 hour military time
				sprintf(top_icon_text, gettext("24H"));

				// We've chosen a 12 hour (meridian) clock style
				if (rtc_menu_node_data->meridian_time)
				{
					// Override the top icon text with the current hour suffix
					char * hour_suffix = NULL;
					clock_render_text_parts(rtc_menu_node_data->local_time, rtc_menu_node_data->meridian_time, NULL, NULL, NULL, NULL, NULL, &hour_suffix);
					sprintf(top_icon_text, hour_suffix);
				}

				// Render a top icon that shows the current meridian time setting for the hour picker
				SDL_Surface * top_icon_surface = SDL_TrimTransparent(TTF_RenderUTF8_Blended(this->context->fonts.normal, top_icon_text, SDL_ToSDLColor(this->context->colors.legend.foreground)), 1);

				// We managed to render a top icon that shows the current meridian time setting for the hour picker
				if (top_icon_surface != NULL)
				{
					// Render the icon onto the page surface
					SDL_Rect top_icon_position;
					top_icon_position.x = (page_surface->w - top_icon_surface->w) >> 1;
					top_icon_position.y = SCREEN_MARGIN * this->context->surfaces.scale_factor;
					top_icon_position.w = top_icon_surface->w;
					top_icon_position.h = top_icon_surface->h;
					SDL_BlitSurfaceAlpha(top_icon_surface, NULL, page_surface, &top_icon_position);

					// Free the top icon surface
					SDL_FreeSurface(top_icon_surface);
				}
			}
		}

		// Free the text surface
		SDL_FreeSurface(text_surface);
	}

	// Return the rendered page surface
	return page_surface;
}

/**
 * @brief Creates a RTC menu item.
 */
static struct gui_node * gui_create_rtc_menu_item(struct gui_node * this, struct gui_node ** first_node, struct gui_node ** previous_node, const char * title, callback activate, callback deactivate, callback update, callback render, callback invalidate, int type, void * data)
{
	// Get the RTC menu node data
	struct gui_rtc_menu_node_data * rtc_menu_node_data = (struct gui_rtc_menu_node_data *)this->data;

	// Create a node for the item
	struct gui_node * node = gui_create_node(this->context, activate, deactivate, update, render, invalidate, type, data);

	// We managed to create a node for the item
	if (node != NULL)
	{
		// The page's "text" icon
		char * text = NULL;

		// Get the part we're actually interested in
		clock_render_text_parts(rtc_menu_node_data->local_time, rtc_menu_node_data->meridian_time, type == NODE_TYPE_CONTEXT_MENU_DATE_AND_TIME_YEAR_SETTER ? &text : NULL, type == NODE_TYPE_CONTEXT_MENU_DATE_AND_TIME_MONTH_SETTER ? &text : NULL, type == NODE_TYPE_CONTEXT_MENU_DATE_AND_TIME_DAY_SETTER ? &text : NULL, type == NODE_TYPE_CONTEXT_MENU_DATE_AND_TIME_HOUR_SETTER ? &text : NULL, type == NODE_TYPE_CONTEXT_MENU_DATE_AND_TIME_MINUTE_SETTER ? &text : NULL, NULL);

		// Duplicate the text (to prevent follow-up calls to clock_render_text_parts from overwriting the static buffer)
		text = strdup(text);

		// We managed to duplicate the text
		if (text != NULL)
		{
			// The node population status
			int populated = 0;

			// Link the node to its parent
			node->parent = this;

			// Pre-render the normal surface
			node->normal_surface = gui_create_rtc_page_surface(node, title, text, 0);

			// We managed to pre-render the normal surface
			if (node->normal_surface != NULL)
			{
				// Pre-render the selected surface
				node->selected_surface = gui_create_rtc_page_surface(node, title, text, 1);

				// We managed to pre-render the selected surface
				if (node->selected_surface != NULL)
				{
					// The caller wants us to link the sibling nodes together
					if (previous_node != NULL)
					{
						// We have a previous sibling node
						if (*previous_node != NULL)
						{
							// Link the previous node to the current one
							(*previous_node)->next = node;

							// Link the current node to the previous one
							node->previous = *previous_node;
						}

						// Update the previous node
						*previous_node = node;
					}

					// Mark the node as populated
					populated = 1;

					// We haven't set the first node yet
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

			// Free the duplicated text
			free(text);
		}
	}

	// Return the node
	return node;
}

/**
 * @brief Toggles between the 12 and 24 hour clock style.
 */
static void gui_toggle_meridian_time(struct gui_node * this)
{
	// Get the RTC menu node data
	struct gui_rtc_menu_node_data * rtc_menu_node_data = (struct gui_rtc_menu_node_data *)this->parent->data;

	// Toggle between the meridian time settings (24 <-> 12 hour clock display)
	rtc_menu_node_data->meridian_time = !rtc_menu_node_data->meridian_time;

	// Create an replacement hour node
	struct gui_node * replacement_node = gui_create_rtc_menu_item(this->parent, NULL, NULL, gettext("Hour"), gui_toggle_meridian_time, NULL, NULL, NULL, NULL, NODE_TYPE_CONTEXT_MENU_DATE_AND_TIME_HOUR_SETTER, NULL);

	// We managed to create an replacement hour node
	if (replacement_node != NULL)
	{
		// We have a previous normal surface that needs freeing
		if (this->normal_surface != NULL)
		{
			// Free the previous normal surface
			SDL_FreeSurface(this->normal_surface);
		}

		// We have a previous selected surface that needs freeing
		if (this->selected_surface != NULL)
		{
			// Free the previous selected surface
			SDL_FreeSurface(this->selected_surface);
		}

		// Replace the surfaces
		this->normal_surface = replacement_node->normal_surface;
		this->selected_surface = replacement_node->selected_surface;

		// Discard the temporary replacement node
		free(replacement_node);
	}
}

/**
 * @brief Activates the RTC configuration context menu.
 */
static void gui_activate_rtc_menu(struct gui_node * this)
{
	// Get the RTC menu node data
	struct gui_rtc_menu_node_data * rtc_menu_node_data = (struct gui_rtc_menu_node_data *)this->data;

	// Get the current local time
	time(&rtc_menu_node_data->local_time);

	// Get the current meridian time setting
	rtc_menu_node_data->meridian_time = this->context->settings.meridian_time;

	// Render the title text surface
	gui_activate_menu(this, gettext("Change Date & Time"));

	// The first node
	struct gui_node * first_node = NULL;

	// The previous node
	struct gui_node * previous_node = NULL;

	// Create a year node
	struct gui_node * year_node = gui_create_rtc_menu_item(this, &first_node, &previous_node, gettext("Year"), NULL, NULL, NULL, NULL, NULL, NODE_TYPE_CONTEXT_MENU_DATE_AND_TIME_YEAR_SETTER, NULL);

	// Create a month node
	struct gui_node * month_node = gui_create_rtc_menu_item(this, &first_node, &previous_node, gettext("Month"), NULL, NULL, NULL, NULL, NULL, NODE_TYPE_CONTEXT_MENU_DATE_AND_TIME_MONTH_SETTER, NULL);

	// Create a day node
	struct gui_node * day_node = gui_create_rtc_menu_item(this, &first_node, &previous_node, gettext("Day"), NULL, NULL, NULL, NULL, NULL, NODE_TYPE_CONTEXT_MENU_DATE_AND_TIME_DAY_SETTER, NULL);

	// Create an hour node
	struct gui_node * hour_node = gui_create_rtc_menu_item(this, &first_node, &previous_node, gettext("Hour"), gui_toggle_meridian_time, NULL, NULL, NULL, NULL, NODE_TYPE_CONTEXT_MENU_DATE_AND_TIME_HOUR_SETTER, NULL);

	// Create a minute node
	struct gui_node * minute_node = gui_create_rtc_menu_item(this, &first_node, &previous_node, gettext("Minute"), NULL, NULL, NULL, NULL, NULL, NODE_TYPE_CONTEXT_MENU_DATE_AND_TIME_MINUTE_SETTER, NULL);

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
	}

	// Make this node take focus on screen
	this->context->menu.active = this;
}

/**
 * @brief Deactivates the RTC configuration context menu.
 */
static void gui_deactivate_rtc_menu(struct gui_node * this)
{
	// The menu has been activated (aka. its in focus, this check is necessary because deactivate gets called on parent node disposal as well)
	if (this->context->menu.active == this)
	{
		// Get the RTC menu node data
		struct gui_rtc_menu_node_data * rtc_menu_node_data = (struct gui_rtc_menu_node_data *)this->data;

		// Set the current local time (which will also set the RTC)
		clock_set_current_time(rtc_menu_node_data->local_time);

		// Activate the new meridian time setting
		this->context->settings.meridian_time = rtc_menu_node_data->meridian_time;

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
 * @brief Changes the currently selected RTC configuration menu node's value.
 */
static void gui_change_rtc(struct gui_node * this, int direction)
{
	// Get the RTC menu node data
	struct gui_rtc_menu_node_data * rtc_menu_node_data = (struct gui_rtc_menu_node_data *)this->data;

	// Convert the current time into a tm structure
	struct tm * calendar_time = localtime(&rtc_menu_node_data->local_time);

	// The replacement node
	struct gui_node * replacement_node = NULL;

	// Handle the different time components
	switch (this->child->type)
	{
		// The user changed the year setting
		case NODE_TYPE_CONTEXT_MENU_DATE_AND_TIME_YEAR_SETTER:
		{
			// Apply the delta
			calendar_time->tm_year = calendar_time->tm_year + direction;

			// We've fallen below the minimum value
			if (calendar_time->tm_year < 0)
			{
				// Roll-over into the valid range
				calendar_time->tm_year = 0;
			}

			// The number of days in this month
			int max_days = clock_get_days_in_month(calendar_time);

			// We're overshooting the days in a month
			if (calendar_time->tm_mday > max_days)
			{
				// Fix the day value of the calendar time
				calendar_time->tm_mday = max_days;
			}

			// Update the timestamp
			rtc_menu_node_data->local_time = mktime(calendar_time);

			// Create a replacement node
			replacement_node = gui_create_rtc_menu_item(this, NULL, NULL, gettext("Year"), NULL, NULL, NULL, NULL, NULL, NODE_TYPE_CONTEXT_MENU_DATE_AND_TIME_YEAR_SETTER, NULL);

			// Prevent fall-through
			break;
		}

		// The user changed the month setting
		case NODE_TYPE_CONTEXT_MENU_DATE_AND_TIME_MONTH_SETTER:
		{
			// Apply the delta
			calendar_time->tm_mon = calendar_time->tm_mon + direction;

			// We've fallen below the minimum value
			while (calendar_time->tm_mon < 0)
			{
				// Roll-over into the valid range
				calendar_time->tm_mon += 12;
			}

			// We've risen above the maximum value
			while (calendar_time->tm_mon > 11)
			{
				// Roll-over into the valid range
				calendar_time->tm_mon -= 12;
			}

			// The number of days in this month
			int max_days = clock_get_days_in_month(calendar_time);

			// We're overshooting the days in a month
			if (calendar_time->tm_mday > max_days)
			{
				// Fix the day value of the calendar time
				calendar_time->tm_mday = max_days;
			}

			// Update the timestamp
			rtc_menu_node_data->local_time = mktime(calendar_time);

			// Create a replacement node
			replacement_node = gui_create_rtc_menu_item(this, NULL, NULL, gettext("Month"), NULL, NULL, NULL, NULL, NULL, NODE_TYPE_CONTEXT_MENU_DATE_AND_TIME_MONTH_SETTER, NULL);

			// Prevent fall-through
			break;
		}

		// The user changed the day setting
		case NODE_TYPE_CONTEXT_MENU_DATE_AND_TIME_DAY_SETTER:
		{
			// Apply the delta
			calendar_time->tm_mday = calendar_time->tm_mday + direction;

			// The number of days in this month
			int max_days = clock_get_days_in_month(calendar_time);

			// We've fallen below the minimum value
			while (calendar_time->tm_mday < 1)
			{
				// Roll-over into the valid range
				calendar_time->tm_mday += max_days;
			}

			// We've risen above the maximum value
			while (calendar_time->tm_mday > max_days)
			{
				// Roll-over into the valid range
				calendar_time->tm_mday -= max_days;
			}

			// Update the timestamp
			rtc_menu_node_data->local_time = mktime(calendar_time);

			// Create a replacement node
			replacement_node = gui_create_rtc_menu_item(this, NULL, NULL, gettext("Day"), NULL, NULL, NULL, NULL, NULL, NODE_TYPE_CONTEXT_MENU_DATE_AND_TIME_DAY_SETTER, NULL);

			// Prevent fall-through
			break;
		}

		// The user changed the hour setting
		case NODE_TYPE_CONTEXT_MENU_DATE_AND_TIME_HOUR_SETTER:
		{
			// Apply the delta
			calendar_time->tm_hour = calendar_time->tm_hour + direction;

			// We've fallen below the minimum value
			while (calendar_time->tm_hour < 0)
			{
				// Roll-over into the valid range
				calendar_time->tm_hour += 24;
			}

			// We've risen above the maximum value
			while (calendar_time->tm_hour > 23)
			{
				// Roll-over into the valid range
				calendar_time->tm_hour -= 24;
			}

			// Update the timestamp
			rtc_menu_node_data->local_time = mktime(calendar_time);

			// Create a replacement node
			replacement_node = gui_create_rtc_menu_item(this, NULL, NULL, gettext("Hour"), gui_toggle_meridian_time, NULL, NULL, NULL, NULL, NODE_TYPE_CONTEXT_MENU_DATE_AND_TIME_HOUR_SETTER, NULL);

			// Prevent fall-through
			break;
		}

		// The user changed the minute setting
		case NODE_TYPE_CONTEXT_MENU_DATE_AND_TIME_MINUTE_SETTER:
		{
			// Apply the delta
			calendar_time->tm_min = calendar_time->tm_min + direction;

			// We've fallen below the minimum value
			while (calendar_time->tm_min < 0)
			{
				// Roll-over into the valid range
				calendar_time->tm_min += 60;
			}

			// We've risen above the maximum value
			while (calendar_time->tm_min > 59)
			{
				// Roll-over into the valid range
				calendar_time->tm_min -= 60;
			}

			// Update the timestamp
			rtc_menu_node_data->local_time = mktime(calendar_time);

			// Create a replacement node
			replacement_node = gui_create_rtc_menu_item(this, NULL, NULL, gettext("Minute"), NULL, NULL, NULL, NULL, NULL, NODE_TYPE_CONTEXT_MENU_DATE_AND_TIME_MINUTE_SETTER, NULL);

			// Prevent fall-through
			break;
		}
	}

	// We managed to create a replacement node
	if (replacement_node != NULL)
	{
		// We have a previous normal surface that needs freeing
		if (this->child->normal_surface != NULL)
		{
			// Free the previous normal surface
			SDL_FreeSurface(this->child->normal_surface);
		}

		// We have a previous selected surface that needs freeing
		if (this->child->selected_surface != NULL)
		{
			// Free the previous selected surface
			SDL_FreeSurface(this->child->selected_surface);
		}

		// Replace the surfaces
		this->child->normal_surface = replacement_node->normal_surface;
		this->child->selected_surface = replacement_node->selected_surface;

		// Discard the temporary replacement node
		free(replacement_node);

		// We've changed the month or year
		if (this->child->type == NODE_TYPE_CONTEXT_MENU_DATE_AND_TIME_YEAR_SETTER || this->child->type == NODE_TYPE_CONTEXT_MENU_DATE_AND_TIME_MONTH_SETTER)
		{
			// Create a replacement node
			replacement_node = gui_create_rtc_menu_item(this, NULL, NULL, gettext("Day"), NULL, NULL, NULL, NULL, NULL, NODE_TYPE_CONTEXT_MENU_DATE_AND_TIME_DAY_SETTER, NULL);

			// We managed to create a replacement node
			if (replacement_node != NULL)
			{
				// The day setting node
				struct gui_node * day_node = this->child;

				// Find the day setting node
				while (day_node->type != NODE_TYPE_CONTEXT_MENU_DATE_AND_TIME_DAY_SETTER)
				{
					// Move to the next setting node
					day_node = day_node->next;
				}

				// We have a previous normal surface that needs freeing
				if (day_node->normal_surface != NULL)
				{
					// Free the previous normal surface
					SDL_FreeSurface(day_node->normal_surface);
				}

				// We have a previous selected surface that needs freeing
				if (day_node->selected_surface != NULL)
				{
					// Free the previous selected surface
					SDL_FreeSurface(day_node->selected_surface);
				}

				// Replace the surfaces
				day_node->normal_surface = replacement_node->normal_surface;
				day_node->selected_surface = replacement_node->selected_surface;

				// Discard the temporary replacement node
				free(replacement_node);
			}
		}
	}
}

/**
 * @brief Updates the RTC context menu.
 */
static void gui_update_rtc_menu(struct gui_node * this)
{
	// The current horizontal direction
	static int horizontal_direction = 0;

	// The time of the last horizontal direction change
	static Uint32 last_horizontal_direction_change = 0;

	// The time of the last horizontal scroll event
	static Uint32 last_horizontal_scroll_event = 0;

	// The current vertical direction
	static int vertical_direction = 0;

	// The time of the last vertical direction change
	static Uint32 last_vertical_direction_change = 0;

	// The time of the last vertical scroll event
	static Uint32 last_vertical_scroll_event = 0;

	// The menu has at least one child node
	if (this->child != NULL)
	{
		// Use the horizontal directional pad axis by default
		int new_horizontal_direction = this->context->inputs.internal.current.dpad_x;

		// Use the horizontal directional pad axis by default
		int new_vertical_direction = this->context->inputs.internal.current.dpad_y;

		// The horizontal direction has changed
		if (horizontal_direction != new_horizontal_direction)
		{
			// Override the current horizontal direction
			horizontal_direction = new_horizontal_direction;

			// Keep track of when the direction changed
			last_horizontal_direction_change = SDL_GetTicks();

			// Reset the repeat event timer
			last_horizontal_scroll_event = 0;

			// Reset the scroll repeat interval
			horizontal_scroll_repeat_interval = SCROLL_EVENT_INITIAL_REPEAT_INTERVAL;
		}

		// We're still scrolling in the same horizontal direction and have room for acceleration
		else if (horizontal_direction != 0 && horizontal_scroll_repeat_interval > SCROLL_EVENT_MINIMUM_REPEAT_INTERVAL)
		{
			// Calculate the acceleration multiplier
			Uint32 acceleration_multiplier = (SDL_GetTicks() - last_horizontal_direction_change) / SCROLL_EVENT_ACCELERATION_TIMESPAN;

			// Calculate the new scroll repeat interval
			int new_scroll_repeat_interval = SCROLL_EVENT_INITIAL_REPEAT_INTERVAL - (SCROLL_EVENT_ACCELERATION * acceleration_multiplier);

			// Set the new scroll repeat interval
			horizontal_scroll_repeat_interval = new_scroll_repeat_interval > SCROLL_EVENT_MINIMUM_REPEAT_INTERVAL ? new_scroll_repeat_interval : SCROLL_EVENT_MINIMUM_REPEAT_INTERVAL;
		}

		// We fulfill all horizontal scroll requirements
		if (horizontal_direction != 0 && (last_horizontal_scroll_event == 0 || (SDL_GetTicks() - last_horizontal_scroll_event) > horizontal_scroll_repeat_interval))
		{
			// We're scrolling left
			if (horizontal_direction < 0)
			{
				// Move to the previous child
				this->child = this->child->previous;
			}

			// We're scrolling right
			else if (horizontal_direction > 0)
			{
				// Move to the next child
				this->child = this->child->next;
			}

			// Set the scroll event timestamp
			last_horizontal_scroll_event = SDL_GetTicks();
		}

		// The vertical direction has changed
		if (vertical_direction != new_vertical_direction)
		{
			// Override the current vertical direction
			vertical_direction = new_vertical_direction;

			// Keep track of when the direction changed
			last_vertical_direction_change = SDL_GetTicks();

			// Reset the repeat event timer
			last_vertical_scroll_event = 0;

			// Reset the scroll repeat interval
			vertical_scroll_repeat_interval = SCROLL_EVENT_INITIAL_REPEAT_INTERVAL;
		}

		// We're still scrolling in the same vertical direction and have room for acceleration
		else if (vertical_direction != 0 && vertical_scroll_repeat_interval > SCROLL_EVENT_MINIMUM_REPEAT_INTERVAL)
		{
			// Calculate the acceleration multiplier
			Uint32 acceleration_multiplier = (SDL_GetTicks() - last_vertical_direction_change) / SCROLL_EVENT_ACCELERATION_TIMESPAN;

			// Calculate the new scroll repeat interval
			int new_scroll_repeat_interval = SCROLL_EVENT_INITIAL_REPEAT_INTERVAL - (SCROLL_EVENT_ACCELERATION * acceleration_multiplier);

			// Set the new scroll repeat interval
			vertical_scroll_repeat_interval = new_scroll_repeat_interval > SCROLL_EVENT_MINIMUM_REPEAT_INTERVAL ? new_scroll_repeat_interval : SCROLL_EVENT_MINIMUM_REPEAT_INTERVAL;
		}

		// We fulfill all vertical scroll requirements
		if (vertical_direction != 0 && (last_vertical_scroll_event == 0 || (SDL_GetTicks() - last_vertical_scroll_event) > vertical_scroll_repeat_interval))
		{
			// Change the RTC settings and re-render the pages as needed
			gui_change_rtc(this, vertical_direction * (-1));

			// Set the scroll event timestamp
			last_vertical_scroll_event = SDL_GetTicks();
		}

		// The east-facing face button was pressed
		if (this->context->inputs.internal.previous.east != this->context->inputs.internal.current.east && this->context->inputs.internal.current.east)
		{
			// The child has an activate callback
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
		// Deactivate the current node
		this->deactivate(this);
	}
}

/**
 * @brief Re-renders the RTC context menu's surfaces.
 */
static void gui_invalidate_rtc_menu(struct gui_node * this)
{
	// Create a temporary node
	struct gui_node * node = gui_create_context_menu_item(this, NULL, NULL, gettext("Change Date & Time"), NULL, NULL, NULL, NULL, NULL, NODE_TYPE_CONTEXT_MENU_LOCALE_MENU, NULL);
	
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