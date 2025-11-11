/*
 * Creates and displays the main menu for the game
 *
 * Author: Jerome
 * Date: 2025-11-10
 * Version: 1
 */
#include "main_menu.h"

#include <gtk/gtk.h>

#include "pick_difficulty.h"
#include "tic_tac_toe_grid.h"

/* Constants */
#define WINDOW_LENGTH 400
#define WINDOW_WIDTH 500
#define SPACING 20
#define MARGIN 40
#define XALIGN 0.5

/* Function prototypes */
static void on_one_player_clicked(GtkButton* button, gpointer user_data);
static void on_two_player_clicked(GtkButton* button, gpointer user_data);
static void on_quit_clicked(GtkButton* button, gpointer user_data);
static void apply_css(void);
static void setup_main_container(GtkBox* vbox);
static void add_welcome_label(GtkBox* vbox);
static void add_player_buttons(GtkBox* vbox, GtkApplication* app);
static void add_spacer(GtkBox* vbox);
static void add_quit_button(GtkBox* vbox);

/**
 * @brief Creates and displays the main menu window
 *
 * Initializes the main menu GUI with player selection buttons,
 * welcome message, and quit button. Sets up the window properties
 * and applies custom CSS styling.
 *
 * @param app Pointer to the GtkApplication instance
 */
void main_menu(GtkApplication* app) {
  GtkWidget* window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "Tic Tac Toe Fun!");
  gtk_window_set_default_size(GTK_WINDOW(window), WINDOW_LENGTH, WINDOW_WIDTH);
  gtk_window_set_resizable(GTK_WINDOW(window), TRUE);

  apply_css();

  GtkBox* vbox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, SPACING));
  setup_main_container(vbox);
  add_welcome_label(vbox);
  add_player_buttons(vbox, app);
  add_spacer(vbox);
  add_quit_button(vbox);

  gtk_window_set_child(GTK_WINDOW(window), GTK_WIDGET(vbox));
  gtk_window_present(GTK_WINDOW(window));
}

/**
 * @brief Sets up margins for the main container
 *
 * Applies uniform margins to all sides of the vertical box container
 * to ensure proper spacing from window edges.
 *
 * @param vbox Pointer to the GtkBox container
 */
static void setup_main_container(GtkBox* vbox) {
  gtk_widget_set_margin_top(GTK_WIDGET(vbox), MARGIN);
  gtk_widget_set_margin_bottom(GTK_WIDGET(vbox), MARGIN);
  gtk_widget_set_margin_start(GTK_WIDGET(vbox), MARGIN);
  gtk_widget_set_margin_end(GTK_WIDGET(vbox), MARGIN);
}

/**
 * @brief Adds the welcome label to the menu
 *
 * Creates and styles a welcome message label with custom CSS styling
 * and centers it horizontally.
 *
 * @param vbox Pointer to the GtkBox container to append the label to
 */
static void add_welcome_label(GtkBox* vbox) {
  GtkWidget* label = gtk_label_new("🎮 Tic Tac Toe Fun! 🎮");
  gtk_label_set_xalign(GTK_LABEL(label), XALIGN);
  gtk_widget_set_name(label, "welcome");
  gtk_box_append(vbox, label);
}

/**
 * @brief Adds player mode selection buttons
 *
 * Creates two buttons for selecting 1-player or 2-player game modes,
 * connects their click handlers, and adds them to the menu.
 *
 * @param vbox Pointer to the GtkBox container
 * @param app Pointer to the GtkApplication (passed to click handlers)
 */
static void add_player_buttons(GtkBox* vbox, GtkApplication* app) {
  GtkWidget* button_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, SPACING);

  GtkWidget* btn_1p = gtk_button_new_with_label("🤖 Play vs Computer");
  gtk_widget_set_name(btn_1p, "player-button");
  g_signal_connect(btn_1p, "clicked", G_CALLBACK(on_one_player_clicked), app);
  gtk_box_append(GTK_BOX(button_container), btn_1p);

  GtkWidget* btn_2p = gtk_button_new_with_label("👥 Play with Friend");
  gtk_widget_set_name(btn_2p, "player-button");
  g_signal_connect(btn_2p, "clicked", G_CALLBACK(on_two_player_clicked), app);
  gtk_box_append(GTK_BOX(button_container), btn_2p);

  gtk_box_append(vbox, button_container);
}

/**
 * @brief Adds a flexible spacer element
 *
 * Creates an expandable spacer to push elements apart vertically,
 * providing flexible spacing in the layout.
 *
 * @param vbox Pointer to the GtkBox container
 */
static void add_spacer(GtkBox* vbox) {
  GtkWidget* spacer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_set_hexpand(spacer, TRUE);
  gtk_widget_set_vexpand(spacer, TRUE);
  gtk_box_append(vbox, spacer);
}

/**
 * @brief Adds the quit button to the menu
 *
 * Creates a quit button at the bottom of the menu and connects
 * its click handler for closing the application.
 *
 * @param vbox Pointer to the GtkBox container
 */
