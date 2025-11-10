/*
 * File containing the game Tic Tac Toe itself.
 * Displays the GUI window allowing players to play the game.
 *
 * Author: Jerome
 * Date: 2025-11-10
 * Version: 1
 */
#include "tic_tac_toe_grid.h"

#include <gtk/gtk.h>
#include <string.h>
#include <time.h>

#include "..\logic_files\board.h"
#include "main_menu.h"

// ============================================================================
// CONSTANTS
// ============================================================================

#define TTT_GRID_SIZE 150
#define SPACING 10
#define MARGIN 30
#define PADDING 20
#define WINDOW_SIZE ((TTT_GRID_SIZE * 3) + (SPACING * 2) + (MARGIN * 2))
#define BLINK_CYCLES 6
#define BLINK_INTERVAL_MS 500
#define TTT_GRID 3
#define TTT_BUTTONS 9
#define SELECTED_BUTTONS 3
#define SCORE_STR_LEN 10

// ============================================================================
// GAME STATE
// ============================================================================

/**
 * @struct GameState
 * @brief Holds all game state information
 *
 * Maintains the current state of the game including player turns,
 * board state, scores, and UI widget references.
 */
typedef struct {
  const char* current_player;           /**< Current player ("X" or "O") */
  char* game_mode;                      /**< Game mode ("1P" or "2P") */
  char board_state[TTT_GRID][TTT_GRID]; /**< 3x3 board state */
  GtkWidget* turn_label;                /**< Label showing current turn */
  GtkWidget* score_x_label;             /**< Score label for Player X */
  GtkWidget* score_o_label;             /**< Score label for Player O */
  GtkWidget* score_tie_label;           /**< Score label for ties */
  GtkWidget* buttons[TTT_BUTTONS];      /**< Array of grid buttons */
  gboolean game_ended; /**< Flag indicating if game has ended */
  int score_x;         /**< Player X score */
  int score_o;         /**< Player O score */
  int score_tie;       /**< Tie game count */
} GameState;

/** @brief Global game state instance */
static GameState game = {
    .current_player = "X",
    .game_mode = "1P",
    .board_state = {{' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '}},
    .turn_label = NULL,
    .score_x_label = NULL,
    .score_o_label = NULL,
    .score_tie_label = NULL,
    .buttons = {NULL},
    .game_ended = FALSE,
    .score_x = 0,
    .score_o = 0,
    .score_tie = 0};

// ============================================================================
// BLINK ANIMATION DATA
// ============================================================================

/**
 * @struct BlinkData
 * @brief Data structure for managing blink animations
 *
 * Tracks the buttons being animated and the animation state.
 */
typedef struct {
  GtkWidget* buttons[SELECTED_BUTTONS]; /**< Buttons to animate */
  int count;                            /**< Current blink cycle count */
  guint timeout_id;                     /**< GTK timeout ID */
} BlinkData;

/** @brief Pointer to currently active blink animation */
static BlinkData* active_blink_data = NULL;

/**
 * @struct ScoreBlinkData
 * @brief Data structure for scoreboard blink animations
 */
typedef struct {
  GtkWidget* label; /**< Score label to animate */
  int count;        /**< Current blink cycle count */
} ScoreBlinkData;

// ============================================================================
// FUNCTION PROTOTYPES
// ============================================================================

static void reset_game(void);
static void reset_board_only(void);
static void apply_css(void);

static void create_turn_label(GtkBox* vbox);
static void create_game_grid(GtkGrid* grid, GtkBox* vbox);
static void create_scoreboard(GtkBox* vbox);
static void create_control_buttons(GtkBox* vbox, GtkApplication* app);

static void update_turn_label(void);
static void update_scoreboard(const char* winner);

static void on_button_clicked(GtkButton* button, gpointer user_data);
static void on_back_clicked(GtkButton* button, GtkApplication* app);

