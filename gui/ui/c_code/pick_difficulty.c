/*
 * GUI for picking difficulty of the bot to play against
 *
 * Author: Jerome
 * Date: 2025-11-10
 * Version: 1
 */

#include <gtk/gtk.h>

#include "main_menu.h"
#include "tic_tac_toe_grid.h"

// ============================================================================
// CONSTANTS
// ============================================================================

#define WINDOW_LENGTH 400
#define WINDOW_WIDTH 500
#define SPACING 20
#define MARGIN 40
#define XALIGN 0.5

// ============================================================================
// DIFFICULTY LEVELS
// ============================================================================

/** @brief Easy difficulty identifier */
#define DIFFICULTY_EASY "😊 Easy Peasy"

/** @brief Medium difficulty identifier */
#define DIFFICULTY_MEDIUM "🤔 Medium Challenge"

/** @brief Hard difficulty identifier */
#define DIFFICULTY_HARD "😎 Super Hard!"

// ============================================================================
// FUNCTION PROTOTYPES
// ============================================================================

static void on_difficulty_clicked(GtkButton* button, gpointer user_data);
static void on_back_to_main_menu_clicked(GtkButton* button,
                                         GtkApplication* app);
static void apply_css(void);
static void setup_main_container(GtkBox* vbox);
static void add_difficulty_label(GtkBox* vbox);
static void add_difficulty_buttons(GtkBox* vbox, GtkApplication* app);
static void add_spacer(GtkBox* vbox);
static void add_back_to_main_menu_button(GtkBox* vbox, GtkApplication* app);

// ============================================================================
// MAIN WINDOW CREATION
// ============================================================================

/**
 * @brief Creates and displays the difficulty selection window
 *
 * Initializes the GTK window for difficulty selection, applies styling,
 * and sets up all UI components including difficulty buttons and
 * navigation controls.
 *
 * @param app The GTK application instance
 */
void pick_difficulty(GtkApplication* app) {
  GtkWidget* window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "Choose Your Level!");
  gtk_window_set_default_size(GTK_WINDOW(window), WINDOW_LENGTH, WINDOW_WIDTH);
  gtk_window_set_resizable(GTK_WINDOW(window), TRUE);

  apply_css();

  GtkBox* vbox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, SPACING));
  setup_main_container(vbox);
  add_difficulty_label(vbox);
  add_difficulty_buttons(vbox, app);
  add_spacer(vbox);
  add_back_to_main_menu_button(vbox, app);

  gtk_window_set_child(GTK_WINDOW(window), GTK_WIDGET(vbox));
  gtk_window_present(GTK_WINDOW(window));
}

// ============================================================================
// UI SETUP FUNCTIONS
// ============================================================================

/**
 * @brief Configures margins for the main container
 *
 * Sets uniform margins on all sides of the main vertical box container.
 *
 * @param vbox The main vertical box container
 */
static void setup_main_container(GtkBox* vbox) {
  gtk_widget_set_margin_top(GTK_WIDGET(vbox), MARGIN);
  gtk_widget_set_margin_bottom(GTK_WIDGET(vbox), MARGIN);
  gtk_widget_set_margin_start(GTK_WIDGET(vbox), MARGIN);
  gtk_widget_set_margin_end(GTK_WIDGET(vbox), MARGIN);
}

/**
 * @brief Creates and adds the difficulty selection title label
 *
 * Creates a centered label with the text "Pick your difficulty" and
 * applies custom styling through CSS.
 *
 * @param vbox The vertical box container to append the label to
 */
static void add_difficulty_label(GtkBox* vbox) {
  GtkWidget* label = gtk_label_new("⭐ Choose Your Level! ⭐");
  gtk_label_set_xalign(GTK_LABEL(label), XALIGN);
  gtk_widget_set_name(label, "difficulty");
  gtk_box_append(vbox, label);
}

/**
 * @brief Creates and adds all difficulty selection buttons
 *
 * Creates three buttons (Easy, Medium, Hard) and connects them to
 * the appropriate callback handlers. All buttons are placed in a
 * vertical container.
 *
 * @param vbox The vertical box container to append buttons to
 * @param app The GTK application instance to pass to callbacks
 */
