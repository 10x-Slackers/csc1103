#include "minimax.h"

static int minimax(Board* board, Player ai_player, bool handicap, bool is_max,
                   int alpha, int beta, int depth) {
  if (handicap && depth >= HANDICAP_MAX_DEPTH) return 0;

  // Evaluate terminal states
  Winner result = check_winner(board);
  if (result == DRAW) return 0;
  if (result == WIN_X || result == WIN_O) {
    int score = SIZE * SIZE - depth;
    Player winner = (result == WIN_X) ? PLAYER_X : PLAYER_O;
    return (ai_player == winner) ? score : -score;
  }

  // Find all possible moves on the board
  Cell empty_cells[SIZE * SIZE];
  int num_moves = find_empty_cells(board, empty_cells);

  if (is_max) {
    // Maximizing player (AI)
    int highest_score = MINIMAX_MIN_SCORE;
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
    int lowest_score = MINIMAX_MAX_SCORE;
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

Cell minimax_find_move(Board* board, bool handicap) {
  Player ai_player = board->current_player;
  Cell best_move = {-1, -1};
  int best_score = MINIMAX_MIN_SCORE;

  Cell empty_cells[SIZE * SIZE];
  int num_moves = find_empty_cells(board, empty_cells);

  // Evaluate all possible moves
  for (int m = 0; m < num_moves; m++) {
    // Make a copy of the board and make a move
    Board temp_board;
    copy_board(board, &temp_board);
    Cell cell = empty_cells[m];
    make_move(&temp_board, &cell);
    // Get the minimax score for this move
    int score = minimax(&temp_board, ai_player, handicap, false,
                        MINIMAX_MIN_SCORE, MINIMAX_MAX_SCORE, 0);
    // Update best move if this move has a higher score
    if (score > best_score) {
      best_score = score;
      best_move = cell;
    }
  }

  return best_move;
}
