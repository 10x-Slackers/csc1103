#include "option_menu.h"

#include "board_gui.h"
#include "game.h"
#include "gui.h"

/**
 * @brief Handle Player X selection.
 *
 * Sets the first player to X and starts the game.
 *
 * @param button Pointer to the GtkButton.
 * @param user_data User data (GtkStack pointer).
 */
static void x_clicked(GtkButton* button G_GNUC_UNUSED, gpointer user_data) {
  set_first_player(PLAYER_X);
  update_game_state();
  navigate_to(GTK_STACK(user_data), "game");
}

/**
 * @brief Handle Player O selection.
 *
 * Sets the first player to O and starts the game.
 *
 * @param button Pointer to the GtkButton.
 * @param user_data User data (GtkStack pointer).
 */
static void o_clicked(GtkButton* button G_GNUC_UNUSED, gpointer user_data) {
  set_first_player(PLAYER_O);
  update_game_state();
  navigate_to(GTK_STACK(user_data), "game");
}

/**
 * @brief Handle Easy difficulty selection.
 *
 * Sets the difficulty to easy and navigates to player selection.
 *
 * @param button Pointer to the GtkButton.
 * @param user_data User data (GtkStack pointer).
 */
static void set_diff_easy(GtkButton* button G_GNUC_UNUSED, gpointer user_data) {
  set_difficulty(DIFF_EASY);
  navigate_to(GTK_STACK(user_data), "player_select");
}

/**
 * @brief Handle Medium difficulty selection.
 *
 * Sets the difficulty to medium and navigates to player selection.
 *
 * @param button Pointer to the GtkButton.
 * @param user_data User data (GtkStack pointer).
 */
static void set_diff_medium(GtkButton* button G_GNUC_UNUSED,
                            gpointer user_data) {
  set_difficulty(DIFF_MEDIUM);
  navigate_to(GTK_STACK(user_data), "player_select");
}

/**
 * @brief Handle Hard difficulty selection.
 *
 * Sets the difficulty to hard and navigates to player selection.
 *
 * @param button Pointer to the GtkButton.
 * @param user_data User data (GtkStack pointer).
 */
static void set_diff_hard(GtkButton* button G_GNUC_UNUSED, gpointer user_data) {
  set_difficulty(DIFF_HARD);
  navigate_to(GTK_STACK(user_data), "player_select");
}

/**
 * @brief Initialize player selection UI and connect signals.
 *
 * Sets up the player selection UI for choosing whether to play as X or O.
 * Connects signal handlers for player selection buttons.
 *
 * @param builder Pointer to the GtkBuilder.
 * @param stack Pointer to the GtkStack.
 * @return 0 on success, -1 on failure.
 */
int player_select(GtkBuilder* builder, GtkStack* stack) {
  GtkWidget* x_button = GTK_WIDGET(gtk_builder_get_object(builder, "player_x"));
  if (x_button) {
    g_signal_connect(x_button, "clicked", G_CALLBACK(x_clicked), stack);
  }

  GtkWidget* o_button = GTK_WIDGET(gtk_builder_get_object(builder, "player_o"));
  if (o_button) {
    g_signal_connect(o_button, "clicked", G_CALLBACK(o_clicked), stack);
  }

  return 0;
}

/**
 * @brief Initialize difficulty selection UI and connect signals.
 *
 * Sets up the difficulty selection UI for choosing easy, medium, or hard
 * difficulty. Connects signal handlers for difficulty selection buttons.
 *
 * @param builder Pointer to the GtkBuilder.
 * @param stack Pointer to the GtkStack.
 * @return 0 on success, -1 on failure.
 */
int difficulty_select(GtkBuilder* builder, GtkStack* stack) {
  GtkWidget* easy_button =
      GTK_WIDGET(gtk_builder_get_object(builder, "diff_easy"));
  if (easy_button) {
    g_signal_connect(easy_button, "clicked", G_CALLBACK(set_diff_easy), stack);
  }

  GtkWidget* medium_button =
      GTK_WIDGET(gtk_builder_get_object(builder, "diff_medium"));
  if (medium_button) {
    g_signal_connect(medium_button, "clicked", G_CALLBACK(set_diff_medium),
                     stack);
  }

  GtkWidget* hard_button =
      GTK_WIDGET(gtk_builder_get_object(builder, "diff_hard"));
  if (hard_button) {
    g_signal_connect(hard_button, "clicked", G_CALLBACK(set_diff_hard), stack);
  }

  return 0;
}
