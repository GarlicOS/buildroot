#ifndef RETROARCH_H
#define RETROARCH_H

/**
 * @brief The default retroarch configuration file.
 */
#define RETROARCH_DEFAULT_CONFIG_FILE_PATH "/root/.config/retroarch/retroarch.cfg"

/**
 * @brief The default retroarch core path format string.
 */
#define RETROARCH_DEFAULT_CORE_FILE_PATH "/root/.config/retroarch/cores/%s_libretro.so"

/**
 * @brief The default retroarch history playlist file.
 */
#define RETROARCH_DEFAULT_HISTORY_PLAYLIST_FILE_PATH "/media/RetroArch/content_history.lpl"

/**
 * @brief The default retroarch favorites playlist file.
 */
#define RETROARCH_DEFAULT_FAVORITES_PLAYLIST_FILE_PATH "/media/RetroArch/content_favorites.lpl"

/**
 * @brief The default retroarch savestate screenshot path format string.
 */
#define RETROARCH_DEFAULT_AUTO_SAVESTATE_SCREENSHOT_FILE_PATH "/media/RetroArch/states/%s.state.auto.png"

/**
 * @brief The retroarch thumbnails subfolder that contains the title screen screenshots.
 */
#define RETROARCH_NAMED_TITLES "Named_Titles"

/**
 * @brief The retroarch thumbnails subfolder that contains the in-game screenshots.
 */
#define RETROARCH_NAMED_SNAPS "Named_Snaps"

/**
 * @brief The retroarch thumbnails subfolder that contains the boxarts.
 */
#define RETROARCH_NAMED_BOXARTS "Named_Boxarts"

/**
 * @brief The default retroarch thumbnail path format string.
 */
#define RETROARCH_DEFAULT_THUMBNAIL_FILE_PATH "/media/RetroArch/thumbnails/%s/%s/%s.png"

/**
 * @brief A single retroarch playlist item.
 */
struct retroarch_playlist_item
{
	char * path;
	char * label;
	char * core_path;
	char * core_name;
	char * crc32;
	char * db_name;
};

/**
 * @brief Starts the given game with the given core.
 */
int retroarch_run(const char * core, const char * game);

/**
 * @brief Gets the given game's auto savestate path.
 *
 * The returned buffer must be freed by the caller.
 */
char * retroarch_get_auto_savestate_path(const char * game);

/**
 * @brief Gets the given game's auto savestate screenshot path.
 *
 * The returned buffer must be freed by the caller.
 */
char * retroarch_get_auto_savestate_screenshot_path(const char * game);

/**
 * @brief Gets the given game's retroarch thumbnail.
 *
 * The returned buffer must be freed by the caller.
 */
char * retroarch_get_thumbnail_path(const char * game, const char * thumbnail_type);

/**
 * @brief Returns the first item from the given playlist that matches the given path.
 *
 * The caller must explicitly free the returned item by calling retroarch_free_playlist_item.
 */
struct retroarch_playlist_item * retroarch_get_playlist_item(const char * playlist, const char * path);

/**
 * @brief Free the given playlist item.
 */
void retroarch_free_playlist_item(struct retroarch_playlist_item * item);

/**
 * @brief Returns all items contained in the given playlist.
 */
int retroarch_get_playlist_items(const char * playlist, struct retroarch_playlist_item *** items);

/**
 * @brief Frees the given playlist items.
 */
void retroarch_free_playlist_items(struct retroarch_playlist_item ** items, int count);

/**
 * @brief Removes all items that match the given path from the given playlist.
 */
void retroarch_remove_playlist_items_by_path(const char * playlist, const char * path);

/**
 * @brief Adds the given item to the given playlist.
 */
void retroarch_add_playlist_item(const char * playlist, struct retroarch_playlist_item * item);

/**
 * @brief Attempts to resume the last played game.
 */
void retroarch_resume_last_game();

#endif