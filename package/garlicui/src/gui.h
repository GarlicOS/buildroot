#ifndef GUI_H
#define GUI_H

/**
 * @brief Forward definition of struct gui_node.
 */
struct gui_node;

/**
 * @brief Forward definition of struct gui_context.
 */
struct gui_context;

#include "io.h"
#include "battery.h"
#include "clock.h"
#include "icon.h"
#include "process.h"
#include "retroarch.h"
#include "localization.h"
#include "network.h"
#include "osk.h"

/**
 * @brief The reference width used for scaling.
 */
#define SCALING_REFERENCE_WIDTH 640

/**
 * @brief The reference height used for scaling.
 */
#define SCALING_REFERENCE_HEIGHT 480

/**
 * @brief Calculates a width-based scale factor.
 */
#define SCALE_FACTOR_WIDTH(current_width, reference_width) (((float)current_width) / ((float)reference_width))

/**
 * @brief Calculates a height-based scale factor.
 */
#define SCALE_FACTOR_HEIGHT(current_height, reference_height) (((float)current_height) / ((float)reference_height))

/**
 * @brief The small font scale factor.
 */
#define SMALL_FONT_SCALE_FACTOR 0.75f

/**
 * @brief The large font scale factor.
 */
#define LARGE_FONT_SCALE_FACTOR 1.25f

/**
 * @brief The gigantic font scale factor.
 */
#define GIGANTIC_FONT_SCALE_FACTOR 2.5f

/**
 * @brief The focused paging card height scale factor.
 */
#define FOCUSED_PAGING_CARD_HEIGHT_SCALE_FACTOR 0.6f

/**
 * @brief The unfocused paging card height scale factor.
 */
#define UNFOCUSED_PAGING_CARD_HEIGHT_SCALE_FACTOR 0.5f

/**
 * @brief The unfocused paging card width scale factor.
 */
#define UNFOCUSED_PAGING_CARD_WIDTH_SCALE_FACTOR 0.5f

/**
 * @brief The unscaled screen margin.
 */
#define SCREEN_MARGIN 16

/**
 * @brief The unscaled top bar height.
 */
#define DEFAULT_TOP_BAR_HEIGHT (DEFAULT_BAR_ICON_HEIGHT + (2 * SCREEN_MARGIN))

/**
 * @brief The unscaled bottom bar height.
 */
#define DEFAULT_BOTTOM_BAR_HEIGHT (localization_font_height() + (3 * SCREEN_MARGIN))

/**
 * @brief The unscaled notch width.
 */
#define DEFAULT_NOTCH_WIDTH 64

/**
 * @brief The unscaled notch height.
 */
#define DEFAULT_NOTCH_HEIGHT 256

/**
 * @brief The context menu node type.
 *
 * This menu is accessible from anywhere by pressing the mode button.
 * It's parent is always set to the focused menu at the time of pressing the mode button.
 */
#define NODE_TYPE_CONTEXT_MENU 0

/**
 * @brief The content resume action node type.
 */
#define NODE_TYPE_CONTEXT_MENU_RESUME_ACTION 1

/**
 * @brief The content restart action node type.
 */
#define NODE_TYPE_CONTEXT_MENU_RESTART_ACTION 2

/**
 * @brief The favorite action node type.
 */
#define NODE_TYPE_CONTEXT_MENU_FAVORITE_ACTION 3

/**
 * @brief The date and time menu node type.
 */
#define NODE_TYPE_CONTEXT_MENU_DATE_AND_TIME_MENU 4

/**
 * @brief Represents a date & time year setter.
 */
#define NODE_TYPE_CONTEXT_MENU_DATE_AND_TIME_YEAR_SETTER 5

/**
 * @brief Represents a date & time month setter.
 */
#define NODE_TYPE_CONTEXT_MENU_DATE_AND_TIME_MONTH_SETTER 6

/**
 * @brief Represents a date & time day setter.
 */
#define NODE_TYPE_CONTEXT_MENU_DATE_AND_TIME_DAY_SETTER 7

/**
 * @brief Represents a date & time hour setter.
 */
#define NODE_TYPE_CONTEXT_MENU_DATE_AND_TIME_HOUR_SETTER 8

/**
 * @brief Represents a date & time minute setter.
 */
