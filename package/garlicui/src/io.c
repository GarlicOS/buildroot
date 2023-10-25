#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <limits.h>
#include <ctype.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include "io.h"

/**
 * @brief Extracts the file name from the given fully qualified path.
 */
const char * io_get_file_name(const char * path)
{
	// Find the last occurence of the separator
	const char * fileName = strrchr(path, '/');

	// The path contains a separator
	if (fileName != NULL)
	{
		// Increment the pointer to exclude the separator
		fileName++;
	}

	// The path doesn't contain a separator
	else
	{
		// Return the original path
		fileName = path;
	}

	// Return the file name
	return fileName;
}

/**
 * @brief Extracts the directory name from the given fully qualified path.
 *
 * The caller is responsible for freeing the allocated memory.
 */
char * io_get_directory_name(const char * path)
{
	// The directory name
	char * directoryName = NULL;

	// Find the last occurrence of the separator
	const char * fileName = strrchr(path, '/');

	// The path contains a separator
	if (fileName != NULL)
	{
		// Calculate the length of the directory name
		size_t directoryLen = fileName - path;

		// Allocate memory for the directory name (+1 for the null terminator)
		directoryName = (char *)malloc(directoryLen + 1);

		// We managed to allocate memory for the directory name
		if (directoryName != NULL)
		{
			// Copy the directory name
			strncpy(directoryName, path, directoryLen);

			// Add the null terminator
			directoryName[directoryLen] = 0;
		}
	}

	// The path doesn't contain a separator
	else
	{
		// Return a copy of the original path
		directoryName = strdup(path);
	}

	// Return the directory name
	return directoryName;
}

/**
 * @brief Returns a somewhat more pleasant to look at display name by removing extension, brackets and excessive whitespaces.
 *
 * The caller is responsible for freeing the allocated memory.
 */
char * io_get_display_name(const char * path)
{
	// The returned buffer
	char * result = NULL;

	// We've been given a valid path to work with
	if (path != NULL)
	{
		// Remove the extension
		char * display_name = io_remove_extension(io_get_file_name(path));

		// We managed to remove the extension
		if (display_name != NULL)
		{
			// Remove the [] blocks
			char * temporary_display_name = io_remove_encapsulated_blocks(display_name, "[", "]");

			// We managed to remove the [] blocks
			if (temporary_display_name != NULL)
			{
				// Free the previous display name
				free(display_name);

				// And replace it
				display_name = temporary_display_name;
			}

			// Remove the () blocks
			temporary_display_name = io_remove_encapsulated_blocks(display_name, "(", ")");

			// We managed to remove the () blocks
			if (temporary_display_name != NULL)
			{
				// Free the previous display name
				free(display_name);

				// And replace it
				display_name = temporary_display_name;
			}

			// Remove duplicate whitespaces
			temporary_display_name = io_remove_duplicate_whitespaces(display_name);

			// We managed to remove the duplicate whitespaces
			if (temporary_display_name != NULL)
			{
				// Free the previous display name
				free(display_name);

				// And replace it
				display_name = temporary_display_name;
			}

			// Trim leading and trailing whitespaces
			temporary_display_name = io_trim_whitespaces(display_name);

			// We managed to remove the duplicate whitespaces
			if (temporary_display_name != NULL)
			{
				// Free the previous display name
				free(display_name);

				// And replace it
				display_name = temporary_display_name;
			}

			// Return the display name
			result = display_name;
		}
	}

	// Return the buffer
	return result;
}

/**
 * @brief Removes all encapsulated blocks that match the given block start and end delimiters.
 *
 * The caller is responsible for freeing the allocated memory.
 */
