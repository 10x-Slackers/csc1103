#ifndef BOARD_GUI_H
#define BOARD_GUI_H

#include "game.h"

/**
 * @brief Update the game state and UI.
 *
 * Updates the board display, scoreboard, and handles AI moves if in
 * single-player mode.
 *
 * @return 0 on success, -1 on failure.
 */
int update_game_state();

/**
 * @brief Initialize the game board UI and connect signals.
 *
 * Sets up the game board UI components including cell buttons, undo button, and
 * difficulty dropdown. Connects signal handlers for player moves and UI
 * interactions.
 *
 * @param builder Pointer to the GtkBuilder instance.
 * @param stack Pointer to the GtkStack for navigation.
 * @return 0 on success, -1 on failure.
 */
int game_board(GtkBuilder* builder, GtkStack* stack);

/**
 * @brief Initialize the win dialog UI and connect signals.
 *
 * Sets up the win/draw dialog with play again and go back buttons.
 * Connects signal handlers for button clicks.
 *
 * @param builder Pointer to the GtkBuilder instance.
 * @return 0 on success, -1 on failure.
 */
int win_dialog(GtkBuilder* builder);

#endif  // BOARD_GUI_H
