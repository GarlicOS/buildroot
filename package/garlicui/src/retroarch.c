#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <cJSON.h>

#include "io.h"
#include "process.h"
#include "retroarch.h"

/**
 * @brief Starts the given game with the given core.
 */
int retroarch_run(struct gui_context * context, const char * core, const char * game)
{
	// Puzzle together the process arguments
	const char * args[] = { "retroarch", "-L", core, game, NULL };

	// We haven't selected a core
	if (core == NULL)
	{
		// Start retroarch's main menu instead
		args[1] = NULL;
	}

	// Start the retroarch process
	return process_run(context, args, 1);
}

/**
 * @brief Gets the given game's auto savestate path.
 *
 * The returned buffer must be freed by the caller.
 */
char * retroarch_get_auto_savestate_path(const char * game)
{
	// The savestate path
	char * savestate_path = NULL;

	// Get the savestate screenshot path
	char * savestate_screenshot_path = retroarch_get_auto_savestate_screenshot_path(game);

	// We managed to get the savestate screenshot path
	if (savestate_screenshot_path != NULL)
	{
		// Remove the .png extension from the screenshot path (which gives us the savestate path)
		savestate_path = io_remove_extension(savestate_screenshot_path);

		// Free the temporary savestate screenshot path
		free(savestate_screenshot_path);
	}

	// Return the savestate path
	return savestate_path;
}

/**
 * @brief Gets the given game's auto savestate screenshot path.
 *
 * The returned buffer must be freed by the caller.
 */
char * retroarch_get_auto_savestate_screenshot_path(const char * game)
{
	// The given input path is outside of our managed library directory
	if (strncmp(game, LIBRARY_FOLDER_PATH, strlen(LIBRARY_FOLDER_PATH)) != 0)
	{
		// There's no reliable way for us to determine the auto savestate screenshot path
		return NULL;
	}

	// Skip the library directory root path component
	const char * library_relative_game_path = game + strlen(LIBRARY_FOLDER_PATH);

	// The given input path is outside of our managed library directory
	if (*library_relative_game_path != '/')
	{
		// There's no reliable way for us to determine the auto savestate screenshot path
		return NULL;
	}

	// Skip the separator
	library_relative_game_path++;

	// Allocate a buffer to hold the auto savestate screenshot path
	char * auto_savestate_screenshot_path = calloc(1, strlen(RETROARCH_DEFAULT_AUTO_SAVESTATE_SCREENSHOT_FILE_PATH) - 1 + strlen(library_relative_game_path));

	// We managed to allocate a buffer to hold the auto savestate screenshot path
	if (auto_savestate_screenshot_path != NULL)
	{
		// The game file has an extension
		if (strrchr(io_get_file_name(library_relative_game_path), '.') != NULL)
		{
			// Remove the extension
			char * library_relative_game_path_sans_extension = io_remove_extension(library_relative_game_path);

			// We managed to remove the extension
			if (library_relative_game_path_sans_extension != NULL)
			{
				// Puzzle together the auto savestate screenshot path
				sprintf(auto_savestate_screenshot_path, RETROARCH_DEFAULT_AUTO_SAVESTATE_SCREENSHOT_FILE_PATH, library_relative_game_path_sans_extension);

				// Free the temporary buffer
				free(library_relative_game_path_sans_extension);
			}
		}

		// The game file has no extension
		else
		{
			// Puzzle together the auto savestate screenshot path
			sprintf(auto_savestate_screenshot_path, RETROARCH_DEFAULT_AUTO_SAVESTATE_SCREENSHOT_FILE_PATH, library_relative_game_path);
		}

		// We failed to remove the file extension
		if (auto_savestate_screenshot_path[0] == 0)
		{
			// Free the path buffer
			free(auto_savestate_screenshot_path);

			// And NULL its reference
			auto_savestate_screenshot_path = NULL;
		}
	}

	// Return the auto savestate screenshot path
	return auto_savestate_screenshot_path;
}

/**
 * @brief Gets the given game's retroarch thumbnail.
 *
 * The returned buffer must be freed by the caller.
 */
