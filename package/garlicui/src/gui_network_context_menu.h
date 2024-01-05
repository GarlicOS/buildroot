#ifndef GUI_NETWORK_CONTEXT_MENU_H
#define GUI_NETWORK_CONTEXT_MENU_H

#include "gui_network_setup_context_menu.h"

/**
 * @brief Creates a network context menu item.
 */
static struct gui_node * gui_create_network_menu_item(struct gui_node * this, struct gui_node ** first_node, struct gui_node ** previous_node, const char * display_name, struct network_access_point * network_ap)
{
	struct gui_node * network_setup_menu_node = NULL;

	// Allocate the additional network setup menu data
	struct gui_network_setup_menu_node_data * network_setup_menu_node_data = calloc(1, sizeof(struct gui_network_setup_menu_node_data));

	// We managed to allocate the additional network setup menu data
	if (network_setup_menu_node_data != NULL)
	{
		// Assign the access point
		network_setup_menu_node_data->network_ap = network_ap;

		// Create a node for the network setup menu
		network_setup_menu_node = gui_create_context_menu_item(this, first_node, previous_node, display_name, gui_activate_network_setup_menu, gui_deactivate_network_setup_menu, gui_update_basic_menu, gui_render_vertical_scrolling_list_menu, gui_invalidate_network_setup_menu, NODE_TYPE_CONTEXT_MENU_NETWORK_SETUP_MENU, network_setup_menu_node_data);

		// We failed to create a node for the network setup menu
		if (network_setup_menu_node == NULL)
		{
			// Free the additional menu data
			free(network_setup_menu_node_data);
		}
	}

	// Return the created node
	return network_setup_menu_node;
}

/**
 * @brief Activates the network context menu.
 */
static void gui_activate_network_menu(struct gui_node * this)
{
	// Get the network menu node data
	struct gui_network_menu_node_data * network_menu_node_data = (struct gui_network_menu_node_data *)this->data;

	// Render the title text surface
	gui_activate_menu(this, gettext("Change Network"));

	// The first node
	struct gui_node * first_node = NULL;

	// The previous node
	struct gui_node * previous_node = NULL;

	// The visible networks
	struct network_access_point ** visible_access_points = NULL;

	// Get the visible networks
	int num_networks = network_get_access_points(&visible_access_points);

	// We managed to get the visible networks
	if (visible_access_points != NULL)
	{
		// Iterate all visible networks
		for (int i = 0; i < num_networks; i++)
		{
			// Create a network menu item
			gui_create_network_menu_item(this, &first_node, &previous_node, visible_access_points[i]->ssid, visible_access_points[i]);
		}

		// Free the list of visible networks
		// TODO network_free_access_points(visible_access_points, num_networks);
	}

	// Iterate all manual networks
	for (int i = 0, n = network_menu_node_data->network_ap_count; i < n; i++)
	{
		// Create a network menu item
		gui_create_network_menu_item(this, &first_node, &previous_node, network_menu_node_data->network_aps[i]->ssid, network_menu_node_data->network_aps[i]);
	}

	// Allocate memory for the access point
    struct network_access_point * access_point = (struct network_access_point *)calloc(1, sizeof(struct network_access_point));

	// We managed to allocate the network access point
	if (access_point != NULL)
	{
		// Create a network menu item
		gui_create_network_menu_item(this, &first_node, &previous_node, gettext("Manual configuration"), access_point);
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
		struct gui_node * network_node = this->child;

		// Iterate all nodes
		do
		{
			// Cast the node's network data
			struct gui_network_setup_menu_node_data * network_setup_node_data = (struct gui_network_setup_menu_node_data *)network_node->data;

			// We've found the current network's node
			if (network_setup_node_data->network_ap->active)
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
 * @brief Deactivates the network context menu.
 */
static void gui_deactivate_network_menu(struct gui_node * this)
{
	// The menu has been activated (aka. its in focus, this check is necessary because deactivate gets called on parent node disposal as well)
	if (this->context->menu.active == this)
	{
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
 * @brief Re-renders the network context menu's surfaces.
 */
static void gui_invalidate_network_menu(struct gui_node * this)
{
	// Create a temporary node
	struct gui_node * node = gui_create_context_menu_item(this, NULL, NULL, gettext("Change Network"), NULL, NULL, NULL, NULL, NULL, NODE_TYPE_CONTEXT_MENU_NETWORK_MENU, NULL);

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
