#include "board_gui.h"

#include "gui.h"

// Forward declarations
/**
 * @brief Process AI move if it's AI's turn.
 *
 * Called as a timed callback to handle the AI's move with a delay.
 *
 * @param user_data Pointer to GameState.
 * @return G_SOURCE_REMOVE to indicate the timeout should not repeat.
 */
static gboolean process_ai_move(gpointer user_data);

/**
 * @brief Update the scoreboard display.
 *
 * Updates the score labels for X, O, and ties based on the current game stats.
 *
 * @param g_game_state Pointer to the GameState.
 * @return 0 on success, -1 on failure.
 */
static int update_scoreboard(GameState* g_game_state) {
  if (!g_game_state || !g_game_state->builder) return -1;

  GtkLabel* score_x =
      GTK_LABEL(gtk_builder_get_object(g_game_state->builder, "score_x"));
  GtkLabel* score_o =
      GTK_LABEL(gtk_builder_get_object(g_game_state->builder, "score_o"));
  GtkLabel* score_tie =
      GTK_LABEL(gtk_builder_get_object(g_game_state->builder, "score_tie"));

  if (score_x && score_o && score_tie) {
    char buffer[SCORE_BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "%d", g_game_state->stats.score_X);
    gtk_label_set_text(score_x, buffer);

    snprintf(buffer, sizeof(buffer), "%d", g_game_state->stats.score_O);
    gtk_label_set_text(score_o, buffer);

    snprintf(buffer, sizeof(buffer), "%d", g_game_state->stats.score_tie);
    gtk_label_set_text(score_tie, buffer);
  }

  return 0;
}

/**
 * @brief Update the cell button images based on game state.
 *
 * Updates all cell button images to reflect the current board state (X, O, or
 * blank).
 *
 * @param g_game_state Pointer to the GameState.
 * @return 0 on success, -1 on failure.
 */
static int update_board_display(GameState* g_game_state) {
  if (!g_game_state || !g_game_state->builder) return -1;

  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      int cell_index = i * SIZE + j + 1;
      char button_img[BUTTON_NAME_SIZE];
      snprintf(button_img, sizeof(button_img), "cell_%d_image", cell_index);

      GtkImage* cell_button =
          GTK_IMAGE(gtk_builder_get_object(g_game_state->builder, button_img));
      if (!cell_button) return -1;

      CellState state = g_game_state->board.cells[i][j];
      const char* image = "resources/blank.png";

      switch (state) {
        case X:
          image = "resources/x.png";  // X icon
          break;
        case O:
          image = "resources/o.png";  // O icon
          break;
        default:
          break;
      }

      gtk_image_set_from_file(cell_button, image);
    }
  }

  return 0;
}

/**
 * @brief Reset the game state for a new game.
 *
 * Toggles the starting player and initializes a new board.
 * If in single-player mode and AI should move first, schedules the AI move.
 *
 * @return 0 on success, -1 on failure.
 */
static int reset_game_state() {
  GameState* g_game_state = get_game_state();
  if (!g_game_state) return -1;

  // Toggle starting player for next game
  g_game_state->starting_player =
      (g_game_state->starting_player == PLAYER_X) ? PLAYER_O : PLAYER_X;
  init_board(&g_game_state->board, g_game_state->starting_player);
  update_board_display(g_game_state);

  // Check if AI should move first
  if (g_game_state->mode == MODE_1_PLAYER &&
      g_game_state->board.current_player == PLAYER_O &&
      check_winner(&g_game_state->board) == ONGOING) {
    g_timeout_add(AI_MOVE_DELAY_MS, process_ai_move, g_game_state);
  }

  return 0;
}

/**
 * @brief Show the win/draw dialog with a message.
 *
 * Displays a dialog showing the game result and updates the UI accordingly.
 *
 * @param g_game_state Pointer to the GameState.
 * @param message The message to display (e.g., "Player X wins!", "It's a
 * draw!").
 * @return 0 on success, -1 on failure.
 */
static int show_win_dialog(GameState* g_game_state, const char* message) {
  if (!g_game_state || !g_game_state->builder) return -1;

  GtkLabel* win_label =
      GTK_LABEL(gtk_builder_get_object(g_game_state->builder, "win_msg"));
  if (!win_label) return -1;

  gtk_label_set_text(win_label, message);

  GtkWidget* win_dialog =
      GTK_WIDGET(gtk_builder_get_object(g_game_state->builder, "win_dialog"));
  if (!win_dialog) return -1;

  gtk_widget_set_visible(win_dialog, TRUE);

  return 0;
}

/**
 * @brief Show either the undo button or difficulty dropdown based on mode.
 *
 * In single-player mode, shows the difficulty dropdown.
 * In two-player mode, shows the undo button.
 *
 * @param g_game_state Pointer to the GameState.
 * @return 0 on success, -1 on failure.
 */
