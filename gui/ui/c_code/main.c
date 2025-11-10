/*
 * Main to initialize Tic Tac Toe Application.
 *
 * Author: Jerome
 * Date: 2025-11-10
 * Version: 1
 */

#include <gtk/gtk.h>

#include "main_menu.h"

/**
 * @brief Callback function triggered when the GTK application is activated
 *
 * This function is called when the application starts and displays the main
 * menu.
 *
 * @param app Pointer to the GtkApplication instance
 * @param user_data User data pointer (unused)
 */
static void on_activate(GtkApplication* app, gpointer user_data) {
  main_menu(app);
}

/**
 * @brief Main entry point of the application
 *
 * Initializes the GTK application, connects the activation signal,
 * and runs the main event loop.
 *
 * @param argc Argument count from command line
 * @param argv Argument vector from command line
 * @return Exit status code (0 for success)
 */
int main(int argc, char** argv) {
  GtkApplication* app;
  int status;

  // Initialize GTK application with unique application ID
  app = gtk_application_new("com.csc1103.TicTacToeMenu",
                            G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);

  // Run application and capture exit status
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  return status;
}
