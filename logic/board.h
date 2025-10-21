#ifndef BOARD_H
#define BOARD_H

#define SIZE 3

typedef struct
{
    int row;
    int col;
} Cell;

void print_board(char board[SIZE][SIZE]);
int find_empty_cells(char board[SIZE][SIZE], Cell empty_cells[], int max_cells);
const char *check_winner(char board[SIZE][SIZE]);

#endif // BOARD_H
