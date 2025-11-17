#include "option_menu.h"

#include "audio.h"
#include "board_gui.h"
#include "game.h"

/**
 * @brief Handle Player X selection.
 * @param stack Pointer to the GtkStack.
 */
static void x_clicked(GtkStack* stack) {
  play_sound(SOUND_CLICK);
  set_first_player(PLAYER_X);
  update_game_state();
  gtk_stack_set_visible_child_name(stack, "game");
}

/**
 * @brief Handle Player O selection.
 * @param stack Pointer to the GtkStack.
 */
static void o_clicked(GtkStack* stack) {
  play_sound(SOUND_CLICK);
  set_first_player(PLAYER_O);
  update_game_state();
  gtk_stack_set_visible_child_name(stack, "game");
}

/**
 * @brief Handle Easy difficulty selection.
 * @param stack Pointer to the GtkStack.
 */
static void set_diff_easy(GtkStack* stack) {
  play_sound(SOUND_CLICK);
  set_difficulty(DIFF_EASY);
  gtk_stack_set_visible_child_name(stack, "player_select");
}

/**
 * @brief Handle Medium difficulty selection.
 * @param stack Pointer to the GtkStack.
 */
static void set_diff_medium(GtkStack* stack) {
  play_sound(SOUND_CLICK);
  set_difficulty(DIFF_MEDIUM);
  gtk_stack_set_visible_child_name(stack, "player_select");
}

/**
 * @brief Handle Hard difficulty selection.
 * @param stack Pointer to the GtkStack.
 */
static void set_diff_hard(GtkStack* stack) {
  play_sound(SOUND_CLICK);
  set_difficulty(DIFF_HARD);
  gtk_stack_set_visible_child_name(stack, "player_select");
}

void update_ai_hint_visibility(GtkBuilder* builder) {
  const GameState* g_game_state = get_game_state();
  if (!g_game_state || !builder) return;

  GtkWidget* ai_hint =
      GTK_WIDGET(gtk_builder_get_object(builder, "player_ai_hint"));
  if (!ai_hint) {
    g_printerr("Error: Could not find AI hint widget.\n");
    return;
  }

  // Show AI hint only in one-player mode
  bool ai_mode = (g_game_state->mode == MODE_1_PLAYER);
  gtk_widget_set_visible(ai_hint, ai_mode);
}

void player_select(GtkBuilder* builder, GtkStack* stack) {
  GtkWidget* x_button = GTK_WIDGET(gtk_builder_get_object(builder, "player_x"));
  GtkWidget* o_button = GTK_WIDGET(gtk_builder_get_object(builder, "player_o"));
  if (!x_button || !o_button) {
    g_printerr("Error: Could not find player selection buttons.\n");
    return;
  }
  // Connect signals for player buttons
  g_signal_connect_swapped(x_button, "clicked", G_CALLBACK(x_clicked), stack);
  g_signal_connect_swapped(o_button, "clicked", G_CALLBACK(o_clicked), stack);
}

void difficulty_select(GtkBuilder* builder, GtkStack* stack) {
  GtkWidget* easy_button =
      GTK_WIDGET(gtk_builder_get_object(builder, "diff_easy"));
  GtkWidget* medium_button =
      GTK_WIDGET(gtk_builder_get_object(builder, "diff_medium"));
  GtkWidget* hard_button =
      GTK_WIDGET(gtk_builder_get_object(builder, "diff_hard"));
  if (!easy_button || !medium_button || !hard_button) {
    g_printerr("Error: Could not find difficulty selection buttons.\n");
    return;
  }
  // Connect signals for difficulty buttons
  g_signal_connect_swapped(easy_button, "clicked", G_CALLBACK(set_diff_easy),
                           stack);
  g_signal_connect_swapped(medium_button, "clicked",
                           G_CALLBACK(set_diff_medium), stack);
  g_signal_connect_swapped(hard_button, "clicked", G_CALLBACK(set_diff_hard),
                           stack);
}
