#include "minimax.h"

#include <limits.h>
#include <stdlib.h>

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
static int minimax(const Board* board, Player ai_player, bool handicap,
                   bool is_max, int alpha, int beta, int depth) {
  if (handicap && depth >= HANDICAP_MAX_DEPTH) return 0;

  // Evaluate terminal states
  Winner result = check_winner(board, NULL);
  if (result == DRAW) return 0;
  if (result == WIN_X || result == WIN_O) {
    int score = SIZE * SIZE - depth;
    Player winner = (result == WIN_X) ? PLAYER_X : PLAYER_O;
    return (ai_player == winner) ? score : -score;
  }

  // Find all possible moves on the board
  Cell empty_cells[SIZE * SIZE];
  int num_moves = find_empty_cells(board, empty_cells, SIZE * SIZE);

  if (is_max) {
    // Maximizing player (AI)
    int highest_score = INT_MIN;
    for (int m = 0; m < num_moves; m++) {
      // Make a copy of the board and make a move
      Board temp_board;
      copy_board(board, &temp_board);
      Cell cell = empty_cells[m];
      make_move(&temp_board, &cell);
      int score = minimax(&temp_board, ai_player, handicap, false, alpha, beta,
                          depth + 1);
      // Find the highest score
      if (score > highest_score) highest_score = score;
      // Apply alpha-beta pruning
      if (highest_score > alpha) alpha = highest_score;
      if (beta <= alpha) break;
    }
    return highest_score;
  } else {
    // Minimizing player (Simulated opponent)
    int lowest_score = INT_MAX;
    for (int m = 0; m < num_moves; m++) {
      // Similar to maximizing player but looks for lowest score
      Board temp_board;
      copy_board(board, &temp_board);
      Cell cell = empty_cells[m];
      make_move(&temp_board, &cell);
      int score = minimax(&temp_board, ai_player, handicap, true, alpha, beta,
                          depth + 1);
      if (score < lowest_score) lowest_score = score;
      if (lowest_score < beta) beta = lowest_score;
      if (beta <= alpha) break;
    }
    return lowest_score;
  }
}

Cell minimax_find_move(const Board* board, bool handicap) {
  Player ai_player = board->current_player;
  Cell best_move = {-1, -1};
  int best_score = INT_MIN;

  Cell empty_cells[SIZE * SIZE];
  int num_moves = find_empty_cells(board, empty_cells, SIZE * SIZE);

  Board board_copy;
  copy_board(board, &board_copy);

  // Randomly sample moves using Fisher-Yates shuffle if handicap is enabled.
  if (handicap && num_moves > HANDICAP_MAX_SAMPLES) {
    for (int i = num_moves - 1; i > 0; --i) {
      int j = rand() % (i + 1);
      Cell tmp = empty_cells[i];
      empty_cells[i] = empty_cells[j];
      empty_cells[j] = tmp;
    }
    num_moves = HANDICAP_MAX_SAMPLES;
  }

  // Evaluate moves
  for (int m = 0; m < num_moves; m++) {
    Cell cell = empty_cells[m];
    make_move(&board_copy, &cell);
    int score =
        minimax(&board_copy, ai_player, handicap, false, INT_MIN, INT_MAX, 0);
    undo_move(&board_copy);
    // Update best move if this move has a higher score
    if (score > best_score) {
      best_score = score;
      best_move = cell;
    }
  }

  return best_move;
}
