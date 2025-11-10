#include <stdbool.h>

#include "../logic/board.h"

#define MINIMAX_MIN_SCORE -10
#define MINIMAX_MAX_SCORE 10
#define MINIMAX_MAX_DEPTH 1
#define MINIMAX_MAX_SAMPLES 2

#define AI_PLAYER O
#define HUMAN_PLAYER X
#define TOTAL_CELLS (SIZE * SIZE)
#define DRAW 0
#define NO_WINNER -1

/**
 * @brief Minimax algorithm with alpha-beta pruning to evaluate the best score
 * for Tic-Tac-Toe.
 *
 * @param board NxN board (array of arrays of int)
 * @param depth current search depth
 * @param is_max true when it's the AI player's turn
 * @param alpha best found option for the AI player
 * @param beta best found option for the human player
 * @param handicap if true, limit depth to MAX_DEPTH to improve performance
 *                 and introduce inaccuracy
 * @return int minimax score for the current board state
 */
int minimax(int board[SIZE][SIZE], int depth, bool is_max, int alpha, int beta,
            bool handicap);

/**
 * @brief Selects a move for the AI player using the minimax algorithm.
 *
 * @param board NxN Tic-Tac-Toe board (array of arrays of int)
 * @param move Output parameter for the best move coordinates
 * @param handicap if true, randomly sample 2 moves to improve performance
 *                 and introduce some inaccuracy
 */
Cell minimax_move(int board[SIZE][SIZE], bool handicap);
