#ifndef GUI_NETWORK_SETUP_CONTEXT_MENU_H
#define GUI_NETWORK_SETUP_CONTEXT_MENU_H

#include "osk_context_menu.h"
#include "gui_network_setup_security_context_menu.h"

/**
 * @brief Change the network setup connection
 */
static void gui_change_network_setup_connection(struct gui_node * this)
{
	// Get the selected network setup's node data
	struct gui_network_setup_menu_node_data * network_setup_node_data = (struct gui_network_setup_menu_node_data *)this->data;

	// Get the network setup menu's node data
	struct gui_network_setup_menu_node_data * network_setup_menu_node_data = (struct gui_network_setup_menu_node_data *)this->parent->data;

	network_change_access_point(network_setup_menu_node_data->network_ap);
}

/**
 * @brief Activates the network setup context menu.
 */
static void gui_activate_network_setup_menu(struct gui_node * this)
{
	// Get the network setup menu node data
	struct gui_network_setup_menu_node_data * network_setup_menu_node_data = (struct gui_network_setup_menu_node_data *) this->data;

	// Get the network access point data
	struct network_access_point * network_access_point = network_setup_menu_node_data->network_ap;

	// Render the title text surface
	gui_activate_menu(this, network_access_point->ssid != NULL ? network_access_point->ssid : gettext("Manual configuration"));

	// The first node
	struct gui_node * first_node = NULL;

	// The previous node
	struct gui_node * previous_node = NULL;

	// Create a network setup ssid menu item
	osk_create_menu(this, &first_node, &previous_node, gettext("SSID"), network_access_point->ssid);

	// Create a network setup security menu item
	gui_create_network_setup_security_menu(this, &first_node, &previous_node, gettext("Security"), network_access_point->security);

	// Create a network setup password menu item
	osk_create_menu(this, &first_node, &previous_node, gettext("Password"), network_access_point->password);

	// Create a network setup connection menu item
	gui_create_context_menu_item(this, &first_node, &previous_node, gettext("Connection"), gui_change_network_setup_connection, NULL, NULL, NULL, NULL, NODE_TYPE_CONTEXT_MENU_NETWORK_SETUP_SETTER, NULL);

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
 * @brief Deactivates the network setup context menu.
 */
static void gui_deactivate_network_setup_menu(struct gui_node * this)
{
	// The menu has been activated (aka. its in focus, this check is necessary because deactivate gets called on parent node disposal as well)
	if (this->context->menu.active == this)
	{
		// Get the network setup menu node data
		struct gui_network_setup_menu_node_data * network_menu_node_data = (struct gui_network_setup_menu_node_data *)this->data;

		// Add the current network setup
		// TODO update network settings
		// this->context->settings.meridian_time = network_setup_security_menu_node_data->security;
		// p ((struct gui_network_setup_menu_node_data *)this->parent->data)->network_ap
		// p ((struct gui_network_setup_menu_node_data *)this->parent->data)->network_ap->security

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
 * @brief Re-renders the network setup context menu's surfaces.
 */
static void gui_invalidate_network_setup_menu(struct gui_node * this)
{
	// Create a temporary node
	struct gui_node * node = gui_create_context_menu_item(this, NULL, NULL, gettext("Manual configuration"), NULL, NULL, NULL, NULL, NULL, NODE_TYPE_CONTEXT_MENU_NETWORK_MENU, NULL);

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
