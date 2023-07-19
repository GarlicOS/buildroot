#ifndef GUI_RETROARCH_MENU_H
#define GUI_RETROARCH_MENU_H

/**
 * @brief Opens the RetroArch menu.
 */
static void gui_activate_retroarch_menu(struct gui_node * this)
{
	// Write the context configuration to disk (so we can restore the UI state on RetroArch exit)
	gui_write_configuration(this->context);

	// Run retroarch without parameters (which opens the RetroArch menu)
	retroarch_run(this->context, NULL, NULL);
}

/**
 * @brief Re-renders the RetroArch menu node's surfaces.
 */
static void gui_invalidate_retroarch(struct gui_node * this)
{
	// Get the localized RetroArch text
	const char * title = gettext("RetroArch");

	// Render the normal size RetroArch icon
	SDL_Surface * icon = icon_load("retroarch.svg", (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor, (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor, &this->context->colors.icon.foreground);

	// We managed to render the normal size RetroArch icon
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

	// Render the small size RetroArch icon
	icon = icon_load("retroarch.svg", (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor * UNFOCUSED_PAGING_CARD_WIDTH_SCALE_FACTOR, (DEFAULT_PAGING_CARD_WIDTH - SCREEN_MARGIN) * this->context->surfaces.scale_factor * UNFOCUSED_PAGING_CARD_WIDTH_SCALE_FACTOR, &this->context->colors.icon.foreground);

	// We managed to render the small size RetroArch icon
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