/**
 * @file main_menu.c
 * @brief Handles main menu interactions.
 * @authors Jerome, commit2main
 * @date 2025-11-13
 */
#include "main_menu.h"

#include "audio.h"
#include "game.h"
#include "option_menu.h"

/**
 * @brief Handle one player button click.
 * @param builder Pointer to the GtkBuilder.
 */
static void one_player_clicked(GtkBuilder* builder) {
  play_sound(SOUND_CLICK);
  set_game_mode(MODE_1_PLAYER);
  update_ai_hint_visibility(builder);
  // Navigate to difficulty selection
  GtkStack* stack = GTK_STACK(gtk_builder_get_object(builder, "main_stack"));
  if (stack) gtk_stack_set_visible_child_name(stack, "diff_select");
}

/**
 * @brief Handle two player button click.
 * @param builder Pointer to the GtkBuilder.
 */
static void two_player_clicked(GtkBuilder* builder) {
  play_sound(SOUND_CLICK);
  set_game_mode(MODE_2_PLAYER);
  update_ai_hint_visibility(builder);
  // Navigate to player selection
  GtkStack* stack = GTK_STACK(gtk_builder_get_object(builder, "main_stack"));
  if (stack) gtk_stack_set_visible_child_name(stack, "player_select");
}

/**
 * @brief Handle audio toggle button.
 * @param toggle_button Pointer to the GtkToggleButton.
 * @param user_data User data (unused).
 */
static void toggle_audio(GtkToggleButton* toggle_button,
                         gpointer user_data G_GNUC_UNUSED) {
  // Get audio state
  gboolean is_active = gtk_toggle_button_get_active(toggle_button);
  if (is_active) {
    set_audio_enabled(true);
    gtk_button_set_icon_name(GTK_BUTTON(toggle_button),
                             "audio-volume-high-symbolic");
  } else {
    set_audio_enabled(false);
    gtk_button_set_icon_name(GTK_BUTTON(toggle_button),
                             "audio-volume-muted-symbolic");
  }
}

void main_menu(GtkBuilder* builder, GtkStack* stack) {
  GtkWidget* one_player =
      GTK_WIDGET(gtk_builder_get_object(builder, "one_player"));
  GtkWidget* two_player =
      GTK_WIDGET(gtk_builder_get_object(builder, "two_player"));
  GtkWidget* audio_toggle =
      GTK_WIDGET(gtk_builder_get_object(builder, "audio_toggle"));
  GtkWidget* quit_game =
      GTK_WIDGET(gtk_builder_get_object(builder, "quit_game"));
  GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(builder, "window"));
  if (!one_player || !two_player || !audio_toggle || !quit_game || !window) {
    g_printerr("Could not find main menu buttons.\n");
    return;
  }
  // Connect signals for menu buttons
  g_signal_connect_swapped(one_player, "clicked",
                           G_CALLBACK(one_player_clicked), builder);
  g_signal_connect_swapped(two_player, "clicked",
                           G_CALLBACK(two_player_clicked), builder);
  g_signal_connect(audio_toggle, "toggled", G_CALLBACK(toggle_audio), NULL);
  g_signal_connect_swapped(quit_game, "clicked", G_CALLBACK(gtk_window_close),
                           window);
}