#define NODE_TYPE_CONTEXT_MENU_DATE_AND_TIME_MINUTE_SETTER 9

/**
 * @brief The locale menu node type.
 */
#define NODE_TYPE_CONTEXT_MENU_LOCALE_MENU 10

/**
 * @brief Represents a locale setter.
 */
#define NODE_TYPE_CONTEXT_MENU_LOCALE_SETTER 11

/**
 * @brief The main menu node type.
 *
 * This menu acts as the Garlic home page and offers houses several submenus for content navigation.
 */
#define NODE_TYPE_MAIN_MENU 12

/**
 * @brief The library menu node type.
 *
 * This menu shows all content directories (any Library subdirectory that contains a config.xml file).
 */
#define NODE_TYPE_LIBRARY_MENU 13

/**
 * @brief The library directory menu node type.
 *
 * This menu shows all content items inside a given content directory.
 */
#define NODE_TYPE_LIBRARY_DIRECTORY_MENU 14

/**
 * @brief The library file node type.
 *
 * This node type identifies playable content.
 */
#define NODE_TYPE_LIBRARY_FILE 15

/**
 * @brief The continue playlist menu node type.
 *
 * This menu is populated by retroarch's history playlist.
 */
#define NODE_TYPE_PLAYLIST_CONTINUE_MENU 16

/**
 * @brief The favorites playlist menu node type.
 *
 * This menu is populated by retroarch's favorites playlist.
 */
#define NODE_TYPE_PLAYLIST_FAVORITES_MENU 17

/**
 * @brief The retroarch menu node type.
 *
 * While not handled by Garlic itself, activating this menu will bring up the retroarch main menu.
 */
#define NODE_TYPE_RETROARCH_MENU 18

/**
 * @brief The no game warning message displayed in playlist menus.
 */
#define NODE_TYPE_NO_GAME_WARNING_MESSAGE 19

/**
 * @brief The remove from history action node type.
 */
#define NODE_TYPE_CONTEXT_MENU_REMOVE_FROM_HISTORY_ACTION 20

/**
 * @brief Represents a date & time UTC offset setter.
 */
#define NODE_TYPE_CONTEXT_MENU_DATE_AND_TIME_UTC_OFFSET_SETTER 21

/**
 * @brief The on-screen keyboard menu node type.
 */
#define NODE_TYPE_CONTEXT_MENU_OSK_MENU 22

/**
 * @brief The network menu node type.
 */
#define NODE_TYPE_CONTEXT_MENU_NETWORK_MENU 23

/**
 * @brief The network setup menu node type.
 */
#define NODE_TYPE_CONTEXT_MENU_NETWORK_SETUP_MENU 24

/**
 * @brief The network setup setter.
 */
#define NODE_TYPE_CONTEXT_MENU_NETWORK_SETUP_SETTER 25

/**
 * @brief The network setup security menu node type.
 */
#define NODE_TYPE_CONTEXT_MENU_NETWORK_SETUP_SECURITY_MENU 26

/**
 * @brief The network setup security setter.
 */
#define NODE_TYPE_CONTEXT_MENU_NETWORK_SETUP_SECURITY_SETTER 27

/**
 * The supported buttons.
 */
#define POWER_BUTTON 0
#define SOUTH_BUTTON 1
#define EAST_BUTTON 2
#define VOLUME_DOWN_BUTTON 3
#define NORTH_BUTTON 4
#define WEST_BUTTON 5
#define VOLUME_UP_BUTTON 6
#define LEFT_BUMPER_BUTTON 7
#define RIGHT_BUMPER_BUTTON 8
#define SELECT_BUTTON 9
#define START_BUTTON 10
#define MODE_BUTTON 11
#define LEFT_THUMB_BUTTON 12
#define RIGHT_THUMB_BUTTON 13

/**
 * The supported axes.
 */
#define LEFT_ANALOG_X_AXIS 0
#define LEFT_ANALOG_Y_AXIS 1
#define LEFT_TRIGGER_AXIS 2
#define RIGHT_ANALOG_X_AXIS 3
#define RIGHT_ANALOG_Y_AXIS 4
#define RIGHT_TRIGGER_AXIS 5