char * retroarch_get_thumbnail_path(const char * game, const char * thumbnail_type)
{
	// The given input path is outside of our managed library directory
	if (strncmp(game, LIBRARY_FOLDER_PATH, strlen(LIBRARY_FOLDER_PATH)) != 0)
	{
		// There's no reliable way for us to determine the thumbnail path
		return NULL;
	}

	// Skip the library directory root path component
	const char * library_relative_game_path = game + strlen(LIBRARY_FOLDER_PATH);

	// The given input path is outside of our managed library directory
	if (*library_relative_game_path != '/')
	{
		// There's no reliable way for us to determine the thumbnail path
		return NULL;
	}

	// Skip the separator
	library_relative_game_path++;

	// Split off the system directory
	char * system_directory = io_get_directory_name(library_relative_game_path);

	// We failed to split off the system directory
	if (system_directory == NULL)
	{
		// There's no reliable way for us to determine the thumbnail path
		return NULL;
	}

	// Split off the file name
	const char * game_file = io_get_file_name(library_relative_game_path);

	// Allocate a buffer to hold the thumbnail path
	char * thumbnail_path = calloc(1, strlen(RETROARCH_DEFAULT_THUMBNAIL_FILE_PATH) + strlen(system_directory) + strlen(thumbnail_type) + strlen(game_file));

	// We managed to allocate a buffer to hold the thumbnail path
	if (thumbnail_path != NULL)
	{
		// The game file has an extension
		if (strrchr(game_file, '.') != NULL)
		{
			// Remove the extension
			char * game_file_sans_extension = io_remove_extension(game_file);

			// We managed to remove the extension
			if (game_file_sans_extension != NULL)
			{
				// Puzzle together the thumbnail path
				sprintf(thumbnail_path, RETROARCH_DEFAULT_THUMBNAIL_FILE_PATH, system_directory, thumbnail_type, game_file_sans_extension);

				// Free the temporary buffer
				free(game_file_sans_extension);
			}
		}

		// The game file has no extension
		else
		{
			// Puzzle together the thumbnail path
			sprintf(thumbnail_path, RETROARCH_DEFAULT_THUMBNAIL_FILE_PATH, system_directory, thumbnail_type, game_file);
		}

		// We failed to remove the file extension
		if (thumbnail_path[0] == 0)
		{
			// Free the path buffer
			free(thumbnail_path);

			// And NULL its reference
			thumbnail_path = NULL;
		}
	}

	// Free the system directory buffer
	free(system_directory);

	// Return the thumbnail path
	return thumbnail_path;
}

/**
 * @brief Returns the first item from the given playlist that matches the given path.
 *
 * The caller must explicitly free the returned item by calling retroarch_free_playlist_item.
 */
struct retroarch_playlist_item * retroarch_get_playlist_item(const char * playlist, const char * path)
{
	// The playlist item
	struct retroarch_playlist_item * playlist_item = NULL;

	// Open the playlist file
	FILE * file = fopen(playlist, "r");

	// We managed to open the playlist file
	if (file != NULL)
	{
		// Determine the playlist file size
		fseek(file, 0, SEEK_END);
		long file_size = ftell(file);
		rewind(file);

		// Allocate a buffer for the playlist file content
		char * file_buffer = (char *)malloc(file_size + 1);

		// We managed to allocate a buffer for the playlist file content
		if (file_buffer != NULL)
		{
			// Read the playlist file content
			if (fread(file_buffer, 1, file_size, file) == file_size)
			{
				// Terminate the playlist file content string
				file_buffer[file_size] = 0;

				// Parse the JSON data
				cJSON * json = cJSON_Parse(file_buffer);

				// We managed to parse the JSON data
				if (json != NULL)
				{
					// Get the items object
					cJSON * items = cJSON_GetObjectItem(json, "items");

					// Make sure the items object is an array
					if (cJSON_IsArray(items))
					{
						// Get the array size
						int array_size = cJSON_GetArraySize(items);

						// Iterate all items
						for (int i = 0; i < array_size; i++)
						{
							// Get the array item
							cJSON * item = cJSON_GetArrayItem(items, i);

							// Get the JSON path object item
							cJSON * json_path = cJSON_GetObjectItem(item, "path");

							// We managed to get the JSON path object item
							if (json_path != NULL)
							{
								// The path matches what we expect
								if (path == NULL || strcmp(json_path->valuestring, path) == 0)
								{
									// Allocate memory for the item
									playlist_item = (struct retroarch_playlist_item *)calloc(1, sizeof(struct retroarch_playlist_item));

									// We managed to allocate the memory for the item
									if (playlist_item != NULL)
									{
										// Duplicate the path string value
										playlist_item->path = strdup(json_path->valuestring);

										// Get the other JSON object items
										cJSON * json_label = cJSON_GetObjectItem(item, "label");
										cJSON * json_core_path = cJSON_GetObjectItem(item, "core_path");
										cJSON * json_core_name = cJSON_GetObjectItem(item, "core_name");
										cJSON * json_crc32 = cJSON_GetObjectItem(item, "crc32");
										cJSON * json_db_name = cJSON_GetObjectItem(item, "db_name");

										// We managed to get the game label
										if (json_label != NULL)
										{
											// Duplicate the string value
											playlist_item->label = strdup(json_label->valuestring);
										}

										// We managed to get the core path
										if (json_core_path != NULL)
										{
											// Duplicate the string value
											playlist_item->core_path = strdup(json_core_path->valuestring);
										}

										// We managed to get the core name
										if (json_core_name != NULL)
										{
											// Duplicate the string value
											playlist_item->core_name = strdup(json_core_name->valuestring);
										}

										// We managed to get the checksum
										if (json_crc32 != NULL)
										{
											// Duplicate the string value
											playlist_item->crc32 = strdup(json_crc32->valuestring);
										}

										// We managed to get the database name
										if (json_db_name != NULL)
										{
											// Duplicate the string value
											playlist_item->db_name = strdup(json_db_name->valuestring);
										}
									}

									// Stop searching
									break;
								}
							}
						}
					}

					// Delete the JSON object
					cJSON_Delete(json);
				}
			}

			// Free the playlist file content buffer
			free(file_buffer);
		}

		// Close the playlist file
		fclose(file);
	}

