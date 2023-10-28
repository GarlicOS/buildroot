#include "icon.h"
#include "localization.h"

/**
 * @brief Gets the font file path for the given locale.
 */
const char * localization_font_file_path(const char * locale)
{
	// The static font file path buffer
	static char font_file_path[PATH_MAX];

	// The current locale
	char current_locale[16];

	// Get the current locale
	strcpy(current_locale, setlocale(LC_ALL, NULL));

	// We need to change the locale
	if (locale != NULL && strcmp(current_locale, locale) != 0)
	{
		// Change the locale
		localization_change_locale(locale);
	}

	// The gettext msgid of the font file path
	const char * font_file_path_msgid = "MSGID_font_file_path";

	// Get the locale's preferred font
	const char * localized_font_file_path = gettext(font_file_path_msgid);

	// The locale has no preferred font
	if (strcmp(localized_font_file_path, font_file_path_msgid) == 0)
	{
		// Fall back to a semi-safe latin default font (better than nothing)
		localized_font_file_path = "/usr/share/fonts/oswald/Oswald-Regular.ttf";
	}

	// Copy the localized font file path into the static buffer
	strcpy(font_file_path, localized_font_file_path);

	// We need to change the locale
	if (locale != NULL && strcmp(current_locale, locale) != 0)
	{
		// Change the locale
		localization_change_locale(current_locale);
	}

	// Return the preferred font
	return font_file_path;
}

/**
 * @brief Gets the font height for the current locale.
 */
int localization_font_height()
{
	// TODO: Do we even need localization specific font heights?
	return DEFAULT_BAR_ICON_HEIGHT;
}

/**
 * @brief Changes the application's locale.
 */
void localization_change_locale(const char * locale)
{
	// Set the language environment variable
	setenv("LANGUAGE", locale, 1);
	setenv("LC_ALL", locale, 1);
	setenv("LC_MESSAGES", locale, 1);
	setenv("LANG", locale, 1);

	// Set the desired locale
	setlocale(LC_ALL, locale);
	setlocale(LC_MESSAGES, locale);

	// Set the text domain for translations
	textdomain("garlicui");

	// Set the directory where translation files are located
	bindtextdomain("garlicui", LOCALIZATION_DIRECTORY);
}

/**
 * @brief Gets the given locale's language display name.
 *
 * The returned language string needs to be freed by the caller explicitly.
 */
char * localization_get_language_display_name(const char * locale, const char * display_locale)
{
	// The display name UTF-8 duplicate
	char * display_name_utf8_duplicate = NULL;

	// The last returned error code
	UErrorCode error = U_ZERO_ERROR;

	// The temporary UTF-16 language display name buffer
	UChar display_name[256];

	// The temporary UTF-8 language display name buffer
	char display_name_utf8[256];

	// Get the display name of the language from the locale
	int32_t display_name_length = uloc_getDisplayLanguage(locale, display_locale, display_name, sizeof(display_name), &error);

	// We managed to get the display name of the language from the locale
	if (U_SUCCESS(error))
	{
		// Convert the UTF-16 display name to UTF-8
		u_strToUTF8(display_name_utf8, sizeof(display_name_utf8), NULL, display_name, display_name_length, &error);

		// We managed to convert the UTF-16 display name to UTF-8
		if (U_SUCCESS(error))
		{
			// Duplicate the UTF-8 display name
			display_name_utf8_duplicate = strdup(display_name_utf8);
		}
	}

	// Return the display name UTF-8 duplicate
	return display_name_utf8_duplicate;
}

/**
 * @brief Returns a list of supported locales.
 *
 * The returned list must be explicitly destroyed with io_free_item_list.
 */
int localization_get_supported_locales(char *** locales)
{
	// The number of supported locales
	size_t num_locales = 0;

	// We've been given a seemingly valid output pointer
	if (locales != NULL)
	{
		// The start needle
		const char * start_needle = "/locale/";

		// The end needle
		const char * end_needle = "/LC_MESSAGES";

		// The wildcard expansion result
		glob_t glob_result;

		// Expand the wildcard into valid paths
		int status = glob("/usr/share/locale/*/LC_MESSAGES/garlicui.mo", 0, NULL, &glob_result);

		// We managed to expand the wildcard
		if (status == 0 && glob_result.gl_pathc > 0)
		{
			// Allocate an array buffer
			*locales = (char **)malloc(glob_result.gl_pathc * sizeof(char *));

			// We managed to allocate an array buffer
			if (*locales != NULL)
			{
				// Iterate all expanded paths
				for (size_t i = 0; i < glob_result.gl_pathc; i++)
				{
					// Determine the locale position and length
					char * locale_start = strstr(glob_result.gl_pathv[i], start_needle) + strlen(start_needle);
					char * locale_end = strstr(locale_start, end_needle);
					size_t locale_length = locale_end - locale_start;

					// Allocate a buffer for the locale
					(*locales)[num_locales] = (char *)malloc(locale_length + 1);

					// We managed to allocate a buffer for the locale
					if ((*locales)[num_locales] != NULL)
					{
						// Copy the locale into the buffer
						strncpy((*locales)[num_locales], locale_start, locale_length);

						// Terminate the buffer
						(*locales)[num_locales][locale_length] = 0;

						// And increment the locale counter
						num_locales++;
					}
				}

				// We ran out of memory
				if (num_locales == 0)
				{
					// Free the array buffer
					free(*locales);

					// And return a NULL pointer
					*locales = NULL;
				}
			}
		}

		// We failed to expand the wildcard
		else
		{
			// Return a NULL pointer
			*locales = NULL;
		}

		// Free the glob result
		globfree(&glob_result);
	}

	// Return the number of supported locales
	return num_locales;
}