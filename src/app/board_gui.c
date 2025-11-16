#include "board_gui.h"

#include "audio.h"

/* Forward Declarations */
static gboolean process_ai_move(gpointer user_data);

/**
 * @brief Update a score label with a numeric value.
 * @param builder Pointer to the GtkBuilder.
 * @param label_name Name of the label widget.
 * @param score The score value to display.
 */
static void update_score_label(GtkBuilder* builder, const char* label_name,
                               int score) {
  GtkLabel* label = GTK_LABEL(gtk_builder_get_object(builder, label_name));
  if (label) {
    char buffer[SCORE_BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "%d", score);
    gtk_label_set_text(label, buffer);
  }
}

/**
 * @brief Get the image resource path for a cell state.
 * @param state The cell state.
 * @return Resource path to the appropriate image file.
 */
static const char* get_cell_image(CellState state) {
  switch (state) {
    case X:
      return X_IMAGE_RESOURCE;
    case O:
      return O_IMAGE_RESOURCE;
    default:
      return BLANK_IMAGE_RESOURCE;
  }
}

/**
 * @brief Get the winner message for the dialog.
 * @param winner The winner state.
 * @return Message string for the winner.
 */
static const char* get_winner_message(Winner winner) {
  switch (winner) {
    case WIN_X:
      return X_WIN_MESSAGE;
    case WIN_O:
      return O_WIN_MESSAGE;
    case DRAW:
      return DRAW_MESSAGE;
    default:
      return NULL;
  }
}

/**
 * @brief Update the scoreboard display.
 * @param g_game_state Pointer to the GameState.
 * @return 0 on success, -1 on failure.
 */
static int update_scoreboard(GameState* g_game_state) {
  if (!g_game_state || !g_game_state->builder) return -1;

  update_score_label(g_game_state->builder, "score_x",
                     g_game_state->stats.score_X);
  update_score_label(g_game_state->builder, "score_o",
                     g_game_state->stats.score_O);
  update_score_label(g_game_state->builder, "score_tie",
                     g_game_state->stats.score_tie);

  return 0;
}

/**
 * @brief Update the cell button images based on game state.
 * @param g_game_state Pointer to the GameState.
 * @return 0 on success, -1 on failure.
 */
static int update_board_display(GameState* g_game_state) {
  if (!g_game_state || !g_game_state->builder) return -1;

  GtkImage* score_x_image =
      GTK_IMAGE(gtk_builder_get_object(g_game_state->builder, "sboard_x"));
  GtkImage* score_o_image =
      GTK_IMAGE(gtk_builder_get_object(g_game_state->builder, "sboard_o"));
  if (!score_x_image || !score_o_image) return -1;
  // Make current player glow
  if (g_game_state->board.current_player == PLAYER_X) {
    gtk_widget_add_css_class(GTK_WIDGET(score_x_image), "current-player");
    gtk_widget_remove_css_class(GTK_WIDGET(score_o_image), "current-player");
  } else {
    gtk_widget_add_css_class(GTK_WIDGET(score_o_image), "current-player");
    gtk_widget_remove_css_class(GTK_WIDGET(score_x_image), "current-player");
  }

  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      char button_img[BUTTON_NAME_SIZE];
      int cell_index = i * SIZE + j + 1;
      snprintf(button_img, sizeof(button_img), "cell_%d_image", cell_index);

      GtkImage* cell_button =
          GTK_IMAGE(gtk_builder_get_object(g_game_state->builder, button_img));
      if (!cell_button) return -1;

      const char* image = get_cell_image(g_game_state->board.cells[i][j]);
      gtk_image_set_from_resource(cell_button, image);
    }
  }

  return 0;
}

/**
 * @brief Show either the undo button or difficulty dropdown based on player
 * mode.
 * @param g_game_state Pointer to the GameState.
 * @return 0 on success, -1 on failure.
 */