	// Return the playlist item
	return playlist_item;
}

/**
 * @brief Free the given playlist item.
 */
void retroarch_free_playlist_item(struct retroarch_playlist_item * item)
{
	// We've been given a seemingly valid item
	if (item != NULL)
	{
		// The item contains a path
		if (item->path != NULL)
		{
			// Free the path
			free(item->path);
		}

		// The item contains a label
		if (item->label != NULL)
		{
			// Free the label
			free(item->label);
		}

		// The item contains a core path
		if (item->core_path != NULL)
		{
			// Free the core path
			free(item->core_path);
		}

		// The item contains a core name
		if (item->core_name != NULL)
		{
			// Free the core name
			free(item->core_name);
		}

		// The item contains a checksum
		if (item->crc32 != NULL)
		{
			// Free the checksum
			free(item->crc32);
		}

		// The item contains a database name
		if (item->db_name != NULL)
		{
			// Free the database name
			free(item->db_name);
		}

		// Free the item itself
		free(item);
	}
}

/**
 * @brief Returns all items contained in the given playlist.
 */
int retroarch_get_playlist_items(const char * playlist, struct retroarch_playlist_item *** playlist_items)
{
	// The playlist item count
	int playlist_item_count = 0;

	// We've been given a valid playlist items output pointer
	if (playlist_items != NULL)
	{
		// NULL the output variable
		*playlist_items = NULL;

		// Open the playlist file
		FILE * file = fopen(playlist, "r");

		// We managed to open the playlist file
		if (file != NULL)
		{
			// Determine the playlist file size
			fseek(file, 0, SEEK_END);
			long file_size = ftell(file);
			rewind(file);

			// Allocate a buffer for the playlist file content
			char * file_buffer = (char *)malloc(file_size + 1);

			// We managed to allocate a buffer for the playlist file content
			if (file_buffer != NULL)
			{
				// Read the playlist file content
				if (fread(file_buffer, 1, file_size, file) == file_size)
				{
					// Terminate the playlist file content string
					file_buffer[file_size] = 0;

					// Parse the JSON data
					cJSON * json = cJSON_Parse(file_buffer);

					// We managed to parse the JSON data
					if (json != NULL)
					{
						// Get the items object
						cJSON * items = cJSON_GetObjectItem(json, "items");

						// Make sure the items object is an array
						if (cJSON_IsArray(items))
						{
							// Get the array size
							int array_size = cJSON_GetArraySize(items);

							// Iterate all items
							for (int i = 0; i < array_size; i++)
							{
								// Get the array item
								cJSON * item = cJSON_GetArrayItem(items, i);

								// Allocate memory for the item
								struct retroarch_playlist_item * playlist_item = (struct retroarch_playlist_item *)calloc(1, sizeof(struct retroarch_playlist_item));

								// We managed to allocate memory for the item
								if (playlist_item != NULL)
								{
									// Get the JSON values
									cJSON * path = cJSON_GetObjectItem(item, "path");
									cJSON * label = cJSON_GetObjectItem(item, "label");
									cJSON * core_path = cJSON_GetObjectItem(item, "core_path");
									cJSON * core_name = cJSON_GetObjectItem(item, "core_name");
									cJSON * crc32 = cJSON_GetObjectItem(item, "crc32");
									cJSON * db_name = cJSON_GetObjectItem(item, "db_name");

									// We managed to get the game path
									if (path != NULL)
									{
										// Duplicate the string value
										playlist_item->path = strdup(path->valuestring);
									}

									// We managed to get the game label
									if (label != NULL)
									{
										// Duplicate the string value
										playlist_item->label = strdup(label->valuestring);
									}

									// We managed to get the core path
									if (core_path != NULL)
									{
										// Duplicate the string value
										playlist_item->core_path = strdup(core_path->valuestring);
									}

									// We managed to get the core name
									if (core_name != NULL)
									{
										// Duplicate the string value
										playlist_item->core_name = strdup(core_name->valuestring);
									}

									// We managed to get the checksum
									if (crc32 != NULL)
									{
										// Duplicate the string value
										playlist_item->crc32 = strdup(crc32->valuestring);
									}

									// We managed to get the database name
									if (db_name != NULL)
									{
										// Duplicate the string value
										playlist_item->db_name = strdup(db_name->valuestring);
									}

									// Resize the playlist items list to accomodate one more item
									void * resized_playlist_items = realloc(*playlist_items, sizeof(struct retroarch_playlist_item *) * (playlist_item_count + 1));

									// We managed to resize the playlist items list
									if (resized_playlist_items)
									{
										// Override the old pointer (some libc implementations can relocate the buffer)
										*playlist_items = resized_playlist_items;
										
										// Add the populated playlist item to the list
										(*playlist_items)[playlist_item_count++] = playlist_item;
									}

									// We failed to resize the playlist items list
									else
									{
										// Free the playlist item
										retroarch_free_playlist_item(playlist_item);
									}
								}
							}
						}

						// Delete the JSON object
						cJSON_Delete(json);
					}
				}

				// Free the playlist file content buffer
				free(file_buffer);
			}

			// Close the playlist file
			fclose(file);
		}
	}

	// Return the playlist item count
	return playlist_item_count;
}