static void add_difficulty_buttons(GtkBox* vbox, GtkApplication* app) {
  GtkWidget* button_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, SPACING);

  // Define button configuration data
  struct {
    const char* label;
    const char* css_name;
  } buttons[] = {{DIFFICULTY_EASY, "easy-button"},
                 {DIFFICULTY_MEDIUM, "medium-button"},
                 {DIFFICULTY_HARD, "hard-button"}};

  // Create all difficulty buttons
  for (int i = 0; i < 3; i++) {
    GtkWidget* button = gtk_button_new_with_label(buttons[i].label);
    gtk_widget_set_name(button, buttons[i].css_name);
    g_signal_connect(button, "clicked", G_CALLBACK(on_difficulty_clicked), app);
    gtk_box_append(GTK_BOX(button_container), button);
  }

  gtk_box_append(vbox, button_container);
}

/**
 * @brief Adds an expanding spacer element
 *
 * Creates a flexible spacer that expands to push other elements apart,
 * ensuring proper vertical spacing in the layout.
 *
 * @param vbox The vertical box container to append the spacer to
 */
static void add_spacer(GtkBox* vbox) {
  GtkWidget* spacer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_set_hexpand(spacer, TRUE);
  gtk_widget_set_vexpand(spacer, TRUE);
  gtk_box_append(vbox, spacer);
}

/**
 * @brief Creates and adds the "Back to Main Menu" button
 *
 * Adds a button at the bottom of the window that allows users to
 * return to the main menu screen.
 *
 * @param vbox The vertical box container to append the button to
 * @param app The GTK application instance to pass to the callback
 */
static void add_back_to_main_menu_button(GtkBox* vbox, GtkApplication* app) {
  GtkWidget* quit_container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, SPACING);
  GtkWidget* btn_main_menu = gtk_button_new_with_label("⬅️ Back");
  gtk_widget_set_name(btn_main_menu, "back-button");
  g_signal_connect(btn_main_menu, "clicked",
                   G_CALLBACK(on_back_to_main_menu_clicked), app);
  gtk_box_append(GTK_BOX(quit_container), btn_main_menu);
  gtk_box_append(vbox, quit_container);
}

// ============================================================================
// EVENT HANDLERS
// ============================================================================

/**
 * @brief Handles difficulty button clicks
 *
 * Called when any difficulty button (Easy, Medium, or Hard) is clicked.
 * Closes the difficulty selection window and launches the game with the
 * selected difficulty level.
 *
 * @note Currently all difficulties launch the same "1P" mode. Future
 *       implementations should pass different difficulty parameters based
 *       on which button was clicked.
 *
 * @param button The clicked button widget
 * @param user_data Pointer to the GTK application instance
 */
static void on_difficulty_clicked(GtkButton* button, gpointer user_data) {
  GtkApplication* app = GTK_APPLICATION(user_data);
  GtkWindow* window = GTK_WINDOW(gtk_widget_get_root(GTK_WIDGET(button)));

  // Get the button label to determine difficulty
  const char* difficulty = gtk_button_get_label(button);

  // TODO: Pass actual difficulty level to launch_game
  // For now, all difficulties launch the same mode
  launch_game(app, "1P");

  gtk_window_close(window);
}

/**
 * @brief Handles the "Back to Main Menu" button click
 *
 * Closes the difficulty selection window and returns to the main menu.
 *
 * @param button The clicked button widget
 * @param app The GTK application instance
 */
static void on_back_to_main_menu_clicked(GtkButton* button,
                                         GtkApplication* app) {
  GtkWindow* window = GTK_WINDOW(gtk_widget_get_root(GTK_WIDGET(button)));
  gtk_window_close(window);
  main_menu(app);
}

// ============================================================================
// CSS STYLING
// ============================================================================

/**
 * @brief Applies custom CSS styling to the application
 *
 * Loads and applies custom CSS rules for styling the difficulty selection
 * with bright colors, rounded corners, and fun hover effects.
 */
