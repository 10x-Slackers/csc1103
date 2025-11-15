#include "main_menu.h"

#include "game.h"
#include "gui.h"

/**
 * @brief Handle one player button click.
 *
 * Sets the game mode to single-player and navigates to difficulty selection.
 *
 * @param button Pointer to the GtkButton.
 * @param user_data User data (GtkStack pointer).
 */
static void one_player_clicked(GtkButton* button G_GNUC_UNUSED,
                               gpointer user_data) {
  set_game_mode(MODE_1_PLAYER);
  navigate_to(GTK_STACK(user_data), "diff_select");
}

/**
 * @brief Handle two player button click.
 *
 * Sets the game mode to two-player and navigates to player selection.
 *
 * @param button Pointer to the GtkButton.
 * @param user_data User data (GtkStack pointer).
 */
static void two_player_clicked(GtkButton* button G_GNUC_UNUSED,
                               gpointer user_data) {
  set_game_mode(MODE_2_PLAYER);
  navigate_to(GTK_STACK(user_data), "player_select");
}

/**
 * @brief Handle audio toggle button.
 *
 * Toggles the audio state and updates the button icon accordingly.
 *
 * @param toggle_button Pointer to the GtkToggleButton.
 * @param user_data User data (unused).
 */
static void toggle_audio(GtkToggleButton* toggle_button,
                         gpointer user_data G_GNUC_UNUSED) {
  // Get audio state
  gboolean is_active = gtk_toggle_button_get_active(toggle_button);
  if (is_active) {
    g_print("Audio Enabled\n");
    gtk_button_set_icon_name(GTK_BUTTON(toggle_button), "audio-volume-high");
  } else {
    g_print("Audio Disabled\n");
    gtk_button_set_icon_name(GTK_BUTTON(toggle_button), "audio-volume-muted");
  }
}

/**
 * @brief Initialize main menu UI and connect signals.
 *
 * Sets up the main menu UI components including one-player and two-player
 * buttons, and audio toggle button. Connects signal handlers for user
 * interactions.
 *
 * @param builder Pointer to the GtkBuilder.
 * @param stack Pointer to the GtkStack.
 * @return 0 on success, -1 on failure.
 */
int main_menu(GtkBuilder* builder, GtkStack* stack) {
  GtkWidget* one_player =
      GTK_WIDGET(gtk_builder_get_object(builder, "one_player"));
  if (one_player) {
    g_signal_connect(one_player, "clicked", G_CALLBACK(one_player_clicked),
                     stack);
  }

  GtkWidget* two_player =
      GTK_WIDGET(gtk_builder_get_object(builder, "two_player"));
  if (two_player) {
    g_signal_connect(two_player, "clicked", G_CALLBACK(two_player_clicked),
                     stack);
  }

  GtkWidget* audio_toggle =
      GTK_WIDGET(gtk_builder_get_object(builder, "audio_toggle"));
  if (audio_toggle) {
    g_signal_connect(audio_toggle, "toggled", G_CALLBACK(toggle_audio), NULL);
  }

  return 0;
}
