#ifndef IO_H
#define IO_H

#include <SDL/SDL.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlsave.h>

/**
 * @brief A text formatter callback that transforms the given input in some way.
 */
typedef char * (*textformatter)(const char *);

/**
 * @brief The directory in which all boot related files are stored.
 *
 * This includes the boot script, rootfs, global configuration file, etc.
 */
#ifdef __MACOSX__
#define FOLDER_CONFIGURATION_BOOT_FOLDER ""
#else
#define FOLDER_CONFIGURATION_BOOT_FOLDER "/media/boot/"
#endif

/**
 * @brief The folder that contains the game library template.
 */
#define LIBRARY_FOLDER_TEMPLATE_PATH "/usr/share/library"

/**
 * @brief The folder that contains the game library.
 */
#define LIBRARY_FOLDER_PATH "/media/Library"

/**
 * @brief The folder that contains the icons.
 */
#define ICON_FOLDER_TEMPLATE_PATH "/usr/share/icons"

/**
 * @brief The folder that contains the icon overrides.
 */
#define ICON_FOLDER_PATH FOLDER_CONFIGURATION_BOOT_FOLDER "icons"

/**
 * @brief The folder icon file name.
 */
#define FOLDER_ICON_FILE_NAME "icon.svg"

/**
 * @brief The folder configuration file name.
 */
#define FOLDER_CONFIGURATION_FILE_NAME "config.xml"

/**
 * @brief The folder configuration XML element that contains the runner.
 */
#define FOLDER_CONFIGURATION_RUN_ELEMENT_NAME "Run"

/**
 * @brief A macro to extract RGB components from a Uint32 color value.
 */
#define GET_RGB_COMPONENTS(color, r, g, b) \
	do { \
		r = (color >> 16) & 0xFF; \
		g = (color >> 8) & 0xFF; \
		b = color & 0xFF; \
	} while(0)

/**
 * @brief Extracts the file name from the given fully qualified path.
 */
const char * io_get_file_name(const char * path);

/**
 * @brief Extracts the directory name from the given fully qualified path.
 *
 * The caller is responsible for freeing the allocated memory.
 */
char * io_get_directory_name(const char * path);

/**
 * @brief Returns a somewhat more pleasant to look at display name by removing extension, brackets and excessive whitespaces.
 *
 * The caller is responsible for freeing the allocated memory.
 */
char * io_get_display_name(const char * path);

/**
 * @brief Removes all encapsulated blocks that match the given block start and end delimiters.
 *
 * The caller is responsible for freeing the allocated memory.
 */
char * io_remove_encapsulated_blocks(const char * text, const char * block_start, const char * block_end);

/**
 * @brief Removes duplicate whitespaces.
 *
 * The caller is responsible for freeing the allocated memory.
 */
char * io_remove_duplicate_whitespaces(const char * text);

/**
 * @brief Trims leading and trailing whitespaces from the given text.
 *
 * The caller is responsible for freeing the allocated memory.
 */
char * io_trim_whitespaces(const char * text);

/**
 * @brief Removes the extension from the given file name.
 *
 * The caller is responsible for freeing the allocated memory.
 */
char * io_remove_extension(const char * path);

/**
 * @brief Reads the file at the given path and attempts to parse its content as an integer.
 *
 * This is mostly used to read sysfs / procfs / devfs nodes.
 */
int io_read_int(const char * path, int * value);

/**
 * @brief Sorts the given item array.
 */
void io_sort_items(int item_count, char ** items, textformatter formatter);

/**
 * @brief Returns a list of content items.
 *
 * The returned list must be explicitly destroyed with io_free_item_list.
 */
int io_get_content_items(const char * path, char *** items, int include_files, int include_directories, textformatter formatter);

/**
 * @brief Frees an item list.
 */
void io_free_item_list(int itemCount, char *** items);

/**
 * @brief Reads the given folder's configuration file.
 */
xmlDocPtr io_read_folder_configuration(const char * path);

/**
 * @brief Writes the given configuration document into the given folder.
 */
int io_write_folder_configuration(xmlDocPtr document, const char * path);

/**
 * @brief Gets the file and directory listing flags from the given configuration document.
 */
void io_get_file_type_whitelist(xmlDocPtr document, int * list_files, int * list_directories);

/**
 * @brief Checks whether the given path matches the given configuration document.
 */
int io_path_matches_configuration(const char * path, xmlDocPtr document);

/**
 * @brief Gets the given skin color from the given configuration document.
 */
Uint32 io_get_skin_color(xmlDocPtr document, const char * category_name, const char * type_name, Uint32 default_color);

/**
 * @brief Checks the given path for files that match the given configuration document's extension list.
 */
int io_does_folder_contain_games(const char * path, int include_files, int include_directories, xmlDocPtr document);

/**
 * @brief Syncs all pending write operations to disk.
 */
void io_sync();

/**
 * @brief Shuts the device down.
 */
void io_shutdown();

/**
 * @brief Copies the given file.
 */
void io_copy_file(const char * src_path, const char * dest_path);

/**
 * @brief Copies the given directory (but only if the destination directory doesn't exist yet).
 */
void io_copy_directory(const char * src_dir, const char * dest_dir);

/**
 * @brief Unpacks a variety of needed resources.
 */
void io_unpack_resources();

#endif
