#ifndef BOARD_H
#define BOARD_H

#define SIZE 3

typedef struct {
  int row;
  int col;
} Cell;

void print_board(int board[SIZE][SIZE]);
int find_empty_cells(int board[SIZE][SIZE], Cell empty_cells[], int max_cells);
const int check_winner(int board[SIZE][SIZE]);

#endif  // BOARD_H