static int toggle_top_right(GameState* g_game_state) {
  if (!g_game_state || !g_game_state->builder) return -1;

  GtkWidget* diff_section =
      GTK_WIDGET(gtk_builder_get_object(g_game_state->builder, "diff_section"));
  GtkWidget* undo_section =
      GTK_WIDGET(gtk_builder_get_object(g_game_state->builder, "undo_section"));

  bool is_single_player = (g_game_state->mode == MODE_1_PLAYER);

  if (undo_section) gtk_widget_set_visible(undo_section, !is_single_player);
  if (diff_section) gtk_widget_set_visible(diff_section, is_single_player);

  // Update difficulty dropdown to show current selection
  if (is_single_player && diff_section) {
    GtkDropDown* diff_dropdown = GTK_DROP_DOWN(
        gtk_builder_get_object(g_game_state->builder, "diff_dropdown"));
    if (diff_dropdown && g_game_state->difficulty >= DIFF_EASY &&
        g_game_state->difficulty <= DIFF_HARD) {
      gtk_drop_down_set_selected(diff_dropdown,
                                 g_game_state->difficulty - DIFF_EASY);
    }
  }

  return 0;
}

/**
 * @brief Check for winner and update stats if game is over.
 *
 * Checks if the game is won or drawn. If so, updates score statistics,
 * displays the appropriate message, and shows the win dialog.
 *
 * @param g_game_state Pointer to the GameState.
 * @return true if game is over, false otherwise.
 */
static bool check_game_over(GameState* g_game_state) {
  if (!g_game_state || !g_game_state->builder) return false;

  Winner winner = check_winner(&g_game_state->board);
  if (winner == ONGOING) return false;

  const char* message = NULL;
  switch (winner) {
    case WIN_X:
      g_game_state->stats.score_X++;
      message = "Player X wins!";
      break;
    case WIN_O:
      g_game_state->stats.score_O++;
      message = "Player O wins!";
      break;
    case DRAW:
      g_game_state->stats.score_tie++;
      message = "It's a draw!";
      break;
    default:
      g_print("Unknown winner state.\n");
      return false;
  }

  update_scoreboard(g_game_state);
  show_win_dialog(g_game_state, message);

  return true;
}

/**
 * @brief Process AI move if it's AI's turn.
 */
static gboolean process_ai_move(gpointer user_data) {
  GameState* g_game_state = (GameState*)user_data;
  if (!g_game_state) return G_SOURCE_REMOVE;

  if (g_game_state->mode == MODE_1_PLAYER &&
      g_game_state->board.current_player == PLAYER_O) {
    Cell ai_move = get_ai_move();
    make_move(&g_game_state->board, &ai_move);
    update_board_display(g_game_state);
    check_game_over(g_game_state);
  }

  return G_SOURCE_REMOVE;
}

/**
 * @brief Navigate to the main menu and reset the game state.
 *
 * Resets the current game and navigates back to the main menu screen.
 *
 * @param stack Pointer to the GtkStack for navigation.
 */
static void to_main_menu(GtkStack* stack) {
  reset_game_state();
  gtk_stack_set_visible_child_name(stack, "main_menu");
}

/**
 * @brief Handle difficulty change from dropdown.
 *
 * Called when the difficulty dropdown selection changes.
 * Updates the game difficulty and resets the game state.
 *
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
      difficulty = DIFF_EASY;
      break;
  }

  set_difficulty(difficulty);
  reset_game_state();
}

/**
 * @brief Handle play again button click.
 *
 * Hides the win dialog and resets the game state to start a new game.
 *
 * @param button Pointer to the GtkButton.
 * @param user_data User data (GtkBuilder pointer).
 */
static void play_again_clicked(GtkButton* button G_GNUC_UNUSED,
                               gpointer user_data) {
  GtkBuilder* builder = GTK_BUILDER(user_data);

  GtkWidget* win_dialog =
      GTK_WIDGET(gtk_builder_get_object(builder, "win_dialog"));
  if (win_dialog) gtk_widget_set_visible(win_dialog, FALSE);

  reset_game_state();
}

/**
 * @brief Handle go back button click.
 *
 * Hides the win dialog and navigates back to the main menu.
 *
 * @param button Pointer to the GtkButton.
 * @param user_data User data (GtkBuilder pointer).
 */
static void go_back_clicked(GtkButton* button G_GNUC_UNUSED,
                            gpointer user_data) {
  GtkBuilder* builder = GTK_BUILDER(user_data);

  GtkWidget* win_dialog =
      GTK_WIDGET(gtk_builder_get_object(builder, "win_dialog"));
  if (win_dialog) gtk_widget_set_visible(win_dialog, FALSE);

  to_main_menu(GTK_STACK(gtk_builder_get_object(builder, "main_stack")));
}

/**
 * @brief Handle undo button click.
 *
 * Undoes the last move on the board and updates the game UI.
 *
 * @param button Pointer to the GtkButton.
 * @param user_data User data (unused).
 */