static int toggle_top_right(GameState* g_game_state) {
  if (!g_game_state || !g_game_state->builder) return -1;

  GtkWidget* diff_section =
      GTK_WIDGET(gtk_builder_get_object(g_game_state->builder, "diff_section"));
  GtkWidget* undo_section =
      GTK_WIDGET(gtk_builder_get_object(g_game_state->builder, "undo_section"));
  if (!undo_section || !diff_section) return -1;

  bool show_difficulty = (g_game_state->mode == MODE_1_PLAYER);
  gtk_widget_set_visible(undo_section, !show_difficulty);
  gtk_widget_set_visible(diff_section, show_difficulty);

  if (show_difficulty) {
    // Update difficulty dropdown to show current selection
    GtkDropDown* diff_dropdown = GTK_DROP_DOWN(
        gtk_builder_get_object(g_game_state->builder, "diff_dropdown"));
    if (!diff_dropdown) return -1;
    gtk_drop_down_set_selected(diff_dropdown,
                               g_game_state->difficulty - DIFF_EASY);
  } else {
    // Update undo button to be enabled/disabled based on last move availability
    GtkButton* undo_button = GTK_BUTTON(
        gtk_builder_get_object(g_game_state->builder, "undo_button"));
    if (!undo_button) return -1;
    gtk_widget_set_sensitive(GTK_WIDGET(undo_button),
                             g_game_state->board.last_move.row != -1);
  }

  return 0;
}

/**
 * @brief Show the win/draw dialog with a message.
 * @param g_game_state Pointer to the GameState.
 * @param message The message to display (e.g., "Player X wins!", "It's a
 * draw!").
 * @return 0 on success, -1 on failure.
 */
static int show_win_dialog(GameState* g_game_state, const char* message) {
  if (!g_game_state || !g_game_state->builder || !message) return -1;

  GtkLabel* win_label =
      GTK_LABEL(gtk_builder_get_object(g_game_state->builder, "win_msg"));
  GtkWidget* win_dialog =
      GTK_WIDGET(gtk_builder_get_object(g_game_state->builder, "win_dialog"));
  if (!win_label || !win_dialog) return -1;

  gtk_label_set_text(win_label, message);
  gtk_widget_set_visible(win_dialog, TRUE);

  return 0;
}

/**
 * @brief Highlight the winning cells by adding a CSS class.
 * @param g_game_state Pointer to the GameState.
 * @param winning_cells Array of the winning cells.
 */
static void highlight_winning_cells(GameState* g_game_state,
                                    const Cell winning_cells[SIZE]) {
  if (!g_game_state || !g_game_state->builder || !winning_cells) return;

  for (int i = 0; i < SIZE; i++) {
    int cell_index = winning_cells[i].row * SIZE + winning_cells[i].col + 1;
    char button_name[BUTTON_NAME_SIZE];
    snprintf(button_name, sizeof(button_name), "cell_%d", cell_index);

    GtkWidget* cell_button =
        GTK_WIDGET(gtk_builder_get_object(g_game_state->builder, button_name));
    if (cell_button) {
      gtk_widget_add_css_class(cell_button, "winning-cell");
    }
  }
}

/**
 * @brief Clear winning cell highlights by removing the CSS class.
 * @param g_game_state Pointer to the GameState.
 */
static void clear_winning_highlights(GameState* g_game_state) {
  if (!g_game_state || !g_game_state->builder) return;

  for (int i = 1; i <= 9; i++) {
    char button_name[BUTTON_NAME_SIZE];
    snprintf(button_name, sizeof(button_name), "cell_%d", i);

    GtkWidget* cell_button =
        GTK_WIDGET(gtk_builder_get_object(g_game_state->builder, button_name));
    if (cell_button) {
      gtk_widget_remove_css_class(cell_button, "winning-cell");
    }
  }
}

/**
 * @brief Reset the game state for a new game.
 * @return 0 on success, -1 on failure.
 */
static int reset_game_state() {
  GameState* g_game_state = get_game_state();
  if (!g_game_state) return -1;

  // Clear winning highlights from previous game
  clear_winning_highlights(g_game_state);

  // Alternate starting player for next game
  g_game_state->starting_player =
      (g_game_state->starting_player == PLAYER_X) ? PLAYER_O : PLAYER_X;
  // Clear the board and update display
  init_board(&g_game_state->board, g_game_state->starting_player);
  update_board_display(g_game_state);

  // Schedule AI move if it should go first
  if (g_game_state->mode == MODE_1_PLAYER &&
      g_game_state->board.current_player == PLAYER_O) {
    g_timeout_add(AI_MOVE_DELAY_MS, process_ai_move, g_game_state);
  }

  return 0;
}