static void start_blink_animation(GtkWidget** selected_buttons);
static gboolean toggle_blink_timeout(gpointer user_data);
static void get_random_filled_buttons(GtkWidget** output, int count);

static void start_scoreboard_blink(GtkWidget* label);
static gboolean toggle_scoreboard_blink(gpointer user_data);

// ============================================================================
// GAME LIFECYCLE FUNCTIONS
// ============================================================================

/**
 * @brief Launches the main game window
 *
 * Initializes the game state, creates the UI, and displays the game window.
 *
 * @param app The GTK application instance
 * @param mode Game mode string ("1P" for single player, "2P" for two player)
 */
void launch_game(GtkApplication* app, char* mode) {
  game.game_mode = mode;

  // Initialize all widget pointers to NULL
  game.turn_label = NULL;
  game.score_x_label = NULL;
  game.score_o_label = NULL;
  game.score_tie_label = NULL;
  for (int i = 0; i < TTT_BUTTONS; i++) {
    game.buttons[i] = NULL;
  }

  // Cancel any active blink animation from previous game
  if (active_blink_data != NULL) {
    if (active_blink_data->timeout_id > 0) {
      g_source_remove(active_blink_data->timeout_id);
    }
    g_free(active_blink_data);
    active_blink_data = NULL;
  }

  reset_game();
  apply_css();

  GtkWidget* window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "Tic Tac Toe Grid");
  gtk_window_set_default_size(GTK_WINDOW(window), WINDOW_SIZE, WINDOW_SIZE);
  gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

  GtkBox* vbox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 20));
  GtkGrid* grid = GTK_GRID(gtk_grid_new());

  GtkWidget* top_spacer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
  gtk_widget_set_vexpand(top_spacer, TRUE);
  gtk_box_append(vbox, top_spacer);

  create_turn_label(vbox);
  create_game_grid(grid, vbox);
  create_scoreboard(vbox);
  create_control_buttons(vbox, app);

  gtk_window_set_child(GTK_WINDOW(window), GTK_WIDGET(vbox));
  gtk_window_present(GTK_WINDOW(window));
}

/**
 * @brief Resets the entire game including scores
 *
 * Clears the board and resets all scores to zero.
 */
static void reset_game(void) {
  reset_board_only();
  game.score_x = 0;
  game.score_o = 0;
  game.score_tie = 0;
}

/**
 * @brief Resets only the game board without affecting scores
 *
 * Clears all cells, cancels active animations, and prepares for a new round.
 */
static void reset_board_only(void) {
  // Cancel active blink animation if it exists
  if (active_blink_data != NULL) {
    if (active_blink_data->timeout_id > 0) {
      g_source_remove(active_blink_data->timeout_id);
    }

    // Remove blink CSS class from all animated buttons
    for (int i = 0; i < SELECTED_BUTTONS; i++) {
      if (active_blink_data->buttons[i] != NULL &&
          GTK_IS_WIDGET(active_blink_data->buttons[i])) {
        gtk_widget_remove_css_class(active_blink_data->buttons[i], "blink");
        gtk_widget_queue_draw(active_blink_data->buttons[i]);
      }
    }

    g_free(active_blink_data);
    active_blink_data = NULL;
  }

  // Clear board state
  for (int i = 0; i < TTT_GRID; i++) {
    for (int j = 0; j < TTT_GRID; j++) {
      game.board_state[i][j] = ' ';
    }
  }

  game.current_player = "X";
  game.game_ended = FALSE;

  // Reset all button labels and styles
  for (int i = 0; i < TTT_BUTTONS; i++) {
    if (game.buttons[i] != NULL && GTK_IS_WIDGET(game.buttons[i])) {
      gtk_button_set_label(GTK_BUTTON(game.buttons[i]), "");
      gtk_widget_remove_css_class(game.buttons[i], "x-style");
      gtk_widget_remove_css_class(game.buttons[i], "o-style");
      gtk_widget_remove_css_class(game.buttons[i], "blink");
    }
  }

  update_turn_label();
}

