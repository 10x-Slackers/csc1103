/**
 * @file minimax.c
 * @brief Minimax algorithm implementation for Tic-Tac-Toe in C
 */

#include "minimax.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

int minimax(int board[SIZE][SIZE], int depth, bool is_max, int alpha, int beta,
            bool handicap) {
  if (handicap && depth >= MINIMAX_MAX_DEPTH) {
    return 0;
  }

  switch (check_winner(board)) {
    case AI_PLAYER:
      return TOTAL_CELLS - depth;
    case HUMAN_PLAYER:
      return -TOTAL_CELLS + depth;
    case DRAW:
      return 0;
    default:
      break;
  }

  // Find all possible moves (empty cells) on the board
  Cell moves[TOTAL_CELLS];
  int num_moves = find_empty_cells(board, moves, TOTAL_CELLS);

  if (is_max) {
    // Maximizing player (AI)
    int highest_score = MINIMAX_MIN_SCORE;
    for (int m = 0; m < num_moves; m++) {
      int i = moves[m].row;
      int j = moves[m].col;
      // Make a move and call minimax recursively, then undo the move
      board[i][j] = AI_PLAYER;
      int score = minimax(board, depth + 1, false, alpha, beta, handicap);
      board[i][j] = EMPTY;

      if (score > highest_score) highest_score = score;
      if (highest_score > alpha) alpha = highest_score;
      // Prune if current value is already better than beta
      if (beta <= alpha) break;
    }
    return highest_score;
  } else {
    // Minimizing player (Human)
    int lowest_score = MINIMAX_MAX_SCORE;
    // Try all possible moves and choose the one with the lowest score
    for (int m = 0; m < num_moves; m++) {
      int i = moves[m].row;
      int j = moves[m].col;
      // Make a move and call minimax recursively, then undo the move
      board[i][j] = HUMAN_PLAYER;
      int score = minimax(board, depth + 1, true, alpha, beta, handicap);
      board[i][j] = EMPTY;

      // Update to the lowest score found so far
      if (score < lowest_score) lowest_score = score;
      if (lowest_score < beta) beta = lowest_score;
      // Prune if current value is already better than alpha
      if (beta <= alpha) break;
    }
    return lowest_score;
  }
}

/**
 * @brief Selects a move for the AI player using the minimax algorithm.
 *
 * @param board NxN Tic-Tac-Toe board (array of arrays of int)
 * @param move Output parameter for the best move coordinates
 * @param handicap if true, randomly sample 2 moves to improve performance
 *                 and introduce some inaccuracy
 */
Cell minimax_move(int board[SIZE][SIZE], bool handicap) {
  int best_score = MINIMAX_MIN_SCORE;
  Cell best_move = {-1, -1};

  // Find all possible moves (empty cells) on the board
  Cell moves[TOTAL_CELLS];
  int num_moves = find_empty_cells(board, moves, TOTAL_CELLS);
  if (num_moves == 0) {
    fprintf(stderr, "Error: No empty cells available for a move.\n");
    return best_move;
  }

  if (num_moves == 1) {
    // Return the only available move
    best_move = moves[0];
    return best_move;
  }

  // If handicap is enabled, randomly sample MAX_SAMPLES moves
  int moves_to_check = num_moves;
  if (handicap && num_moves > MINIMAX_MAX_SAMPLES) {
    moves_to_check = MINIMAX_MAX_SAMPLES;
    for (int i = 0; i < MINIMAX_MAX_SAMPLES; i++) {
      // Fisher-Yates shuffle to randomize the first MAX_SAMPLES moves
      int j = i + rand() % (num_moves - i);
      Cell temp = moves[i];
      moves[i] = moves[j];
      moves[j] = temp;
    }
  }

  for (int m = 0; m < moves_to_check; m++) {
    int i = moves[m].row;
    int j = moves[m].col;
    // Make a move and call minimax, then undo the move
    board[i][j] = AI_PLAYER;
    int score = minimax(board, 0, false, MINIMAX_MIN_SCORE, MINIMAX_MAX_SCORE,
                        handicap);
    board[i][j] = EMPTY;
    // Update to the best score and move found so far
    if (score > best_score) {
      best_score = score;
      best_move = moves[m];
    }
  }

  return best_move;
}