/**
 * @brief Check for winner and update stats if game is over.
 * @param g_game_state Pointer to the GameState.
 * @return true if game is over, false otherwise.
 */
static bool check_game_over(GameState* g_game_state) {
  if (!g_game_state || !g_game_state->builder) return false;

  Cell winning_cells[SIZE];
  Winner winner = check_winner(&g_game_state->board, winning_cells);
  if (winner == ONGOING) return false;

  // Highlight winning cells if there's a winner (not a draw)
  if (winner == WIN_X || winner == WIN_O) {
    highlight_winning_cells(g_game_state, winning_cells);
  }

  // Update score based on winner
  switch (winner) {
    case WIN_X:
      g_game_state->stats.score_X++;
      break;
    case WIN_O:
      g_game_state->stats.score_O++;
      break;
    case DRAW:
      g_game_state->stats.score_tie++;
      break;
    default:
      g_print("Unknown winner state.\n");
      return false;
  }

  // Play win sound
  play_sound(SOUND_WIN);

  // Show win/draw dialog
  const char* message = get_winner_message(winner);
  if (message) {
    update_scoreboard(g_game_state);
    show_win_dialog(g_game_state, message);
  }

  return true;
}

/**
 * @brief Process AI move if it's AI's turn.
 *
 * Called as a timed callback to buffer for processing time.
 *
 * @param user_data Pointer to GameState.
 * @return G_SOURCE_REMOVE to indicate the timeout should not repeat.
 */
static gboolean process_ai_move(gpointer user_data) {
  GameState* g_game_state = (GameState*)user_data;
  if (!g_game_state) return G_SOURCE_REMOVE;

  Cell ai_move = get_ai_move();
  make_move(&g_game_state->board, &ai_move);
  update_board_display(g_game_state);
  check_game_over(g_game_state);

  return G_SOURCE_REMOVE;
}

/**
 * @brief Navigate to the main menu and reset the game state.
 * @param stack Pointer to the GtkStack for navigation.
 */
static void to_main_menu(GtkStack* stack) {
  reset_scoreboard();
  reset_game_state();
  gtk_stack_set_visible_child_name(stack, "main_menu");
}

/**
 * @brief Handle difficulty change from dropdown.
 * @param dropdown Pointer to the GtkDropDown.
 * @param pspec GParamSpec (unused).
 * @param user_data User data (unused).
 */
static void change_difficulty(GtkDropDown* dropdown,
                              GParamSpec* pspec G_GNUC_UNUSED,
                              gpointer user_data G_GNUC_UNUSED) {
  guint selected = gtk_drop_down_get_selected(dropdown);
  DifficultyLevel difficulty;

  switch (selected) {
    case 0:
      difficulty = DIFF_EASY;
      break;
    case 1:
      difficulty = DIFF_MEDIUM;
      break;
    case 2:
      difficulty = DIFF_HARD;
      break;
    default:
      difficulty = DIFF_NONE;
      break;
  }

  GameState* g_game_state = get_game_state();
  if (!g_game_state) return;

  // Only reset if difficulty actually changed
  if (g_game_state->difficulty == difficulty) return;
  set_difficulty(difficulty);
  reset_scoreboard();
  reset_game_state();
}

/**
 * @brief Handle play again button click.
 * @param builder Pointer to the GtkBuilder.
 */
static void play_again_clicked(GtkBuilder* builder) {
  // Hide the win dialog
  GtkWidget* win_dialog =
      GTK_WIDGET(gtk_builder_get_object(builder, "win_dialog"));
  if (win_dialog) gtk_widget_set_visible(win_dialog, FALSE);
  // Reset game state for a new game
  reset_game_state();
}

/**
 * @brief Handle go back button click.
 * @param builder Pointer to the GtkBuilder.
 */
static void go_back_clicked(GtkBuilder* builder) {
  // Hide the win dialog
  GtkWidget* win_dialog =
      GTK_WIDGET(gtk_builder_get_object(builder, "win_dialog"));
  if (win_dialog) gtk_widget_set_visible(win_dialog, FALSE);
  // Navigate to main menu
  GtkStack* stack = GTK_STACK(gtk_builder_get_object(builder, "main_stack"));
  if (stack) to_main_menu(stack);
}