char * io_remove_encapsulated_blocks(const char * text, const char * block_start, const char * block_end)
{
	// The length of the input text
	size_t len = strlen(text);

	// The length of the block start delimiter
	size_t block_start_len = strlen(block_start);

	// The length of the block end delimiter
	size_t block_end_len = strlen(block_end);

	// Allocate memory for the transformed text
	char * transformed_text = (char *)malloc(len + 1);

	// We managed to allocate memory for the transformed text
	if (transformed_text != NULL)
	{
		// The input buffer position
		size_t i = 0;

		// The output buffer position
		size_t j = 0;

		// Iterate all input characters
		while (text[i] != 0)
		{
			// Check if the current position matches the block start delimiter
			if (strncmp(&text[i], block_start, block_start_len) == 0)
			{
				// Skip the block start delimiter
				i += block_start_len;

				// Move towards the block end delimiter
				while (text[i] != 0 && strncmp(&text[i], block_end, block_end_len) != 0)
				{
					// And skip each character along the way
					i++;
				}

				// We've found a block end delimiter
				if (text[i] != 0)
				{
					// Skip the block end delimiter
					i += block_end_len;
				}
			}

			// We're outside of an encapsulated block
			else
			{
				// Copy the character to the transformed text
				transformed_text[j] = text[i];

				// Increment the input buffer position
				i++;

				// Increment the output buffer position
				j++;
			}
		}

		// Terminate the transformed text
		transformed_text[j] = 0;
	}

	// Return the transformed text
	return transformed_text;
}

/**
 * @brief Removes duplicate whitespaces.
 *
 * The caller is responsible for freeing the allocated memory.
 */
char * io_remove_duplicate_whitespaces(const char * text)
{
	// The length of the input text
	size_t len = strlen(text);

	// Allocate a buffer for the transformed text
	char * transformed_text = (char *)malloc(len + 1);

	// We managed to allocate a buffer that's big enough to hold the input text
	if (transformed_text != NULL)
	{
		// The input buffer position
		size_t i = 0;

		// The output buffer position
		size_t j = 0;

		// Whether the previous character was a whitespace character
		int prev_char_is_space = 0;

		// Iterate all input characters
		while (text[i] != 0)
		{
			// We've encountered a whitespace character
			if (isspace(text[i]))
			{
				// The previous character wasn't a whitespace
				if (!prev_char_is_space)
				{
					// Add a space
					transformed_text[j] = ' ';

					// Increment the output buffer position
					j++;

					// And toggle the whitespace flag
					prev_char_is_space = 1;
				}
			}

			// We've encountered a printable character
			else
			{
				// Add the character
				transformed_text[j] = text[i];

				// Increment the output buffer position
				j++;

				// Erase the whitespace flag
				prev_char_is_space = 0;
			}

			// Increment the input buffer position
			i++;
		}

		// Terminate the transformed text
		transformed_text[j] = 0;
	}

	// Return the transformed text
	return transformed_text;
}

/**
 * @brief Trims leading and trailing whitespaces from the given text.
 *
 * The caller is responsible for freeing the allocated memory.
 */
char * io_trim_whitespaces(const char * text)
{
	// The length of the input text
	size_t len = strlen(text);

	// The index of the first non-whitespace character
	size_t start = 0;

	// The index of the last non-whitespace character
	size_t end = len - 1;

	// Find the first non-whitespace character
	while (start < len && isspace(text[start]))
	{
		// Increment the start index
		start++;
	}

	// Find the last non-whitespace character
	while (end > start && isspace(text[end]))
	{
		// Decrement the end index
		end--;
	}

	// Calculate the length of the trimmed text
	size_t trimmed_len = end - start + 1;

	// Allocate memory for the trimmed text
	char * trimmed_text = (char *)malloc(trimmed_len + 1);

	// We managed to allocate memory for the trimmed text
	if (trimmed_text != NULL)
	{
		// Copy the trimmed text to the newly allocated buffer
		strncpy(trimmed_text, text + start, trimmed_len);

		// And terminate the trimmed text
		trimmed_text[trimmed_len] = 0;
	}

	// Return the trimmed text
	return trimmed_text;
}

/**
 * @brief Removes the extension from the given file name.
 *
 * The caller is responsible for freeing the allocated memory.
 */
char * io_remove_extension(const char * path)
{
	// Duplicate the path
	char * duplicate = strdup(path);

	// We managed to duplicate the path
	if (duplicate != NULL)
	{
		// Find the extension
		char * extension = strrchr(duplicate, '.');

		// We found the extension
		if (extension != NULL)
		{
			// Remove the extension
			*extension = 0;
		}
	}

	// Return the extension-less duplicate
	return duplicate;
}

/**
 * @brief Reads the file at the given path and attempts to parse its content as an integer.
 *
 * This is mostly used to read sysfs / procfs / devfs nodes.
 */