static void add_quit_button(GtkBox* vbox) {
  GtkWidget* quit_container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, SPACING);
  GtkWidget* btn_quit = gtk_button_new_with_label("Exit");
  gtk_widget_set_name(btn_quit, "quit-button");
  g_signal_connect(btn_quit, "clicked", G_CALLBACK(on_quit_clicked), NULL);
  gtk_box_append(GTK_BOX(quit_container), btn_quit);
  gtk_box_append(vbox, quit_container);
}

/**
 * @brief Callback for 1 Player button click
 *
 * Launches the game in single-player mode and closes the menu window.
 *
 * @param button Pointer to the button that was clicked
 * @param user_data Pointer to the GtkApplication instance
 */
static void on_one_player_clicked(GtkButton* button, gpointer user_data) {
  GtkApplication* app = GTK_APPLICATION(user_data);
  GtkWindow* window = GTK_WINDOW(gtk_widget_get_root(GTK_WIDGET(button)));
  pick_difficulty(app);
  gtk_window_close(window);
}

/**
 * @brief Callback for 2 Player button click
 *
 * Launches the game in two-player mode and closes the menu window.
 *
 * @param button Pointer to the button that was clicked
 * @param user_data Pointer to the GtkApplication instance
 */
static void on_two_player_clicked(GtkButton* button, gpointer user_data) {
  GtkApplication* app = GTK_APPLICATION(user_data);
  GtkWindow* window = GTK_WINDOW(gtk_widget_get_root(GTK_WIDGET(button)));
  launch_game(app, "2P");
  gtk_window_close(window);
}

/**
 * @brief Callback for Quit button click
 *
 * Closes the menu window, terminating the application.
 *
 * @param button Pointer to the button that was clicked
 * @param user_data User data (unused)
 */
static void on_quit_clicked(GtkButton* button, gpointer user_data) {
  (void)user_data;
  GtkWindow* window = GTK_WINDOW(gtk_widget_get_root(GTK_WIDGET(button)));
  gtk_window_close(window);
}

/**
 * @brief Applies custom CSS styling to the application
 *
 * Loads and applies custom CSS rules for styling with bright colors,
 * rounded corners, and fun animations for a kid-friendly interface.
 */
static void apply_css(void) {
  const gchar* css =
      "window {"
      "   background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);"
      "   background-color: #667eea;"
      "}"
      ""
      "label#welcome {"
      "   font-family: 'Comic Sans MS', cursive, sans-serif;"
      "   font-size: 36px;"
      "   font-weight: bold;"
      "   color: #ffffff;"
      "   text-shadow: 3px 3px 6px rgba(0, 0, 0, 0.3),"
      "                -1px -1px 2px rgba(255, 255, 255, 0.2);"
      "   padding: 20px;"
      "   margin-bottom: 30px;"
      "}"
      ""
      "button#player-button {"
      "   font-family: 'Comic Sans MS', cursive, sans-serif;"
      "   font-size: 22px;"
      "   font-weight: bold;"
      "   color: #1a1a2e;"
      "   background: linear-gradient(135deg, #ffd700 0%, #ffed4e 100%);"
      "   background-color: #ffd700;"
      "   border: 4px solid #1a1a2e;"
      "   border-radius: 25px;"
      "   padding: 20px 40px;"
      "   min-height: 70px;"
      "   box-shadow: 0 6px 12px rgba(0, 0, 0, 0.3),"
      "               inset 0 1px 0 rgba(255, 255, 255, 0.3);"
      "   transition: all 0.3s ease;"
      "}"
      ""
      "button#player-button:hover {"
      "   background: linear-gradient(135deg, #ffe44d 0%, #fff9a3 100%);"
      "   background-color: #ffe44d;"
      "   transform: scale(1.05);"
      "   box-shadow: 0 8px 16px rgba(0, 0, 0, 0.4),"
      "               inset 0 1px 0 rgba(255, 255, 255, 0.4);"
      "}"
      ""
      "button#player-button:active {"
      "   transform: scale(0.98);"
      "   box-shadow: 0 4px 8px rgba(0, 0, 0, 0.3);"
      "}"
      ""
      "button#quit-button {"
      "   font-family: 'Comic Sans MS', cursive, sans-serif;"
      "   font-size: 16px;"
      "   font-weight: bold;"
      "   color: #1a1a2e;"
      "   background: linear-gradient(135deg, #ff6b6b 0%, #ff8e8e 100%);"
      "   background-color: #ff6b6b;"
      "   border: 3px solid #1a1a2e;"
      "   border-radius: 20px;"
      "   padding: 12px 30px;"
      "   box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2);"
      "}"
      ""
      "button#quit-button:hover {"
      "   background: linear-gradient(135deg, #ff8787 0%, #ffa5a5 100%);"
      "   background-color: #ff8787;"
      "   transform: scale(1.03);"
      "}";

  GtkCssProvider* style_provider = gtk_css_provider_new();
  gtk_css_provider_load_from_string(style_provider, css);

  GdkDisplay* display = gdk_display_get_default();
  gtk_style_context_add_provider_for_display(
      display, GTK_STYLE_PROVIDER(style_provider),
      GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  g_object_unref(style_provider);
}