// ============================================================================
// UI CREATION FUNCTIONS
// ============================================================================

/**
 * @brief Creates the turn indicator label
 *
 * Displays which player's turn it is (Player X, Player O, or Computer).
 *
 * @param vbox The vertical box container to append the label to
 */
static void create_turn_label(GtkBox* vbox) {
  GtkWidget* label_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 40);
  gtk_widget_set_halign(label_box, GTK_ALIGN_CENTER);

  game.turn_label = gtk_label_new("Player X's Turn");
  gtk_widget_add_css_class(game.turn_label, "turn-x");

  gtk_box_append(GTK_BOX(label_box), game.turn_label);
  gtk_box_append(vbox, label_box);
}

/**
 * @brief Creates the 3x3 game grid
 *
 * Initializes all nine game buttons with proper styling and click handlers.
 *
 * @param grid The grid container for buttons
 * @param vbox The vertical box container to append the grid to
 */
static void create_game_grid(GtkGrid* grid, GtkBox* vbox) {
  gtk_grid_set_row_spacing(grid, SPACING);
  gtk_grid_set_column_spacing(grid, SPACING);
  gtk_widget_set_halign(GTK_WIDGET(grid), GTK_ALIGN_CENTER);

  for (int i = 0; i < TTT_BUTTONS; i++) {
    game.buttons[i] = gtk_button_new();
    gtk_widget_set_size_request(game.buttons[i], TTT_GRID_SIZE, TTT_GRID_SIZE);
    gtk_widget_add_css_class(game.buttons[i], "grid-button");
    gtk_widget_set_halign(game.buttons[i], GTK_ALIGN_CENTER);
    gtk_widget_set_valign(game.buttons[i], GTK_ALIGN_CENTER);

    g_signal_connect(game.buttons[i], "clicked", G_CALLBACK(on_button_clicked),
                     GINT_TO_POINTER(i));

    gtk_grid_attach(grid, game.buttons[i], i % TTT_GRID, i / TTT_GRID, 1, 1);
  }

  gtk_box_append(vbox, GTK_WIDGET(grid));
}

/**
 * @brief Creates the scoreboard display
 *
 * Shows scores for Player X, Player O (or Computer), and Ties.
 *
 * @param vbox The vertical box container to append the scoreboard to
 */
static void create_scoreboard(GtkBox* vbox) {
  GtkWidget* scoreboard_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 40);
  gtk_widget_set_halign(scoreboard_box, GTK_ALIGN_CENTER);

  const char* player_o_label =
      (strcmp(game.game_mode, "1P") == 0) ? "Computer" : "Player O";

  struct {
    const char* label_text;
    const char* css_class;
    GtkWidget** score_label_ptr;
  } columns[] = {{"Player X", "score-x", &game.score_x_label},
                 {"Tie", "score-tie", &game.score_tie_label},
                 {player_o_label, "score-o", &game.score_o_label}};

  for (int i = 0; i < 3; i++) {
    GtkWidget* column_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    GtkWidget* label = gtk_label_new(columns[i].label_text);
    gtk_widget_add_css_class(label, "scoreboard-label");
    gtk_widget_add_css_class(label, columns[i].css_class);

    *columns[i].score_label_ptr = gtk_label_new("0");
    gtk_widget_add_css_class(*columns[i].score_label_ptr, "score-value");
    gtk_widget_add_css_class(*columns[i].score_label_ptr, columns[i].css_class);

    gtk_box_append(GTK_BOX(column_box), label);
    gtk_box_append(GTK_BOX(column_box), *columns[i].score_label_ptr);
    gtk_box_append(GTK_BOX(scoreboard_box), column_box);
  }

  gtk_box_append(vbox, scoreboard_box);
}

