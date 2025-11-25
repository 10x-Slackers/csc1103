#ifndef UTILS_H
#define UTILS_H

#include <gtk/gtk.h>

/**
 * @brief Loads and applies CSS styling from the specified resource.
 * @param css_resource Resource path to the CSS file.
 */
void load_css(const char* css_resource);

/**
 * @brief Loads icons from the specified resource path into the icon theme.
 * @param resource_path Resource path to the icons.
 */
void load_icons(const char* resource_path);

/**
 * @brief Loads a GtkBuilder from the specified resource path.
 * @param resource_path Resource path to the UI file.
 * @return Pointer to the loaded GtkBuilder, or NULL on failure.
 */
GtkBuilder* get_builder(const char* resource_path);

#endif  // UTILS_H