static void undo_move_handler(GtkButton* button G_GNUC_UNUSED,
                              gpointer user_data G_GNUC_UNUSED) {
  GameState* g_game_state = get_game_state();
  if (undo_move(&g_game_state->board)) {
    update_game_state();
  }
}

/**
 * @brief Handle cell button click.
 *
 * Processes player moves when a cell button is clicked. Converts button index
 * to board coordinates, makes the move, updates the display, and checks for
 * game over.
 *
 * @param button Pointer to the GtkButton.
 * @param user_data User data (cell index as integer pointer).
 */
static void cell_clicked(GtkButton* button G_GNUC_UNUSED, gpointer user_data) {
  GameState* g_game_state = get_game_state();

  int cell_index = GPOINTER_TO_INT(user_data);

  // Check if game is already over or it's AI's turn
  if (check_winner(&g_game_state->board) != ONGOING) return;
  if (g_game_state->mode == MODE_1_PLAYER &&
      g_game_state->board.current_player == PLAYER_O) {
    return;
  }

  cell_index--;  // Convert to 0-based index
  int row = cell_index / SIZE;
  int col = cell_index % SIZE;
  Cell move = {row, col};

  if (!make_move(&g_game_state->board, &move)) return;

  update_board_display(g_game_state);
  if (!check_game_over(g_game_state)) {
    update_game_state();
  }
}

/**
 * @brief Update the game state and UI.
 *
 * Updates the board display, scoreboard, and top-right UI section.
 * Schedules an AI move if in single-player mode and it's the AI's turn.
 *
 * @return 0 on success, -1 on failure.
 */
int update_game_state() {
  GameState* g_game_state = get_game_state();
  if (!g_game_state || !g_game_state->builder) return -1;

  toggle_top_right(g_game_state);
  update_board_display(g_game_state);
  update_scoreboard(g_game_state);

  // Check if it's AI's turn and schedule AI move
  if (g_game_state->mode == MODE_1_PLAYER &&
      g_game_state->board.current_player == PLAYER_O &&
      check_winner(&g_game_state->board) == ONGOING) {
    g_timeout_add(AI_MOVE_DELAY_MS, process_ai_move, g_game_state);
  }

  return 0;
}

/**
 * @brief Initialize the game board UI and connect signals.
 *
 * Sets up the game board UI components including cell buttons (1-9), undo
 * button, difficulty dropdown, and back button. Connects signal handlers for
 * all interactions.
 *
 * @param builder Pointer to the GtkBuilder.
 * @param stack Pointer to the GtkStack.
 * @return 0 on success, -1 on failure.
 */
int game_board(GtkBuilder* builder, GtkStack* stack) {
  GtkButton* back_button =
      GTK_BUTTON(gtk_builder_get_object(builder, "back_button"));
  if (back_button) {
    g_signal_connect_swapped(back_button, "clicked", G_CALLBACK(to_main_menu),
                             stack);
  }

  GtkDropDown* diff_dropdown =
      GTK_DROP_DOWN(gtk_builder_get_object(builder, "diff_dropdown"));
  if (diff_dropdown) {
    g_signal_connect(diff_dropdown, "notify::selected",
                     G_CALLBACK(change_difficulty), stack);
  }

  GtkButton* undo_button =
      GTK_BUTTON(gtk_builder_get_object(builder, "undo_button"));
  if (undo_button) {
    g_signal_connect(undo_button, "clicked", G_CALLBACK(undo_move_handler),
                     stack);
  }

  // Connect the 9 cell buttons
  for (int i = 1; i < 10; i++) {
    char button_name[BUTTON_NAME_SIZE];
    snprintf(button_name, sizeof(button_name), "cell_%d", i);
    GtkButton* cell_button =
        GTK_BUTTON(gtk_builder_get_object(builder, button_name));
    if (cell_button) {
      g_signal_connect(cell_button, "clicked", G_CALLBACK(cell_clicked),
                       GINT_TO_POINTER(i));
    }
  }

  return 0;
}

/**
 * @brief Initialize the win dialog UI and connect signals.
 *
 * Sets up the win/draw dialog with play again and go back buttons.
 * Connects signal handlers for button clicks.
 *
 * @param builder Pointer to the GtkBuilder.
 * @return 0 on success, -1 on failure.
 */
int win_dialog(GtkBuilder* builder) {
  GtkButton* play_again_button =
      GTK_BUTTON(gtk_builder_get_object(builder, "play_again"));
  if (play_again_button) {
    g_signal_connect(play_again_button, "clicked",
                     G_CALLBACK(play_again_clicked), builder);
  }

  GtkButton* go_back_button =
      GTK_BUTTON(gtk_builder_get_object(builder, "go_back"));
  if (go_back_button) {
    g_signal_connect(go_back_button, "clicked", G_CALLBACK(go_back_clicked),
                     builder);
  }

  return 0;
}