/**
 * @brief Frees the given playlist items.
 */
void retroarch_free_playlist_items(struct retroarch_playlist_item ** items, int count)
{
	// We've been given valid parameters
	if (items != NULL && count != 0)
	{
		// Iterate all items
		for (int i = 0; i < count; i++)
		{
			// Free the item
			retroarch_free_playlist_item(items[i]);
		}

		// Free the list itself
		free(items);
	}
}

/**
 * @brief Removes all items that match the given path from the given playlist.
 */
void retroarch_remove_playlist_items_by_path(const char * playlist, const char * path)
{
	// We've been given valid parameters
	if (playlist != NULL && path != NULL)
	{
		// Open the playlist file
		FILE * file = fopen(playlist, "r");

		// We managed to open the playlist file
		if (file != NULL)
		{
			// Determine the playlist file size
			fseek(file, 0, SEEK_END);
			long file_size = ftell(file);
			rewind(file);

			// Allocate a buffer for the playlist file content
			char * file_buffer = (char *)malloc(file_size + 1);

			// We managed to allocate a buffer for the playlist file content
			if (file_buffer != NULL)
			{
				// Read the playlist file content
				if (fread(file_buffer, 1, file_size, file) == file_size)
				{
					// Close the playlist file
					fclose(file);

					// NULL the playlist file pointer
					file = NULL;

					// Parse the JSON data
					cJSON * root = cJSON_Parse(file_buffer);

					// We managed the parse the JSON data
					if (root != NULL)
					{
						// Get the "items" array
						cJSON * items_array = cJSON_GetObjectItem(root, "items");

						// We managed to get the "items" array
						if (items_array != NULL && cJSON_IsArray(items_array))
						{
							// The current array index
							int i = 0;

							// Iterate all items
							while (i < cJSON_GetArraySize(items_array))
							{
								// Get the item
								cJSON * item = cJSON_GetArrayItem(items_array, i);

								// And its path
								cJSON * path_item = cJSON_GetObjectItem(item, "path");

								// The item's path matches the one we wish to remove
								if (path_item != NULL && cJSON_IsString(path_item) && strcmp(path_item->valuestring, path) == 0)
								{
									// Delete the item from the array
									cJSON_DeleteItemFromArray(items_array, i);
								}

								// The item's path doesn't match the given path
								else
								{
									// Skip the item
									i++;
								}
							}

							// Update the JSON structure
							char * updated_json_data = cJSON_PrintUnformatted(root);

							// We managed to update the JSON structure
							if (updated_json_data != NULL)
							{
								// Open the playlist for writing
								file = fopen(playlist, "w");

								// We managed to open the playlist for writing
								if (file != NULL)
								{
									// Write the updated JSON structure back to the playlist file
									fwrite(updated_json_data, 1, strlen(updated_json_data), file);

									// Close the playlist file
									fclose(file);

									// NULL the playlist file pointer
									file = NULL;
								}

								// Free the updated JSON structure
								free(updated_json_data);
							}
						}

						// Free the JSON data
						cJSON_Delete(root);
					}
				}

				// Free the file buffer
				free(file_buffer);
			}

			// We have an open playlist file
			if (file != NULL)
			{
				// Close the playlist file
				fclose(file);
			}
		}
	}
}

