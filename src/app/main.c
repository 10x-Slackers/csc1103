#include <gtk/gtk.h>
#include <stdlib.h>
#include <time.h>

#include "board_gui.h"
#include "game.h"
#include "main_menu.h"
#include "option_menu.h"
#include "utils.h"
#define APP_ID "com.csc1103.tictactoe"
#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 900
#define CSS_RESOURCE "/org/csc1103/tictactoe/style.css"
#define BUILDER_RESOURCE "/org/csc1103/tictactoe/builder.ui"

/**
 * @brief Callback function triggered when the GTK application is activated.
 *
 * Loads the UI from the builder file, applies CSS styling, initializes the game
 * state, sets up all UI components, and displays the main window.
 *
 * @param app Pointer to the GtkApplication instance.
 * @param user_data User data pointer (unused).
 */
static void activate(GtkApplication* app, gpointer user_data G_GNUC_UNUSED) {
  // Load the UI from the builder resource
  GtkBuilder* builder = get_builder(BUILDER_RESOURCE);
  if (!builder) {
    g_printerr("Failed to load UI resource.\n");
    return;
  }

  // Load CSS from resource
  load_css(CSS_RESOURCE);

  // Load the Naive Bayes model
  NaiveBayesModel nb_model;
  if (load_nb_model(&nb_model, "src/ml/naive_bayes.bin") != 0) {
    g_printerr(
        "Error: Failed to load Naive Bayes model from "
        "'src/ml/naive_bayes.bin'\n");
    g_object_unref(builder);
    return;
  }

  // Initialize game state with the builder and Naive Bayes model
  if (init_game_state(builder, &nb_model) != 0) {
    g_printerr("Failed to initialize game state.\n");
  }

  // Get the main stack
  GtkStack* stack = GTK_STACK(gtk_builder_get_object(builder, "main_stack"));
  if (!stack) {
    g_printerr("Failed to find main_stack in UI file.\n");
    g_object_unref(builder);
    return;
  }

  // Initialize all UI components
  main_menu(builder, stack);
  player_select(builder, stack);
  difficulty_select(builder, stack);
  game_board(builder, stack);
  win_dialog(builder);

  // Set the stack to show the main menu
  gtk_stack_set_visible_child_name(stack, "main_menu");

  // Show the window
  GtkWidget* window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
  if (!window) {
    g_printerr("Failed to find main window in UI file.\n");
    g_object_unref(builder);
    return;
  }
  gtk_window_set_application(GTK_WINDOW(window), app);
  gtk_widget_set_visible(window, TRUE);
}

/**
 * @brief Main entry point of the application.
 *
 * Seeds the random number generator, loads the Naive Bayes model, then
 * creates and runs the GTK application.
 *
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return EXIT_SUCCESS on successful run, EXIT_FAILURE on error.
 */
int main(int argc, char* argv[]) {
  // Seed RNG
  srand((unsigned int)time(NULL));

  // Create and run the GTK application
  GtkApplication* app =
      GTK_APPLICATION(gtk_application_new(APP_ID, G_APPLICATION_DEFAULT_FLAGS));
  if (!app) {
    fprintf(stderr, "Error: Failed to create GTK application\n");
    return EXIT_FAILURE;
  }
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  int status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  return status;
}
