#ifndef MINIMAX_H
#define MINIMAX_H

#include <stdbool.h>

#include "board.h"

#define HANDICAP_MAX_DEPTH 2
#define HANDICAP_MAX_SAMPLES 4

/**
 * @brief Selects a move for the AI player using the minimax algorithm.
 * @param board Pointer to the Board structure.
 * @param handicap if true, randomly sample 2 moves and limit depth to improve
 *                performance and introduce inaccuracy
 */
Cell minimax_find_move(const Board* board, bool handicap);

#endif  // MINIMAX_H