int io_read_int(const char * path, int * value)
{
	// The parsing result
	int result = 0;

	// We've been given valid arguments
	if (path != NULL && value != NULL)
	{
		// Open the file for reading
		FILE * file = fopen(path, "r");

		// We managed to open the file for reading
		if (file != NULL)
		{
			// Attempt to parse the file content into an integer
			if (fscanf(file, "%d", value))
			{
				// The parsing was successful
				result = 1;
			}

			// And close the battery capacity sysfs node
			fclose(file);
		}
	}

	// Return the parsing result
	return result;
}

/**
 * @brief Sorts the given item array.
 */
void io_sort_items(int item_count, char ** items, textformatter formatter)
{
	// Iterate all items
	for (int i = 0; i < item_count; i++)
	{
		// Iterate all items
		for (int j = 0; j < item_count - 1 - i; j++)
		{
			// Format the items
			char * item_a = formatter != NULL ? formatter(items[j]) : items[j];
			char * item_b = formatter != NULL ? formatter(items[j + 1]) : items[j + 1];

			// Compare the items
			if (item_a != NULL && item_b != NULL && strcmp(item_a, item_b) > 0)
			{
				// Sort the items
				char * temp = items[j];
				items[j] = items[j + 1];
				items[j + 1] = temp;
			}

			// We've been formatting the first item
			if (formatter != NULL && item_a != NULL)
			{
				// Free the formatted item
				free(item_a);
			}

			// We've been formatting the second item
			if (formatter != NULL && item_b != NULL)
			{
				// Free the formatted item
				free(item_b);
			}
		}
	}
}

/**
 * @brief Returns a list of content items.
 *
 * The returned list must be explicitly destroyed with io_free_item_list.
 */
int io_get_content_items(const char * path, char *** items, int include_files, int include_directories, textformatter formatter)
{
	// The number of discovered items
	int item_count = 0;

	// The current directory entry
	struct dirent * entry;

	// Open the given directory
	DIR * directory = opendir(path);

	// The collected items
	char ** collected_items = NULL;

	// The relative path buffer
	char relative_path[PATH_MAX + 1];

	// The absolute path buffer
	char absolute_path[PATH_MAX + 1];

	// Iterate all items
	while ((entry = readdir(directory)) != NULL)
	{
		// The file statistics
		struct stat st;

		// Ignore known bad file names
		if (entry->d_name[0] == '.' || entry->d_name[0] == '~' || strstr(entry->d_name, "_cache") || strcmp(entry->d_name, FOLDER_CONFIGURATION_FILE_NAME) == 0 || strcmp(entry->d_name, FOLDER_ICON_FILE_NAME) == 0)
		{
			// Move to the next item on match
			continue;
		}

		// Get the file statistics
		if (fstatat(dirfd(directory), entry->d_name, &st, 0) < 0)
		{
			// Move to the next item on error
			continue;
		}

		// The caller doesn't want files
		if (!S_ISDIR(st.st_mode) && !include_files)
		{
			// Move to the next item
			continue;
		}

		// The caller doesn't want directories
		if (S_ISDIR(st.st_mode) && !include_directories)
		{
			// Move to the next item
			continue;
		}

		// Put together a relative path
		sprintf(relative_path, "%s/%s", path, entry->d_name);

		// And convert it to an absolute path
		if (realpath(relative_path, absolute_path))
		{
			// Increment the item count
			item_count++;

			// Increase the item array size
			collected_items = realloc(collected_items, item_count * sizeof(char *));

			// Duplicate the absolute path and store it inside the array
			collected_items[item_count - 1] = strdup(absolute_path);
		}
	}

	// Close the directory
	closedir(directory);

	// Sort the items
	io_sort_items(item_count, collected_items, formatter);

	// Return the item array
	*items = collected_items;

	// Return the item count
	return item_count;
}

/**
 * @brief Frees an item list.
 */
void io_free_item_list(int itemCount, char *** items)
{
	// The passed parameters seem invalid
	if (itemCount == 0 || items == NULL || *items == NULL)
	{
		// Exit out early
		return;
	}

	// Iterate all items
	for (int i = 0; i < itemCount; i++)
	{
		// Free the item
		free(items[0][i]);
	}

	// Free the item array
	free(items[0]);

	// NULL the array pointer
	*items = NULL;
}

/**
 * @brief Reads the given folder's configuration file.
 */
