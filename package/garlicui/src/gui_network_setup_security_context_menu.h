#ifndef GUI_NETWORK_SETUP_SECURITY_CONTEXT_MENU_H
#define GUI_NETWORK_SETUP_SECURITY_CONTEXT_MENU_H

/**
 * @brief Sets the to-be-applied / saved new network setup security.
 */
static void gui_change_network_setup_security(struct gui_node * this)
{
	// Get the selected network setup security's node data
	struct gui_network_setup_security_menu_node_data * network_setup_security_node_data = (struct gui_network_setup_security_menu_node_data *)this->data;

	// Get the network setup security menu's node data
	struct gui_network_setup_security_menu_node_data * network_setup_security_menu_node_data = (struct gui_network_setup_security_menu_node_data *)this->parent->data;

	// Allocate the network setup security buffer
	network_setup_security_menu_node_data->security = calloc(1, strlen(network_setup_security_node_data->security));

	// Set the network setup security
	strcpy(network_setup_security_menu_node_data->security, network_setup_security_node_data->security);

	// Deactivate the network setup security menu's node (which will set & flush the network setting to disk)
	this->parent->deactivate(this->parent);
}

/**
 * @brief Creates a network setup security context menu item.
 */
static struct gui_node * gui_create_network_setup_security_menu_item(struct gui_node * this, struct gui_node ** first_node, struct gui_node ** previous_node, const char * security)
{
	// The created node
	struct gui_node * network_setup_security_menu_node = NULL;

	// Allocate the network setup security menu node data buffer
	struct gui_network_setup_security_menu_node_data * network_setup_security_menu_node_data = calloc(1, sizeof(struct gui_network_setup_security_menu_node_data));

	// We managed to allocate the network setup security menu node data buffer
	if (network_setup_security_menu_node_data != NULL)
	{
		// Allocate the network setup security buffer
		network_setup_security_menu_node_data->security = calloc(1, strlen(security));

		// We managed to allocate the network setup security buffer
		if (network_setup_security_menu_node_data->security != NULL)
		{
			// Set the network setup security
			strcpy(network_setup_security_menu_node_data->security, security);
		}

		// Create a node for the network setup security menu
		network_setup_security_menu_node = gui_create_context_menu_item(this, first_node, previous_node, security, gui_change_network_setup_security, NULL, NULL, NULL, NULL, NODE_TYPE_CONTEXT_MENU_NETWORK_SETUP_SECURITY_SETTER, network_setup_security_menu_node_data);

		// We failed to create a node for the network setup security menu
		if (network_setup_security_menu_node == NULL)
		{
			// Free the network setup security buffer
			free(network_setup_security_menu_node_data->security);

			// Free the network setup security menu node data buffer
			free(network_setup_security_menu_node_data);
		}
	}

	// Return the created node
	return network_setup_security_menu_node;
}

/**
 * @brief Activates the network setup security context menu.
 */
static void gui_activate_network_setup_security_menu(struct gui_node * this)
{
	// Get the network setup security menu node data
	struct gui_network_setup_security_menu_node_data * network_setup_security_menu_node_data = (struct gui_network_setup_security_menu_node_data *)this->data;

	// Render the title text surface
	gui_activate_menu(this, gettext("Security"));

	// The first node
	struct gui_node * first_node = NULL;

	// The previous node
	struct gui_node * previous_node = NULL;

	// Create a network setup security none menu item
	gui_create_network_setup_security_menu_item(this, &first_node, &previous_node, gettext("None"));

	// Create a network setup security WEP menu item
	gui_create_network_setup_security_menu_item(this, &first_node, &previous_node, gettext("WEP"));

	// Create a network setup security WPA menu item
	gui_create_network_setup_security_menu_item(this, &first_node, &previous_node, gettext("WPA"));

	// Create a network setup security WPA2 menu item
	gui_create_network_setup_security_menu_item(this, &first_node, &previous_node, gettext("WPA2"));

	// Create a network setup security WPA3 menu item
	gui_create_network_setup_security_menu_item(this, &first_node, &previous_node, gettext("WPA3"));

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
		struct gui_node * network_node = this->child;

		// Iterate all nodes
		do
		{
			// Cast the node's network data
			struct gui_network_setup_security_menu_node_data * network_setup_security_node_data = (struct gui_network_setup_security_menu_node_data *)network_node->data;

			// We've found the current network setup security's node
			if (network_setup_security_menu_node_data->security != NULL && strcmp(network_setup_security_node_data->security, network_setup_security_menu_node_data->security) == 0)
			{
				// Pre-select this node so that the user always starts at his currently selected network
				this->child = network_node;

				// No need to iterate the rest
				break;
			}

			// Move to the next node
			network_node = network_node->next;
		} while (network_node != this->child);
	}

	// Make this node take focus on screen
	this->context->menu.active = this;
}

/**
 * @brief Deactivates the network setup security context menu.
 */
static void gui_deactivate_network_setup_security_menu(struct gui_node * this)
{
	// The menu has been activated (aka. its in focus, this check is necessary because deactivate gets called on parent node disposal as well)
	if (this->context->menu.active == this)
	{
		// Get the network setup security menu node data
		struct gui_network_setup_security_menu_node_data * network_setup_security_menu_node_data = (struct gui_network_setup_security_menu_node_data *)this->data;

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
 * @brief Re-renders the network setup security context menu's surfaces.
 */
static void gui_invalidate_network_setup_security_menu(struct gui_node * this)
{
	// Create a temporary node
	struct gui_node * node = gui_create_context_menu_item(this, NULL, NULL, gettext("Security"), NULL, NULL, NULL, NULL, NULL, NODE_TYPE_CONTEXT_MENU_NETWORK_MENU, NULL);

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
 * @brief Creates a network setup security context menu.
 */
static struct gui_node * gui_create_network_setup_security_menu(struct gui_node * this, struct gui_node ** first_node, struct gui_node ** previous_node, const char * display_name, const char * security)
{
	// The created node
	struct gui_node * network_setup_security_menu_node = NULL;

	// Allocate the additional network setup security menu data
	struct gui_network_setup_security_menu_node_data * network_setup_security_menu_node_data = calloc(1, sizeof(struct gui_network_setup_security_menu_node_data));

	// We managed to allocate the additional network setup security menu data
	if (network_setup_security_menu_node_data != NULL)
	{
		// Assign the access point security
		network_setup_security_menu_node_data->security = security;

		// Create a node for the network setup security menu
		network_setup_security_menu_node = gui_create_context_menu_item(this, first_node, previous_node, display_name, gui_activate_network_setup_security_menu, gui_deactivate_network_setup_security_menu, gui_update_basic_menu, gui_render_vertical_scrolling_list_menu, gui_invalidate_network_setup_security_menu, NODE_TYPE_CONTEXT_MENU_NETWORK_SETUP_SECURITY_MENU, network_setup_security_menu_node_data);

		// We failed to create a node for the network setup security menu
		if (network_setup_security_menu_node == NULL)
		{
			// Free the additional menu data
			free(network_setup_security_menu_node_data);
		}
	}

	// Return the created node
	return network_setup_security_menu_node;
}

#endif
