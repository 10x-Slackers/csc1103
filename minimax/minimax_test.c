#include "minimax.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
 * @brief Main function to test the minimax algorithm.
 */
int main(void) {
  // Initialize random seed
  srand(time(NULL));

  int test_board[SIZE][SIZE] = {
      {X, EMPTY, X},
      {X, O, EMPTY},
      {O, EMPTY, EMPTY},
  };

  printf("Test board:\n");
  print_board(test_board, NULL);

  // Test minimax move with handicap
  printf("Minimax move with handicap:\n");
  Cell move = minimax_move(test_board, true);
  if (move.row != -1 && move.col != -1) {
    test_board[move.row][move.col] = AI_PLAYER;
    print_board(test_board, NULL);
    test_board[move.row][move.col] = EMPTY;
  }

  // Test perfect minimax move
  printf("Minimax move without handicap:\n");
  move = minimax_move(test_board, false);
  if (move.row != -1 && move.col != -1) {
    printf("Best move: row=%d, col=%d (cell %d)\n", move.row, move.col,
           move.row * SIZE + move.col + 1);
    test_board[move.row][move.col] = AI_PLAYER;
    print_board(test_board, NULL);
  }

  return 0;
}