/**
 * @brief Creates control buttons (Back to Main Menu)
 *
 * @param vbox The vertical box container to append buttons to
 * @param app The GTK application instance
 */
static void create_control_buttons(GtkBox* vbox, GtkApplication* app) {
  GtkWidget* button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, PADDING);
  gtk_widget_set_halign(button_box, GTK_ALIGN_CENTER);

  GtkWidget* back_button = gtk_button_new_with_label("Back to Main Menu");
  g_signal_connect(back_button, "clicked", G_CALLBACK(on_back_clicked), app);

  gtk_box_append(GTK_BOX(button_box), back_button);
  gtk_box_append(vbox, button_box);

  GtkWidget* bottom_spacer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 30);
  gtk_widget_set_vexpand(bottom_spacer, TRUE);
  gtk_box_append(vbox, bottom_spacer);
}

// ============================================================================
// UI UPDATE FUNCTIONS
// ============================================================================

/**
 * @brief Updates the turn label to show the current player
 *
 * Changes text and styling based on whose turn it is.
 */
static void update_turn_label(void) {
  if (!game.turn_label) return;

  gtk_widget_remove_css_class(game.turn_label, "turn-x");
  gtk_widget_remove_css_class(game.turn_label, "turn-o");

  const char* label_text;
  const char* css_class;

  if (game.current_player[0] == 'X') {
    label_text = "Player X's Turn";
    css_class = "turn-x";
  } else {
    label_text = (strcmp(game.game_mode, "1P") == 0) ? "Computer's Turn"
                                                     : "Player O's Turn";
    css_class = "turn-o";
  }

  gtk_label_set_text(GTK_LABEL(game.turn_label), label_text);
  gtk_widget_add_css_class(game.turn_label, css_class);
}

/**
 * @brief Updates the scoreboard with the game result
 *
 * Increments the appropriate score and triggers blink animation.
 *
 * @param winner The winner string ("X", "O", or "Tie")
 */
static void update_scoreboard(const char* winner) {
  GtkWidget* label_to_blink = NULL;
  char new_score[SCORE_STR_LEN];

  if (strcmp(winner, "X") == 0) {
    game.score_x++;
    snprintf(new_score, sizeof(new_score), "%d", game.score_x);
    gtk_label_set_text(GTK_LABEL(game.score_x_label), new_score);
    label_to_blink = game.score_x_label;
  } else if (strcmp(winner, "O") == 0) {
    game.score_o++;
    snprintf(new_score, sizeof(new_score), "%d", game.score_o);
    gtk_label_set_text(GTK_LABEL(game.score_o_label), new_score);
    label_to_blink = game.score_o_label;
  } else if (strcmp(winner, "Tie") == 0) {
    game.score_tie++;
    snprintf(new_score, sizeof(new_score), "%d", game.score_tie);
    gtk_label_set_text(GTK_LABEL(game.score_tie_label), new_score);
    label_to_blink = game.score_tie_label;
  }

  if (label_to_blink != NULL) {
    start_scoreboard_blink(label_to_blink);
  }
}

// ============================================================================
// EVENT HANDLERS
// ============================================================================

/**
 * @brief Handles grid button clicks
 *
 * Places the current player's mark, checks for win conditions,
 * and switches turns.
 *
 * @param button The clicked button
 * @param user_data Button index as gpointer
 */