/**
 * @brief Handle undo button click.
 */
static void undo_move_handler(void*) {
  GameState* g_game_state = get_game_state();
  if (undo_move(&g_game_state->board)) update_game_state();
}

/**
 * @brief Handle cell button click.
 * @param button Pointer to the GtkButton.
 * @param user_data User data (cell index as integer pointer).
 */
static void cell_clicked(GtkButton* button G_GNUC_UNUSED, gpointer user_data) {
  GameState* g_game_state = get_game_state();
  if (!g_game_state) return;

  // Check if game is already over or it's AI's turn
  if (check_winner(&g_game_state->board, NULL) != ONGOING) return;
  if (g_game_state->mode == MODE_1_PLAYER &&
      g_game_state->board.current_player == PLAYER_O)
    return;

  // Convert button index to board coordinates
  int cell_index = GPOINTER_TO_INT(user_data) - 1;
  Cell move = {.row = cell_index / SIZE, .col = cell_index % SIZE};

  if (!make_move(&g_game_state->board, &move)) return;

  play_sound(SOUND_CLICK);
  update_board_display(g_game_state);
  if (!check_game_over(g_game_state)) update_game_state();
}

int update_game_state() {
  GameState* g_game_state = get_game_state();
  if (!g_game_state || !g_game_state->builder) return -1;

  toggle_top_right(g_game_state);
  update_board_display(g_game_state);
  update_scoreboard(g_game_state);

  // Schedule AI move if it's the AI's turn
  if (g_game_state->mode == MODE_1_PLAYER &&
      g_game_state->board.current_player == PLAYER_O &&
      check_winner(&g_game_state->board, NULL) == ONGOING)
    g_timeout_add(AI_MOVE_DELAY_MS, process_ai_move, g_game_state);

  return 0;
}

void game_board(GtkBuilder* builder, GtkStack* stack) {
  GtkButton* back_button =
      GTK_BUTTON(gtk_builder_get_object(builder, "back_button"));
  GtkDropDown* diff_dropdown =
      GTK_DROP_DOWN(gtk_builder_get_object(builder, "diff_dropdown"));
  GtkButton* undo_button =
      GTK_BUTTON(gtk_builder_get_object(builder, "undo_button"));
  if (!back_button || !diff_dropdown || !undo_button) {
    g_printerr("Error: Could not find game board UI components.\n");
    return;
  }
  // Connect signals for game board buttons
  g_signal_connect_swapped(back_button, "clicked", G_CALLBACK(to_main_menu),
                           stack);
  g_signal_connect(diff_dropdown, "notify::selected",
                   G_CALLBACK(change_difficulty), NULL);
  g_signal_connect_swapped(undo_button, "clicked",
                           G_CALLBACK(undo_move_handler), NULL);

  // Connect all 9 cell buttons
  for (int i = 1; i <= 9; i++) {
    char button_name[BUTTON_NAME_SIZE];
    snprintf(button_name, sizeof(button_name), "cell_%d", i);
    GtkButton* cell_button =
        GTK_BUTTON(gtk_builder_get_object(builder, button_name));
    if (!cell_button) {
      g_printerr("Error: Could not find button %s.\n", button_name);
      continue;
    }
    g_signal_connect(cell_button, "clicked", G_CALLBACK(cell_clicked),
                     GINT_TO_POINTER(i));
  }
}

void win_dialog(GtkBuilder* builder) {
  GtkButton* play_again_button =
      GTK_BUTTON(gtk_builder_get_object(builder, "play_again"));
  GtkButton* go_back_button =
      GTK_BUTTON(gtk_builder_get_object(builder, "go_back"));
  if (!play_again_button || !go_back_button) {
    g_printerr("Error: Could not find win dialog buttons.\n");
    return;
  }
  // Connect signals for win dialog buttons
  g_signal_connect_swapped(play_again_button, "clicked",
                           G_CALLBACK(play_again_clicked), builder);
  g_signal_connect_swapped(go_back_button, "clicked",
                           G_CALLBACK(go_back_clicked), builder);
}
