#ifndef BOARD_GUI_H
#define BOARD_GUI_H

#include "game.h"

#define AI_MOVE_DELAY_MS 500  // Buffer time for AI move processing
#define SCORE_BUFFER_SIZE 6   // Buffer size for score text formatting
#define BUTTON_NAME_SIZE 20   // Buffer size for button name formatting

/**
 * @brief Update the game state and UI.
 *
 * Updates the board display, scoreboard, and top-right UI section.
 * Schedules an AI move if in singleplayer mode and it's the AI's turn.
 *
 * @return 0 on success, -1 on failure.
 */
int update_game_state();

/**
 * @brief Initialize the game board UI and connect signals.
 *
 * Sets up the game board UI components including cell buttons, undo button, and
 * difficulty dropdown.
 *
 * @param builder Pointer to the GtkBuilder instance.
 * @param stack Pointer to the GtkStack for navigation.
 */
void game_board(GtkBuilder* builder, GtkStack* stack);

/**
 * @brief Initialize the win dialog UI and connect signals.
 *
 * Sets up the win/draw dialog with play again and go back buttons.
 *
 * @param builder Pointer to the GtkBuilder.
 */
void win_dialog(GtkBuilder* builder);

#endif  // BOARD_GUI_H