/**
 * @brief Function pointer type for gui_node callback functions.
 *
 * This typedef defines the function pointer used by the gui_node callback functions.
 * These callbacks should be used to dynamically populate & free gui_node contents.
 * The signature is: void (*callback)(struct gui_node *);
 *
 * The first argument is a pointer to the node in question.
 * The callback function does not return a value.
 */
typedef void (*callback)(struct gui_node *);

/**
 * @brief Represents a single SDL surface + screen position.
 *
 * Most SDL surfaces created will eventually get blitted onto the screen surface.
 * Doing so requires a SDL rectangle to position it, so why not bundle these two together?
 */
struct gui_context_surface
{
	SDL_Surface * surface; /** The SDL surface */
	SDL_Rect position; /** The surface's screen position */
};

/**
 * @brief A list of top bar SDL surfaces.
 */
struct gui_context_top_bar_surfaces
{
	struct gui_context_surface logo; /** The top bar logo */
	struct gui_context_surface clock; /** The top bar clock */
	struct gui_context_surface battery; /** The top bar battery */
};

/**
 * @brief A list of bottom bar action legend SDL surfaces.
 */
struct gui_context_bottom_bar_action_surfaces
{
	struct gui_context_surface select; /** The select action legend */
	struct gui_context_surface back; /** The back action legend */
	struct gui_context_surface menu; /** The menu action legend */
};

/**
 * @brief A list of bottom bar SDL surfaces.
 */
struct gui_context_bottom_bar_surfaces
{
	struct gui_context_bottom_bar_action_surfaces actions; /** A list of action legend SDL surfaces */
};

/**
 * @brief A list of top & bottom bar SDL surfaces.
 */
struct gui_context_bar_surfaces
{
	struct gui_context_top_bar_surfaces top; /** The top bar surfaces */
	struct gui_context_bottom_bar_surfaces bottom; /** The bottom bar surfaces */
};

/**
 * @brief A list of overlay SDL surfaces.
 */
struct gui_context_overlay_surfaces
{
	struct gui_context_surface notch; /** The scroll letter notch */
	struct gui_context_surface osk; /** The on-screen keyboard */
	struct gui_context_surface osk_buffer; /** The on-screen keyboard buffer */
	struct gui_context_surface osk_active_key; /** The on-screen keyboard active key */
};

/**
 * @brief A list of context-bound SDL surfaces.
 */
struct gui_context_surfaces
{
	float scale_factor; /** The scale factor */
	SDL_Surface * screen; /** SDL's screen surface */
	struct gui_context_bar_surfaces bars; /** A list of top & bottom bar SDL surfaces */
	struct gui_context_overlay_surfaces overlays; /** A list of overlay SDL surfaces */
};

/**
 * @brief The available fonts of the context.
 */
struct gui_context_fonts
{
	TTF_Font * normal; /** The font used to render most text */
	TTF_Font * small; /** The small font used to render subtitles and smaller texts */
	TTF_Font * large; /** The large font used to render certain overlays and larger texts */
	TTF_Font * gigantic; /** The large font used to render certain overlays and larger texts */
};

/**
 * @brief A data structure representing a background-foreground color set.
 */
struct gui_context_bgfg_color
{
	Uint32 background; /** The background color */
	Uint32 foreground; /** The foreground color */
};

/**
 * @brief The theme colors of the context.
 */
struct gui_context_colors
{
	struct gui_context_bgfg_color base; /** The base colors */
	struct gui_context_bgfg_color legend; /** The legend colors */
	struct gui_context_bgfg_color icon; /** The icon colors */
	struct gui_context_bgfg_color notch; /** The notch colors */
	struct gui_context_bgfg_color osk; /** The on-screen keyboard colors */
};

/**
 * @brief The battery status.
 */
struct gui_context_battery_status
{
	int percentage; /** The current battery percentage */
	int charging; /** The current charging status */
};

/**
 * @brief The clock status.
 */
struct gui_context_clock_status
{
	char current_time[16]; /** The current time as a formatted string */
};

/**
 * @brief The application status.
 */
struct gui_context_application_status
{
	int running; /** The main loop's exit condition */
	int exit_code; /** The main loop's exit code */
};

/**
 * @brief The context status.
 */
struct gui_context_status
{
	struct gui_context_application_status application; /** The application status */
	struct gui_context_clock_status clock; /** The RTC status */
	struct gui_context_battery_status battery; /** The battery status */
};