static void on_button_clicked(GtkButton* button, gpointer user_data) {
  int index = GPOINTER_TO_INT(user_data);
  int row = index / TTT_GRID;
  int col = index % TTT_GRID;

  // If game ended, clicking any cell starts new game
  if (game.game_ended) {
    reset_board_only();
    return;
  }

  // Ignore clicks on occupied cells
  if (game.board_state[row][col] != ' ') return;

  // Place player's mark
  gtk_button_set_label(button, game.current_player);
  const char* css_class =
      (game.current_player[0] == 'X') ? "x-style" : "o-style";
  gtk_widget_add_css_class(GTK_WIDGET(button), css_class);

  game.board_state[row][col] = game.current_player[0];

  // Check for winner
  int winning_positions[3];
  const char* winner = check_winner(game.board_state, winning_positions);

  if (winner != NULL) {
    game.game_ended = TRUE;

    if (strcmp(winner, "Tie") == 0) {
      printf("It's a Tie!\n");
      gtk_label_set_text(GTK_LABEL(game.turn_label),
                         "It's a Tie! Click any cell to start a new game");
      gtk_widget_remove_css_class(game.turn_label, "turn-x");
      gtk_widget_remove_css_class(game.turn_label, "turn-o");
      gtk_widget_add_css_class(game.turn_label, "game-over");
      update_scoreboard("Tie");
    } else {
      // Handle X or O win
      const char* winner_text;
      if (strcmp(winner, "X") == 0) {
        printf("Player X wins!\n");
        winner_text = "Player X Wins! Click any cell to start a new game";
      } else {
        printf("Player O wins!\n");
        winner_text = (strcmp(game.game_mode, "1P") == 0)
                          ? "Computer Wins! Click any cell to play again"
                          : "Player O Wins! Click any cell to play again";
      }

      gtk_label_set_text(GTK_LABEL(game.turn_label), winner_text);
      gtk_widget_remove_css_class(game.turn_label, "turn-x");
      gtk_widget_remove_css_class(game.turn_label, "turn-o");
      gtk_widget_add_css_class(game.turn_label, "game-over");

      // Animate winning buttons
      GtkWidget* winning_buttons[3];
      for (int i = 0; i < 3; i++) {
        winning_buttons[i] = game.buttons[winning_positions[i]];
      }
      start_blink_animation(winning_buttons);
      update_scoreboard(winner);
    }
    return;
  }

  // Switch turns
  game.current_player = (game.current_player[0] == 'X') ? "O" : "X";
  update_turn_label();
}

/**
 * @brief Handles the "Back to Main Menu" button click
 *
 * Closes the game window and returns to the main menu.
 *
 * @param button The clicked button
 * @param app The GTK application instance
 */
static void on_back_clicked(GtkButton* button, GtkApplication* app) {
  GtkWindow* window = GTK_WINDOW(gtk_widget_get_root(GTK_WIDGET(button)));
  gtk_window_close(window);
  main_menu(app);
}

// ============================================================================
// BLINK ANIMATION
// ============================================================================

/**
 * @brief Gets random filled buttons from the game board
 *
 * @param output Array to store selected button pointers
 * @param count Number of buttons to select
 */
static void get_random_filled_buttons(GtkWidget** output, int count) {
  int filled_rows[TTT_BUTTONS];
  int filled_cols[TTT_BUTTONS];
  int filled_count = 0;

  // Collect all filled positions
  for (int i = 0; i < TTT_GRID; i++) {
    for (int j = 0; j < TTT_GRID; j++) {
      if (game.board_state[i][j] != ' ') {
        filled_rows[filled_count] = i;
        filled_cols[filled_count] = j;
        filled_count++;
      }
    }
  }

  if (filled_count < count) {
    printf("Not enough filled buttons (need %d, have %d)\n", count,
           filled_count);
    output[0] = NULL;
    return;
  }

  // Randomly select buttons without replacement
  srand(time(NULL));
  for (int i = 0; i < count; i++) {
    int rand_index = rand() % filled_count;
    int row = filled_rows[rand_index];
    int col = filled_cols[rand_index];
    output[i] = game.buttons[row * 3 + col];

    // Remove selected button from pool
    filled_rows[rand_index] = filled_rows[--filled_count];
    filled_cols[rand_index] = filled_cols[filled_count];
  }
}

/**
 * @brief Starts the blink animation for winning buttons
 *
 * Cancels any existing animation and starts a new one.
 *
 * @param selected_buttons Array of buttons to animate
 */