static void apply_css(void) {
  const gchar* css =
      "window {\n"
      "   background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);\n"
      "   background-color: #667eea;\n"
      "}\n"
      "\n"
      "label#difficulty {\n"
      "   font-family: 'Comic Sans MS', cursive, sans-serif;\n"
      "   font-size: 36px;\n"
      "   font-weight: bold;\n"
      "   color: #ffffff;\n"
      "   text-shadow: 3px 3px 6px rgba(0, 0, 0, 0.3),\n"
      "                -1px -1px 2px rgba(255, 255, 255, 0.2);\n"
      "   padding: 20px;\n"
      "   margin-bottom: 30px;\n"
      "}\n"
      "\n"
      "button#easy-button {\n"
      "   font-family: 'Comic Sans MS', cursive, sans-serif;\n"
      "   font-size: 22px;\n"
      "   font-weight: bold;\n"
      "   color: #1a1a2e;\n"
      "   background: linear-gradient(135deg, #4ade80 0%, #86efac 100%);\n"
      "   background-color: #4ade80;\n"
      "   border: 4px solid #1a1a2e;\n"
      "   border-radius: 25px;\n"
      "   padding: 20px 40px;\n"
      "   min-height: 70px;\n"
      "   box-shadow: 0 6px 12px rgba(0, 0, 0, 0.3);\n"
      "}\n"
      "\n"
      "button#easy-button:hover {\n"
      "   background: linear-gradient(135deg, #6ee7a7 0%, #a7f3c0 100%);\n"
      "   background-color: #6ee7a7;\n"
      "   transform: scale(1.05);\n"
      "}\n"
      "\n"
      "button#medium-button {\n"
      "   font-family: 'Comic Sans MS', cursive, sans-serif;\n"
      "   font-size: 22px;\n"
      "   font-weight: bold;\n"
      "   color: #1a1a2e;\n"
      "   background: linear-gradient(135deg, #fbbf24 0%, #fcd34d 100%);\n"
      "   background-color: #fbbf24;\n"
      "   border: 4px solid #1a1a2e;\n"
      "   border-radius: 25px;\n"
      "   padding: 20px 40px;\n"
      "   min-height: 70px;\n"
      "   box-shadow: 0 6px 12px rgba(0, 0, 0, 0.3);\n"
      "}\n"
      "\n"
      "button#medium-button:hover {\n"
      "   background: linear-gradient(135deg, #fcd34d 0%, #fde68a 100%);\n"
      "   background-color: #fcd34d;\n"
      "   transform: scale(1.05);\n"
      "}\n"
      "\n"
      "button#hard-button {\n"
      "   font-family: 'Comic Sans MS', cursive, sans-serif;\n"
      "   font-size: 22px;\n"
      "   font-weight: bold;\n"
      "   color: #1a1a2e;\n"
      "   background: linear-gradient(135deg, #f87171 0%, #fca5a5 100%);\n"
      "   background-color: #f87171;\n"
      "   border: 4px solid #1a1a2e;\n"
      "   border-radius: 25px;\n"
      "   padding: 20px 40px;\n"
      "   min-height: 70px;\n"
      "   box-shadow: 0 6px 12px rgba(0, 0, 0, 0.3);\n"
      "}\n"
      "\n"
      "button#hard-button:hover {\n"
      "   background: linear-gradient(135deg, #fca5a5 0%, #fecaca 100%);\n"
      "   background-color: #fca5a5;\n"
      "   transform: scale(1.05);\n"
      "}\n"
      "\n"
      "button#back-button {\n"
      "   font-family: 'Comic Sans MS', cursive, sans-serif;\n"
      "   font-size: 16px;\n"
      "   font-weight: bold;\n"
      "   color: #1a1a2e;\n"
      "   background: linear-gradient(135deg, #60a5fa 0%, #93c5fd 100%);\n"
      "   background-color: #60a5fa;\n"
      "   border: 3px solid #1a1a2e;\n"
      "   border-radius: 20px;\n"
      "   padding: 12px 30px;\n"
      "   box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2);\n"
      "}\n"
      "\n"
      "button#back-button:hover {\n"
      "   background: linear-gradient(135deg, #93c5fd 0%, #bfdbfe 100%);\n"
      "   background-color: #93c5fd;\n"
      "   transform: scale(1.03);\n"
      "}\n";

  GtkCssProvider* style_provider = gtk_css_provider_new();
  gtk_css_provider_load_from_string(style_provider, css);

  GdkDisplay* display = gdk_display_get_default();
  gtk_style_context_add_provider_for_display(
      display, GTK_STYLE_PROVIDER(style_provider),
      GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

  g_object_unref(style_provider);
}