/**
 * @brief Represents a input state snapshot.
 */
struct gui_context_input_state
{
	int dpad_x; /** The horizontal D-pad axis */
	int dpad_y; /** The vertical D-pad axis */
	int left_analog_x; /** The left analog stick's horizontal axis */
	int left_analog_y; /** The left analog stick's vertical axis */
	int right_analog_x; /** The right analog stick's horizontal axis */
	int right_analog_y; /** The right analog stick's vertical axis */
	int left_trigger; /** The left trigger axis */
	int right_trigger; /** The right trigger axis */
	int left_thumb; /** The left thumb button */
	int right_thumb; /** The right thumb button */
	int left_bumper; /** The left bumper */
	int right_bumper; /** The right bumper */
	int north; /** The north face button */
	int east; /** The east face button */
	int south; /** The south face button */
	int west; /** The west face button */
	int select; /** The select button */
	int start; /** The start button */
	int mode; /** The home / menu / mode button */
	int volume_up; /** The volume up button */
	int volume_down; /** The volume down button */
	int power; /** The power button */
};

/**
 * @brief A context input device.
 */
struct gui_context_input
{
	SDL_Joystick * joystick; /** The SDL joystick */
	struct gui_context_input_state previous; /** The previous frame's input state snapshot */
	struct gui_context_input_state current; /** The current frame's input state snapshot */
};

/**
 * @brief The context input devices.
 */
struct gui_context_inputs
{
	struct gui_context_input internal; /** The internal gamepad */
};

/**
 * @brief The context settings.
 */
struct gui_context_settings
{
	int meridian_time; /** The user's current meridian time setting */
	char locale[16]; /** The user's current locale setting (intentionally 1 frame late so surfaces can be re-rendered) */
	char ui_state[128 + PATH_MAX * 2]; /** The UI state of the previous GarlicUI instance (populated on load, restored post-main-menu activation) */
	int resume; /** The auto-resume flag */
	int network_ap_count; /** The number of user's network access points */
	struct network_access_point ** network_aps; /** The user's network access points */
};

/**
 * @brief The context's node tree.
 */
struct gui_context_node_tree
{
	struct gui_node * root; /** The root node */
	struct gui_node * active; /** The currently active (visible) node */
};

/**
 * @brief A data structure representing the user interface context.
 *
 * The user interface context contains often-used system-wide resources.
 * These are things like the SDL screen surface, font instances, pre-rasterized vector images, etc.
 */
struct gui_context
{
	struct gui_context_surfaces surfaces; /** The initialized surfaces */
	struct gui_context_fonts fonts; /** The initialized fonts */
	struct gui_context_colors colors; /** The theme colors */
	struct gui_context_inputs inputs; /** The context input devices */
	struct gui_context_status status; /** The context status */
	struct gui_context_settings settings; /** The context settings */
	struct gui_context_node_tree menu; /** The menu tree nodes */
	struct osk_context osk; /** The on-screen keyboard context */
};

/**
 * @brief A data structure representing a basic item node.
 *
 * Item nodes are the core of all user interface components.
 * They are used in menus, items, item lists, etc. and can be expanded upon with optional data.
 */
struct gui_node
{
	struct gui_context * context; /** The user interface context */
	struct gui_node * parent; /** The parent node */
	struct gui_node * children; /** A pointer to the "first" child node (used as an anchor point for rendering moving pointer menus) */
	struct gui_node * child; /** The selected child node */
	struct gui_node * previous; /** The previous sibling node */
	struct gui_node * next; /** The next sibling node */
	callback activate; /** The activate callback (which should be used to populate the node) */
	callback deactivate; /** The deactivate callback (which should be used to free resources) */
	callback update; /** The update callback (which triggers once per frame when the node is active) */
	callback render; /** The render callback (which triggers once per frame when the node is active) */
	callback invalidate; /** The invalidate callback (which triggers when surfaces need to be re-rendered) */
	SDL_Surface * normal_surface; /** The surface representing the node's pre-rendered normal state representation */
	SDL_Surface * selected_surface; /** The surface representing the node's pre-rendered selected state representation */
	int type; /** The GUI node type */
	void * data; /** The optional node-specific data */
};

