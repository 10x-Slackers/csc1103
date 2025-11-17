#include <gtk/gtk.h>
#include <stdlib.h>
#include <time.h>

#include "audio.h"
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
#define DEFAULT_MODEL_PATH "src/ml/naive_bayes.bin"

/* Global Naive Bayes model */
static NaiveBayesModel nb_model;

/**
 * @brief Callback function triggered when the GTK application is activated.
 *
 * Loads the UI from the builder file, applies CSS styling, initialises the game
 * state, sets up all UI components, and displays the main window.
 *
 * @param app Pointer to the GtkApplication instance.
 * @param user_data User data pointer (unused).
 */
static void gui_activate(GtkApplication* app,
                         gpointer user_data G_GNUC_UNUSED) {
  if (init_audio() != 0) {
    g_printerr("Warning: Failed to initialise audio system.\n");
    return;
  }

  // Load the UI from the builder resource
  GtkBuilder* builder = get_builder(BUILDER_RESOURCE);
  if (!builder) {
    g_printerr("Failed to load UI resource.\n");
    return;
  }

  // Load CSS from resource
  load_css(CSS_RESOURCE);

  // Initialise game state with the builder and Naive Bayes model
  if (init_game_state(builder, &nb_model) != 0) {
    g_printerr("Failed to initialise game state.\n");
  }

  // Get the main stack
  GtkStack* stack = GTK_STACK(gtk_builder_get_object(builder, "main_stack"));
  if (!stack) {
    g_printerr("Failed to find main_stack in UI file.\n");
    g_object_unref(builder);
    return;
  }

  // Initialise all UI components
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
  play_sound(SOUND_THEME);
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

  // Load the Naive Bayes model
  int nb_err;
#ifdef EMBED_NB_MODEL
  nb_err = load_nb_model(&nb_model, NULL) != 0;
#else
  nb_err = load_nb_model(&nb_model, DEFAULT_MODEL_PATH) != 0;
#endif
  if (nb_err != 0) {
    fprintf(stderr, "Error: Failed to load Naive Bayes model\n");
    return EXIT_FAILURE;
  }

  // Create and run the GTK application
  GtkApplication* app =
      GTK_APPLICATION(gtk_application_new(APP_ID, G_APPLICATION_DEFAULT_FLAGS));
  if (!app) {
    fprintf(stderr, "Error: Failed to create GTK application\n");
    return EXIT_FAILURE;
  }
  g_signal_connect(app, "activate", G_CALLBACK(gui_activate), NULL);
  int status = g_application_run(G_APPLICATION(app), argc, argv);

  // Clean up
  g_object_unref(app);
  cleanup_audio();

  return status;
}
