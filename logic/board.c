#include "board.h"

#include <stdio.h>

/**
 * @brief Print the Tic-Tac-Toe board in a readable format.
 *
 * Displays 'X', 'O', or the number of the empty cell (1-9).
 * Cells in a row are separated by ' | '.
 *
 * @param board The 3x3 Tic-Tac-Toe board.
 */
void print_board(int board[SIZE][SIZE]) {
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      int cell = board[i][j];
      printf("%d", cell);
      if (j < SIZE - 1) printf(" | ");
    }
    printf("\n");
    if (i < SIZE - 1) printf("---------\n");
  }
  printf("\n");
}

/**
 * @brief Find all empty cells on the Tic-Tac-Toe board.
 *
 * An empty cell is one containing a space (' ').
 * Stores the coordinates of empty cells in the provided array.
 *
 * @param board The 3x3 Tic-Tac-Toe board.
 * @param empty_cells Output array to store empty cell positions.
 * @param max_cells Maximum number of cells the array can hold.
 * @return int The number of empty cells found.
 */
int find_empty_cells(int const board[SIZE][SIZE], Cell empty_cells[],
                     int max_cells) {
  int count = 0;
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      if (board[i][j] == 0) {
        if (count < max_cells) {
          empty_cells[count++] = (Cell){i, j};
        }
      }
    }
  }
  return count;
}

/**
 * @brief Check if there is a winner or if the game is a draw.
 *
 * Checks all rows, columns, and diagonals for a winner.
 * If no winner and no empty cells remain, it's a draw.
 *
 * @param board The 3x3 Tic-Tac-Toe board.
 * @return const char* Returns:
 *         - "X" if player X wins,
 *         - "O" if player O wins,
 *         - "Draw" if the board is full with no winner,
 *         - NULL if the game is still ongoing.
 */
const int check_winner(int board[SIZE][SIZE]) {
  // Check rows and columns
  for (int i = 0; i < SIZE; i++) {
    if (board[i][0] != 0 && board[i][0] == board[i][1] &&
        board[i][1] == board[i][2]) {
      return (board[i][0] == 1) ? 1 : 2;
    }

    if (board[0][i] != 0 && board[0][i] == board[1][i] &&
        board[1][i] == board[2][i]) {
      return (board[0][i] == 1) ? 1 : 2;
    }
  }

  // Check diagonals
  if (board[0][0] != 0 && board[0][0] == board[1][1] &&
      board[1][1] == board[2][2]) {
    return (board[0][0] == 1) ? 1 : 2;
  }

  if (board[0][2] != 0 && board[0][2] == board[1][1] &&
      board[1][1] == board[2][0]) {
    return (board[0][2] == 1) ? 1 : 2;
  }

  // Check for draw
  Cell empty_cells[SIZE * SIZE];
  if (find_empty_cells(board, empty_cells, SIZE * SIZE) == 0) {
    return 0;
  }

  return -1;
}