/**
 * @brief A data structure holding commonly shared menu node-specific data.
 */
struct gui_menu_node_data
{
	struct gui_context_surface title; /** The menu title */
};

/**
 * @brief A data structure holding RTC data.
 */
struct gui_rtc_menu_node_data
{
	struct gui_menu_node_data base; /** The base menu data */
	time_t local_time; /** The local time (set on activation) */
	int utc_offset; /** The local timezone as a UTC offset (set on activation) */
	int meridian_time; /** The current meridian time setting (set on activation) */
};

/**
 * @brief A data structure holding locale data.
 */
struct gui_locale_menu_node_data
{
	struct gui_menu_node_data base; /** The base menu data */
	char locale[16]; /** The current locale */
};

/**
 * @brief A data structure holding network data.
 */
struct gui_network_menu_node_data
{
	struct gui_menu_node_data base; /** The base menu data */
	int network_ap_count; /** The number of network access points */
	struct network_access_point ** network_aps; /** The network access points */
};

/**
 * @brief A data structure holding network setup data.
 */
struct gui_network_setup_menu_node_data
{
	struct gui_menu_node_data base; /** The base menu data */
	struct network_access_point * network_ap; /** The network access point */
};

/**
 * @brief A data structure holding network setup security data.
 */
struct gui_network_setup_security_menu_node_data
{
	struct gui_menu_node_data base; /** The base menu data */
	char * security; /** The current security setting */
};

/**
 * @brief A data structure holding on-screen keyboard data.
 */
struct gui_osk_menu_node_data
{
	struct gui_menu_node_data base; /** The base menu data */
	char * title; /** The on-screen keyboard title */
	char * buffer; /** The on-screen keyboard buffer */
};

/**
 * @brief A extended version of gui_menu_node_data that can accommodate a path.
 */
struct gui_path_menu_node_data
{
	struct gui_menu_node_data base; /** The base menu data */
	char path[PATH_MAX]; /** The path */
	xmlDocPtr configuration; /** The path-specific configuration */
};

/**
 * @brief A data structure representing ROM menu node-specific data.
 *
 * ROM menu nodes are displayed in a variety of different ways depending on which menu houses them.
 * The recent games menu displays the game's last screenshot as a background with a bottom-centered file name.
 * The favorites and regular content system submenus displays them as a combination of background, artwork and file name.
 * This data structure holds all the data necessary for that.
 */
struct gui_rom_node_data
{
	char * core_path; /** The path to the libretro core used to emulate this ROM */
	char * rom_path; /** The path to the ROM itself */
	char * artwork_path; /** The path to the ROM's artwork image */
	char * background_path; /** The path to the ROM's background image */
	SDL_Surface * artwork; /** The loaded ROM artwork image */
	SDL_Surface * background; /** The loaded ROM background image */
};

/**
 * @brief Creates a new GUI context.
 *
 * This function allocates and initializes a new GUI context structure.
 * The generated context object must be freed explicitly via gui_destroy_context.
 */
struct gui_context * gui_create_context(int argc, char * argv[]);

/**
 * @brief Frees the given GUI context.
 *
 * This function shuts down and frees the given GUI context structure.
 */
void gui_destroy_context(struct gui_context * context);

/**
 * @brief Creates a basic node.
 *
 * This function allocates and populates a basic GUI node structure.
 */
struct gui_node * gui_create_node(struct gui_context * context, callback activate, callback deactivate, callback update, callback render, callback invalidate, int type, void * data);

/**
 * @brief Frees the given node.
 *
 * This function deactivates and frees the given GUI node structure.
 */
void gui_destroy_node(struct gui_node * node, int free_root);

/**
 * @brief Polls for input and executes logic-related tasks.
 *
 * This is essentially the brain of the operation.
 * It reads joystick input and executes actions.
 */
void gui_update(struct gui_context * context);

/**
 * @brief Renders the user interface.
 *
 * This is essentially the brawn of the operation.
 * It renders the current user interface state.
 */
void gui_render(struct gui_context * context);

/**
 * @brief Reads the current configuration from disk.
 */
void gui_read_configuration(struct gui_context * context);

/**
 * @brief Writes the current configuration to disk.
 */
void gui_write_configuration(struct gui_context * context);

#endif
