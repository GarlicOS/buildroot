#ifndef LOCALIZATION_H
#define LOCALIZATION_H

#include <glob.h>
#include <locale.h>
#include <libintl.h>
#include <unicode/uloc.h>
#include <unicode/ulocdata.h>

/**
 * @brief The localization directory.
 */
#define LOCALIZATION_DIRECTORY "/usr/share/locale"

/**
 * @brief Gets the font file path for the given locale.
 */
const char * localization_font_file_path(const char * locale);

/**
 * @brief Gets the font height for the current locale.
 */
int localization_font_height();

/**
 * @brief Changes the application's locale.
 */
void localization_change_locale(const char * locale);

/**
 * @brief Gets the given locale's language display name.
 *
 * The returned language string needs to be freed by the caller explicitly.
 */
char * localization_get_language_display_name(const char * locale, const char * display_locale);

/**
 * @brief Returns a list of supported locales.
 *
 * The returned list must be explicitly destroyed with io_free_item_list.
 */
int localization_get_supported_locales(char ***locales);

#endif