xmlDocPtr io_read_folder_configuration(const char * path)
{
	// The folder configuration
	xmlDocPtr configuration = NULL;

	// Allocate a buffer that's big enough to hold the full configuration file path
	char * configuration_file_path = calloc(1, strlen(path) + 1 + strlen(FOLDER_CONFIGURATION_FILE_NAME) + 1);

	// We managed to allocate a buffer for the configuration file path
	if (configuration_file_path != NULL)
	{
		// Puzzle together the configuration file path
		sprintf(configuration_file_path, "%s/%s", path, FOLDER_CONFIGURATION_FILE_NAME);

		// Read the folder configuration
		configuration = xmlReadFile(configuration_file_path, NULL, 0);

		// Free the configuration file path buffer
		free(configuration_file_path);
	}

	// Return the folder configuration
	return configuration;
}

/**
 * @brief Writes the given configuration document into the given folder.
 */
int io_write_folder_configuration(xmlDocPtr document, const char * path)
{
	// The write result
	int result = 0;

	// Allocate a file path buffer
	char * file_path = malloc(strlen(path) + 1 + strlen(FOLDER_CONFIGURATION_FILE_NAME) + 1);

	// We managed to allocate a file path buffer
	if (file_path != NULL)
	{
		// Puzzle together the file path
		sprintf(file_path, "%s/%s", path, FOLDER_CONFIGURATION_FILE_NAME);

		// Write the document to the file
		if (xmlSaveFormatFile(file_path, document, 1) > 0)
		{
			// Set the write result
			result = 1;
		}

		// Free the file path buffer
		free(file_path);
	}

	// Return the write result
	return result;
}

/**
 * @brief Gets the file and directory listing flags from the given configuration document.
 */
void io_get_file_type_whitelist(xmlDocPtr document, int * list_files, int * list_directories)
{
	// Set the default values
	*list_files = 1;
	*list_directories = 0;

	// Get the root element of the XML document
	xmlNodePtr root = xmlDocGetRootElement(document);

	// Iterate the child nodes
	for (xmlNodePtr node = root->children; node != NULL; node = node->next)
	{
		// Ensure we're working with XML element nodes
		if (node->type == XML_ELEMENT_NODE)
		{
			// We've found the <ListFiles> node
			if (xmlStrcmp(node->name, BAD_CAST "ListFiles") == 0)
			{
				// Get the content of the node
				xmlChar * content = xmlNodeGetContent(node);

				// We managed to get the content of the node
				if (content != NULL)
				{
					// Set the file listing flag
					*list_files = xmlStrcmp(content, BAD_CAST "true") == 0;

					// Free the node content
					xmlFree(content);
				}
			}

			// We've found the <ListDirectories> node
			else if (xmlStrcmp(node->name, BAD_CAST "ListDirectories") == 0)
			{
				// Get the content of the node
				xmlChar * content = xmlNodeGetContent(node);

				// We managed to get the content of the node
				if (content != NULL)
				{
					// Set the directory listing flag
					*list_directories = xmlStrcmp(content, BAD_CAST "true") == 0;

					// Free the node content
					xmlFree(content);
				}
			}
		}
	}
}

/**
 * @brief Checks whether the given path matches the given configuration document.
 */
int io_path_matches_configuration(const char * path, xmlDocPtr document)
{
	// The matching result
	int result = 1;

	// Whether the extension filter is enabled or not
	int extension_filter_enabled = 0;

	// Get the root element
	xmlNodePtr root = xmlDocGetRootElement(document);

	// We managed to get the root element
	if (root != NULL)
	{
		// Ensure the root element is a <Configuration> node
		if (xmlStrcmp(root->name, BAD_CAST "Configuration") == 0)
		{
			// Iterate all root node children
			for (xmlNodePtr root_child = root->children; root_child; root_child = root_child->next)
			{
				// We've found the <Extensions> node
				if (root_child->type == XML_ELEMENT_NODE && xmlStrcmp(root_child->name, BAD_CAST "Extensions") == 0)
				{
					// Cast the node to make the following code more readable
					xmlNodePtr extensions = root_child;

					// Iterate all extensions node children
					for (xmlNodePtr extensions_child = extensions->children; extensions_child; extensions_child = extensions_child->next)
					{
						// We've found a <Extension> node
						if (extensions_child->type == XML_ELEMENT_NODE && xmlStrcmp(extensions_child->name, BAD_CAST "Extension") == 0)
						{
							// Get the extension
							xmlChar * extension = xmlNodeGetContent(extensions_child);

							// We managed to get the extension
							if (extension != NULL)
							{
								// The extension filter was previously disabled
								if (!extension_filter_enabled)
								{
									// Enable the extension filter
									extension_filter_enabled = 1;

									// And reset the matching result
									result = 0;
								}

								// The given path matches the extension
								if (strlen(path) > strlen(extension) && strcmp(path + strlen(path) - strlen(extension), extension) == 0)
								{
									// Set the result
									result = 1;

									// One matching extension is enough to whitelist the path
									break;
								}

								// Free the extension
								xmlFree(extension);
							}
						}
					}

					// We only care about the <Extensions> node
					break;
				}
			}
		}
	}

	// Return the matching result
	return result;
}

