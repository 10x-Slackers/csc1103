#ifndef BOARD_H
#define BOARD_H

#define SIZE 3
#define X 1
#define O 2
#define EMPTY 0

typedef struct {
  int row;
  int col;
} Cell;

void print_board(const int board[SIZE][SIZE]);
int find_empty_cells(const int board[SIZE][SIZE], Cell empty_cells[],
                     int max_cells);
const int check_winner(const int board[SIZE][SIZE]);

#endif  // BOARD_H