static void start_blink_animation(GtkWidget** selected_buttons) {
  // Cancel existing animation
  if (active_blink_data != NULL) {
    if (active_blink_data->timeout_id > 0) {
      g_source_remove(active_blink_data->timeout_id);
    }
    for (int i = 0; i < SELECTED_BUTTONS; i++) {
      if (GTK_IS_WIDGET(active_blink_data->buttons[i])) {
        gtk_widget_remove_css_class(active_blink_data->buttons[i], "blink");
      }
    }
    g_free(active_blink_data);
  }

  // Create new animation data
  BlinkData* data = g_malloc(sizeof(BlinkData));
  active_blink_data = data;

  for (int i = 0; i < SELECTED_BUTTONS; i++) {
    data->buttons[i] = selected_buttons[i];
  }
  data->count = 0;
  data->timeout_id = 0;

  // Apply initial blink style
  for (int i = 0; i < SELECTED_BUTTONS; i++) {
    gtk_widget_add_css_class(data->buttons[i], "blink");
    gtk_widget_queue_draw(data->buttons[i]);
  }
  data->count = 1;

  // Start timeout for animation
  data->timeout_id =
      g_timeout_add(BLINK_INTERVAL_MS, toggle_blink_timeout, data);
}

/**
 * @brief Timeout callback for blink animation
 *
 * Toggles the blink CSS class on and off for the animation effect.
 *
 * @param user_data Pointer to BlinkData structure
 * @return G_SOURCE_REMOVE when animation completes, G_SOURCE_CONTINUE otherwise
 */
static gboolean toggle_blink_timeout(gpointer user_data) {
  BlinkData* data = (BlinkData*)user_data;

  // Validate all widgets still exist
  for (int i = 0; i < SELECTED_BUTTONS; i++) {
    if (!GTK_IS_WIDGET(data->buttons[i])) {
      if (active_blink_data == data) {
        active_blink_data = NULL;
      }
      g_free(data);
      return G_SOURCE_REMOVE;
    }
  }

  // Toggle blink class
  for (int i = 0; i < SELECTED_BUTTONS; i++) {
    if (data->count % 2 == 0) {
      gtk_widget_add_css_class(data->buttons[i], "blink");
    } else {
      gtk_widget_remove_css_class(data->buttons[i], "blink");
    }
    gtk_widget_queue_draw(data->buttons[i]);
  }

  data->count++;

  // Stop after BLINK_CYCLES
  if (data->count >= BLINK_CYCLES) {
    for (int i = 0; i < SELECTED_BUTTONS; i++) {
      gtk_widget_remove_css_class(data->buttons[i], "blink");
      gtk_widget_queue_draw(data->buttons[i]);
    }

    if (active_blink_data == data) {
      active_blink_data = NULL;
    }

    g_free(data);
    return G_SOURCE_REMOVE;
  }

  return G_SOURCE_CONTINUE;
}

// ============================================================================
// SCOREBOARD BLINK ANIMATION
// ============================================================================

/**
 * @brief Starts a blink animation for a scoreboard label
 *
 * @param label The label widget to animate
 */
static void start_scoreboard_blink(GtkWidget* label) {
  ScoreBlinkData* data = g_malloc(sizeof(ScoreBlinkData));
  data->label = label;
  data->count = 0;

  gtk_widget_add_css_class(label, "score-blink");
  gtk_widget_queue_draw(label);
  data->count = 1;

  g_timeout_add(BLINK_INTERVAL_MS, toggle_scoreboard_blink, data);
}

/**
 * @brief Timeout callback for scoreboard blink animation
 *
 * @param user_data Pointer to ScoreBlinkData structure
 * @return G_SOURCE_REMOVE when animation completes, G_SOURCE_CONTINUE otherwise
 */
