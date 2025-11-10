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

typedef struct {
  int score_X;
  int score_O;
  int score_tie;
} score_board_t;

void print_board(const int board[SIZE][SIZE], const score_board_t* score_board);
int find_empty_cells(const int board[SIZE][SIZE], Cell empty_cells[],
                     int max_cells);
const int check_winner(const int board[SIZE][SIZE]);

#endif  // BOARD_H
