#include "board.h"

#include <stdio.h>

/**
 * @brief Print the Tic-Tac-Toe board in a readable format.
 *
 * Displays 'X', 'O', or the number of the empty cell (1-9).
 * Cells in a row are separated by ' | '.
 *
 * @param board The 3x3 Tic-Tac-Toe board.
 * @param score_board Pointer to the score_board_t struct for displaying scores.
 */
void print_board(const int board[SIZE][SIZE],
                 const score_board_t* score_board) {
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      int cell = board[i][j];
      int cell_num = i * SIZE + j + 1;
      if (cell == EMPTY) {
        printf("%d", cell_num);
      } else {
        printf("%c", cell == X ? 'X' : 'O');
      }
      if (j < SIZE - 1) printf(" | ");
    }
    printf("\n");
    if (i < SIZE - 1) printf("---------\n");
  }
  printf("\n");
  if (score_board != NULL) {
    printf("Player X | Tie | Player O\n");
    printf("   %d     |  %d  |    %d\n\n", score_board->score_X,
           score_board->score_tie, score_board->score_O);
  }
}

/**
 * @brief Find all empty cells on the Tic-Tac-Toe board.
 *
 * An empty cell is one containing 0.
 * Stores the coordinates of empty cells in the provided array.
 *
 * @param board The 3x3 Tic-Tac-Toe board.
 * @param empty_cells Output array to store empty cell positions.
 * @param max_cells Maximum number of cells the array can hold.
 * @return int The number of empty cells found.
 */
int find_empty_cells(const int board[SIZE][SIZE], Cell empty_cells[],
                     int max_cells) {
  int count = 0;
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      if (board[i][j] == EMPTY) {
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
 * @return const int Returns:
 *         - "X" if player X wins,
 *         - "O" if player O wins,
 *         - "Draw" if the board is full with no winner,
 *         - NULL if the game is still ongoing.
 */
const int check_winner(const int board[SIZE][SIZE]) {
  // Check rows and columns
  for (int i = 0; i < SIZE; i++) {
    if (board[i][0] != EMPTY && board[i][0] == board[i][1] &&
        board[i][1] == board[i][2]) {
      return (board[i][0] == X) ? X : O;
    }

    if (board[0][i] != EMPTY && board[0][i] == board[1][i] &&
        board[1][i] == board[2][i]) {
      return (board[0][i] == X) ? X : O;
    }
  }

  // Check diagonals
  if (board[0][0] != EMPTY && board[0][0] == board[1][1] &&
      board[1][1] == board[2][2]) {
    return (board[0][0] == X) ? X : O;
  }

  if (board[0][2] != EMPTY && board[0][2] == board[1][1] &&
      board[1][1] == board[2][0]) {
    return (board[0][2] == X) ? X : O;
  }

  // Check for draw
  Cell empty_cells[SIZE * SIZE];
  if (find_empty_cells(board, empty_cells, SIZE * SIZE) == 0) {
    return EMPTY;  // Indicate draw with EMPTY
  }

  return -1;
}
