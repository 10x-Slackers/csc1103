#include "gui.h"

#include <glib/gstdio.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <time.h>

#include "board_gui.h"
#include "game.h"
#include "main_menu.h"
#include "option_menu.h"

// Global Naive Bayes model
static NaiveBayesModel nb_model;

/**
 * @brief Navigate to a specific page in the stack.
 * @param stack Pointer to the GtkStack.
 * @param page_name Name of the page to navigate to.
 */
void navigate_to(GtkStack* stack, const char* page_name) {
  gtk_stack_set_visible_child_name(stack, page_name);
}

/**
 * @brief Loads and applies CSS styling from the specified file.
 *
 * Creates a CSS provider and loads CSS styles from the given file path,
 * then applies them to the default display.
 *
 * @param css_path Path to the CSS file.
 */
static void load_css(const char* css_path) {
  GtkCssProvider* provider = gtk_css_provider_new();
  gtk_css_provider_load_from_path(provider, css_path);
  gtk_style_context_add_provider_for_display(
      gdk_display_get_default(), GTK_STYLE_PROVIDER(provider),
      GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  g_object_unref(provider);
}

/**
 * @brief Callback function triggered when the GTK application is activated.
 *
 * Loads the UI from the builder file, initializes the game state, sets up all
 * UI components (main menu, player selection, difficulty selection, game board,
 * and win dialog), applies CSS styling, and displays the main window.
 *
 * @param app Pointer to the GtkApplication instance.
 * @param user_data User data pointer (unused).
 */
static void activate(GtkApplication* app, gpointer user_data G_GNUC_UNUSED) {
  // Load the UI from the builder file
  GtkBuilder* builder = gtk_builder_new();
  gtk_builder_add_from_file(builder, BUILDER_PATH, NULL);

  GtkStack* stack = GTK_STACK(gtk_builder_get_object(builder, "main_stack"));

  // Load CSS
  load_css(CSS_PATH);

  // Initialize game state with the Naive Bayes model
  init_game_state(builder, &nb_model);

  // Initialize all UI components
  main_menu(builder, stack);
  player_select(builder, stack);
  difficulty_select(builder, stack);
  game_board(builder, stack);
  win_dialog(builder);

  // Set the stack to show the main menu
  gtk_stack_set_visible_child_name(stack, "main_menu");

  // Show the main window
  GtkWidget* window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
  gtk_window_set_application(GTK_WINDOW(window), app);
  gtk_widget_set_visible(window, TRUE);
}

/**
 * @brief Main entry point of the application.
 *
 * Seeds the random number generator, loads the Naive Bayes model,
 * creates and runs the GTK application.
 *
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return EXIT_SUCCESS on successful run, EXIT_FAILURE on error.
 */
int main(int argc, char* argv[]) {
  // Seed RNG
  srand(time(NULL));

  // Load Naive Bayes model
  int res = load_nb_model(&nb_model, "src/ml/naive_bayes.bin");
  if (res != 0) {
    fprintf(stderr, "Failed to load Naive Bayes model.\n");
    return EXIT_FAILURE;
  }

  // Create and run the GTK application
  GtkApplication* app =
      GTK_APPLICATION(gtk_application_new(APP_ID, G_APPLICATION_DEFAULT_FLAGS));
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  int status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  return status;
}