/**
 * @brief Adds the given item to the given playlist.
 */
void retroarch_add_playlist_item(const char * playlist, struct retroarch_playlist_item * item)
{
	// We've been given valid parameters
	if (playlist != NULL && item != NULL)
	{
		// Open the playlist file
		FILE * file = fopen(playlist, "r");

		// We managed to open the playlist file
		if (file != NULL)
		{
			// Determine the playlist file size
			fseek(file, 0, SEEK_END);
			long file_size = ftell(file);
			rewind(file);

			// Allocate a buffer for the playlist file content
			char * file_buffer = (char *)malloc(file_size + 1);

			// We managed to allocate a buffer for the playlist file content
			if (file_buffer != NULL)
			{
				// Read the playlist file content
				if (fread(file_buffer, 1, file_size, file) == file_size)
				{
					// Close the playlist file
					fclose(file);

					// NULL the playlist file pointer
					file = NULL;

					// Parse the JSON data
					cJSON * root = cJSON_Parse(file_buffer);

					// We managed the parse the JSON data
					if (root != NULL)
					{
						// Create a new item object and populate it
						cJSON* new_item = cJSON_CreateObject();
						cJSON_AddItemToObject(new_item, "path", cJSON_CreateString(item->path));
						cJSON_AddItemToObject(new_item, "label", cJSON_CreateString(item->label));
						cJSON_AddItemToObject(new_item, "core_path", cJSON_CreateString(item->core_path));
						cJSON_AddItemToObject(new_item, "core_name", cJSON_CreateString(item->core_name));
						cJSON_AddItemToObject(new_item, "crc32", cJSON_CreateString(item->crc32));
						cJSON_AddItemToObject(new_item, "db_name", cJSON_CreateString(item->db_name));

						// Get the "items" array
						cJSON * items_array = cJSON_GetObjectItem(root, "items");

						// We couldn't find an "items" array
						if (items_array == NULL || !cJSON_IsArray(items_array))
						{
							// Create a new "items" array
							items_array = cJSON_AddArrayToObject(root, "items");
						}

						// Add the new item to the array
						cJSON_AddItemToArray(items_array, new_item);

						// Update the JSON structure
						char * updated_json_data = cJSON_PrintUnformatted(root);

						// We managed to update the JSON structure
						if (updated_json_data != NULL)
						{
							// Open the playlist for writing
							file = fopen(playlist, "w");

							// We managed to open the playlist for writing
							if (file != NULL)
							{
								// Write the updated JSON structure back to the playlist file
								fwrite(updated_json_data, 1, strlen(updated_json_data), file);

								// Close the playlist file
								fclose(file);

								// NULL the playlist file pointer
								file = NULL;
							}

							// Free the updated JSON structure
							free(updated_json_data);
						}

						// Free the JSON data
						cJSON_Delete(root);
					}
				}

				// Free the file buffer
				free(file_buffer);
			}

			// We have an open playlist file
			if (file != NULL)
			{
				// Close the playlist file
				fclose(file);
			}
		}
	}
}

/**
 * @brief Attempts to resume the last played game.
 */
void retroarch_resume_last_game(struct gui_context * context)
{
	// Get the last played game
	struct retroarch_playlist_item * last_game = retroarch_get_playlist_item(RETROARCH_DEFAULT_HISTORY_PLAYLIST_FILE_PATH, NULL);

	// We managed to get the last played game
	if (last_game != NULL)
	{
		// Resume the game
		retroarch_run(context, last_game->core_path, last_game->path);

		// Free the playlist item
		retroarch_free_playlist_item(last_game);
	}
}