/**
 * @brief Gets the given skin color from the given configuration document.
 */
Uint32 io_get_skin_color(xmlDocPtr document, const char * category_name, const char * type_name, Uint32 default_color)
{
	// The color
	Uint32 result = default_color;

	// Get the root element
	xmlNodePtr root = xmlDocGetRootElement(document);

	// We managed to get the root element
	if (root != NULL)
	{
		// Ensure the root element is a <Configuration> node
		if (xmlStrcmp(root->name, BAD_CAST "Configuration") == 0)
		{
			// Iterate all root node children
			for (xmlNodePtr root_child = root->children; root_child; root_child = root_child->next)
			{
				// We've found the <Skin> node
				if (root_child->type == XML_ELEMENT_NODE && xmlStrcmp(root_child->name, BAD_CAST "Skin") == 0)
				{
					// Cast the node to make the following code more readable
					xmlNodePtr skin = root_child;

					// Iterate all skin node children
					for (xmlNodePtr skin_child = skin->children; skin_child; skin_child = skin_child->next)
					{
						// We've found a category node
						if (skin_child->type == XML_ELEMENT_NODE && xmlStrcmp(skin_child->name, BAD_CAST category_name) == 0)
						{
							// Cast the node to make the following code more readable
							xmlNodePtr category = skin_child;

							// Iterate all category node children
							for (xmlNodePtr category_child = category->children; category_child; category_child = category_child->next)
							{
								// We've found the type node
								if (category_child->type == XML_ELEMENT_NODE && xmlStrcmp(category_child->name, BAD_CAST type_name) == 0)
								{
									// Cast the node to make the following code more readable
									xmlNodePtr type = category_child;

									// Iterate all type node children
									for (xmlNodePtr type_child = type->children; type_child; type_child = type_child->next)
									{
										// We've found the <Color> node
										if (type_child->type == XML_ELEMENT_NODE && xmlStrcmp(type_child->name, BAD_CAST "Color") == 0)
										{
											// Cast the node to make the following code more readable
											xmlNodePtr color = type_child;

											// Get the color value
											xmlChar * value = xmlNodeGetContent(color);

											// We managed to get the color value
											if (value != NULL)
											{
												// Parse the color using the hexadecimal number system
												result = (Uint32)strtol((const char *)value, NULL, 16);

												// Free the color value
												xmlFree(value);
											}

											// We only care about the <Color> node
											break;
										}
									}

									// We only care about the type node
									break;
								}
							}

							// We only care about the category node
							break;
						}
					}

					// We only care about the <Skin> node
					break;
				}
			}
		}
	}

	// Return the color
	return result;
}

/**
 * @brief Checks the given path for files that match the given configuration document's extension list.
 */
int io_does_folder_contain_games(const char * path, int include_files, int include_directories, xmlDocPtr document)
{
	// The result
	int contains_games = 0;

	// The items
	char ** items = NULL;

	// The item count
	int item_count = io_get_content_items(path, &items, include_files, include_directories, NULL);

	// We managed to get an item list
	if (items != NULL)
	{
		// Iterate all items
		for (int i = 0; i < item_count; i++)
		{
			// The item matches the configuration document's extension list
			if ((contains_games = io_path_matches_configuration(items[i], document)))
			{
				// One matching item is enough for this check
				break;
			}
		}

		// Free the item list
		io_free_item_list(item_count, &items);
	}

	// Return the result
	return contains_games;
}