static gboolean toggle_scoreboard_blink(gpointer user_data) {
  ScoreBlinkData* data = (ScoreBlinkData*)user_data;

  if (!GTK_IS_WIDGET(data->label)) {
    g_free(data);
    return G_SOURCE_REMOVE;
  }

  // Toggle blink class
  if (data->count % 2 == 0) {
    gtk_widget_add_css_class(data->label, "score-blink");
  } else {
    gtk_widget_remove_css_class(data->label, "score-blink");
  }
  gtk_widget_queue_draw(data->label);

  data->count++;

  // Stop after BLINK_CYCLES
  if (data->count >= BLINK_CYCLES) {
    gtk_widget_remove_css_class(data->label, "score-blink");
    gtk_widget_queue_draw(data->label);
    g_free(data);
    return G_SOURCE_REMOVE;
  }

  return G_SOURCE_CONTINUE;
}

// ============================================================================
// CSS STYLING
// ============================================================================

/**
 * @brief Applies CSS styling to the application
 *
 * * Loads custom CSS for buttons, labels, animations, and scoreboard styling.
 */
static void apply_css(void) {
  GtkCssProvider* provider = gtk_css_provider_new();

  const gchar* css =
      "/* X and O symbols in grid */\n"
      ".x-style {\n"
      "   font-size: 50px;\n"
      "   color: red;\n"
      "   font-weight: bold;\n"
      "}\n"
      ".o-style {\n"
      "   font-size: 50px;\n"
      "   color: blue;\n"
      "   font-weight: bold;\n"
      "}\n"
      "\n"
      "/* Turn label */\n"
      ".turn-x {\n"
      "   font-size: 24px;\n"
      "   color: red;\n"
      "   font-weight: bold;\n"
      "}\n"
      ".turn-o {\n"
      "   font-size: 24px;\n"
      "   color: blue;\n"
      "   font-weight: bold;\n"
      "}\n"
      ".game-over {\n"
      "   font-size: 20px;\n"
      "   color: green;\n"
      "   font-weight: bold;\n"
      "}\n"
      "\n"
      "/* Grid buttons */\n"
      ".grid-button {\n"
      "   background-color: white;\n"
      "   border: 2px solid #ccc;\n"
      "   border-radius: 8px;\n"
      "   padding: 0;\n"
      "   transition: background-color 0.25s ease-in-out, border 0.25s "
      "ease-in-out;\n"
      "}\n"
      ".grid-button:hover {\n"
      "   border-color: #888;\n"
      "   background-color: #f2f2f2;\n"
      "}\n"
      ".grid-button.blink {\n"
      "   background-color: yellow;\n"
      "   border: 2px solid orange;\n"
      "   box-shadow: 0 0 15px 5px yellow;\n"
      "   transition: background-color 0.25s ease-in-out, box-shadow 0.25s "
      "ease-in-out;\n"
      "}\n"
      "\n"
      "/* Remove GTK default shadows */\n"
      "button {\n"
      "   background-image: none;\n"
      "   box-shadow: none;\n"
      "}\n"
      "\n"
      "/* Scoreboard */\n"
      ".scoreboard-label {\n"
      "   font-size: 18px;\n"
      "   font-weight: bold;\n"
      "}\n"
      ".score-value {\n"
      "   font-size: 24px;\n"
      "   font-weight: bold;\n"
      "}\n"
      ".score-x { color: red; }\n"
      ".score-o { color: blue; }\n"
      ".score-tie { color: black; }\n"
      ".score-blink {\n"
      "   transform: scale(1.3);\n"
      "   font-weight: 900;\n"
      "   text-shadow: 0 0 10px currentColor;\n"
      "   transition: all 0.25s ease-in-out;\n"
      "}\n";

  gtk_css_provider_load_from_string(provider, css);
  GdkDisplay* display = gdk_display_get_default();
  gtk_style_context_add_provider_for_display(
      display, GTK_STYLE_PROVIDER(provider),
      GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}
