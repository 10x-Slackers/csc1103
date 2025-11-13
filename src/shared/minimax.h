#ifndef MINIMAX_H
#define MINIMAX_H

#include <limits.h>
#include <stdbool.h>

#include "board.h"

#define HANDICAP_MAX_DEPTH 1
#define MINIMAX_MIN_SCORE INT_MIN
#define MINIMAX_MAX_SCORE INT_MAX

/**
 * @brief Evaluates the board state using the minimax algorithm with alpha-beta
 *        pruning.
 * @param board Pointer to the Board structure.
 * @param handicap if true, limit depth to improve performance and introduce
 *                 inaccuracy
 * @param is_max true if the current layer is maximizing player, false if
 *               minimizing
 * @param alpha Alpha value for pruning.
 * @param beta Beta value for pruning.
 * @param depth Current depth in the game tree.
 * @return int minimax score for the current board state
 */
static int minimax(Board* board, Player ai_player, bool handicap, bool is_max,
                   int alpha, int beta, int depth);

/**
 * @brief Selects a move for the AI player using the minimax algorithm.
 * @param board Pointer to the Board structure.
 * @param handicap if true, randomly sample 2 moves and limit depth to improve
 *                performance and introduce inaccuracy
 */
Cell minimax_find_move(Board* board, bool handicap);

#endif  // MINIMAX_H