/**
 * @brief Syncs all pending write operations to disk.
 */
void io_sync()
{
	// Do this a few times for good measure (some MicroSD cards are weird)
	for (int i = 0; i < 5; i++)
	{
		// Sync the pending write operations to disk
		(void)!system("sync");
	}
}

/**
 * @brief Shuts the device down.
 */
void io_shutdown()
{
	// Flush all pending write operations to disk
	io_sync();

	// Open the SysRq trigger file
	FILE * sysrq = fopen("/proc/sysrq-trigger", "w");

	// We managed to open the SysRq trigger file
	if (sysrq != NULL)
	{
		// Write the power off command to the node
		fputs("o", sysrq);

		// Close the SysRq trigger file
		fclose(sysrq);
	}
}

/**
 * @brief Copies the given file.
 */
void io_copy_file(const char * src_path, const char * dest_path)
{
	// Open the source file
	FILE * src_file = fopen(src_path, "rb");

	// We managed to open the source file
	if (src_file != NULL)
	{
		// Open the destination file
		FILE * dest_file = fopen(dest_path, "wb");

		// We managed to open the destination file
		if (dest_file != NULL)
		{
			// Allocate a buffer for file-copying
			char buffer[4096];

			// The number of bytes read into the chunk
			size_t bytes_read;

			// Keep reading chunks
			while ((bytes_read = fread(buffer, 1, sizeof(buffer), src_file)) > 0)
			{
				// And write them to the destination file
				fwrite(buffer, 1, bytes_read, dest_file);
			}

			// Close the destination file
			fclose(dest_file);
		}

		// Close the source file
		fclose(src_file);
	}
}

/**
 * @brief Copies the given directory (but only if the destination directory doesn't exist yet).
 */
void io_copy_directory(const char * src_dir, const char * dest_dir)
{
	// The destination directory doesn't exist yet
	if (access(dest_dir, F_OK) == -1)
	{
		// Create the destination directory
		if (mkdir(dest_dir, 0700) == 0)
		{
			// Open the source directory
			DIR * dir = opendir(src_dir);

			// We managed to open the source directory
			if (dir != NULL)
			{
				// Iterate all source directory entries
				for (struct dirent * entry = readdir(dir); entry != NULL; entry = readdir(dir))
				{
					// Filter out the current and parent directory entry
					if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
					{
						// Construct the source and destination file paths
						char src_path[PATH_MAX];
						char dest_path[PATH_MAX];
						snprintf(src_path, sizeof(src_path), "%s/%s", src_dir, entry->d_name);
						snprintf(dest_path, sizeof(dest_path), "%s/%s", dest_dir, entry->d_name);

						// The directory entry information
						struct stat entry_stat;

						// We managed to query the directory entry information
						if (lstat(src_path, &entry_stat) == 0)
						{
							// We're handling a subdirectory
							if (S_ISDIR(entry_stat.st_mode))
							{
								// Recursively copy the subdirectory
								io_copy_directory(src_path, dest_path);
							}

							// We're handling a file
							else
							{
								// Copy the file
								io_copy_file(src_path, dest_path);
							}
						}
					}
				}

				// Close the source directory
				closedir(dir);
			}
		}
	}
}

/**
 * @brief Creates the required RetroArch folder structure.
 */
static void io_create_retroarch_folder_structure()
{
	// NOTE: RetroArch resets its directory settings if it can't find these...
	mkdir("/media/RetroArch", 0700);
	mkdir("/media/RetroArch/saves", 0700);
	mkdir("/media/RetroArch/screenshots", 0700);
	mkdir("/media/RetroArch/states", 0700);
	mkdir("/media/RetroArch/system", 0700);
	mkdir("/media/RetroArch/thumbnails", 0700);
}

/**
 * @brief Unpacks a variety of needed resources.
 */
void io_unpack_resources()
{
	// Unpack the library template
	io_copy_directory(LIBRARY_FOLDER_TEMPLATE_PATH, LIBRARY_FOLDER_PATH);

	// Unpack the system icons template
	io_copy_directory(ICON_FOLDER_TEMPLATE_PATH, ICON_FOLDER_PATH);

	// Create the required RetroArch folder structure
	io_create_retroarch_folder_structure();
}
