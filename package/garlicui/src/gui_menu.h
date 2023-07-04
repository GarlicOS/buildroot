#ifndef GUI_MENU_H
#define GUI_MENU_H

#include "gui.h"

/**
 * The initial scroll repeat interval.
 */
#define SCROLL_EVENT_INITIAL_REPEAT_INTERVAL 250

/**
 * The minimum scroll repeat interval.
 */
#define SCROLL_EVENT_MINIMUM_REPEAT_INTERVAL 50

/**
 * The acceleration (decrement) of the scroll repeat interval.
 */
#define SCROLL_EVENT_ACCELERATION 100

/**
 * The interval in which the scroll repeat interval gets accelerated (decreased).
 */
#define SCROLL_EVENT_ACCELERATION_TIMESPAN 500

/**
 * @brief Activates a basic menu, pre-rendering its title text surface.
 */
void gui_activate_menu(struct gui_node * this, const char * title);

/**
 * @brief Deactivates a basic menu, freeing its title text surface.
 */
void gui_deactivate_menu(struct gui_node * this);

/**
 * @brief Handles basic menu related input events.
 *
 * This basic handler is good enough for horizontal / vertical navigation as well as activating and de-activating nodes.
 */
void gui_update_basic_menu(struct gui_node * this);

/**
 * @brief Renders the given node's children as a paging icon menu.
 *
 * A paging icon menu is a horizontal endlessly scrolling list of item.
 */
void gui_render_paging_icon_menu(struct gui_node * this);

/**
 * @brief Renders the given node's children as a vertical scroll list menu.
 */
void gui_render_vertical_scrolling_list_menu(struct gui_node * this);

#endif