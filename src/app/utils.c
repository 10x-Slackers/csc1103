/**
 * @file utils.c
 * @brief Shared utility functions for the application.
 * @authors commit2main
 * @date 2025-11-13
 */
#include "utils.h"

/**
 * @brief Loads and applies CSS styling from the specified resource.
 * @param css_resource Resource path to the CSS file.
 */
void load_css(const char* css_resource) {
  GtkCssProvider* provider = gtk_css_provider_new();
  gtk_css_provider_load_from_resource(provider, css_resource);
  gtk_style_context_add_provider_for_display(
      gdk_display_get_default(), GTK_STYLE_PROVIDER(provider),
      GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  g_object_unref(provider);
}

/**
 * @brief Loads a GtkBuilder from the specified resource path.
 * @param resource_path Resource path to the UI file.
 * @return Pointer to the loaded GtkBuilder, or NULL on failure.
 */
GtkBuilder* get_builder(const char* resource_path) {
  GtkBuilder* builder = gtk_builder_new();
  GError* error = NULL;
  if (!gtk_builder_add_from_resource(builder, resource_path, &error)) {
    g_printerr("Error loading UI resource: %s\n", error->message);
    if (error) g_error_free(error);
    g_object_unref(builder);
    return NULL;
  }
  return builder;
}
