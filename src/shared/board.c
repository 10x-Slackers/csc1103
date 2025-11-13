#include "board.h"

#include <stdio.h>

void init_board(Board* board, Player starting_player) {
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      board->cells[i][j] = EMPTY;
    }
  }
  board->current_player = starting_player;
  board->move_count = 0;
}

void print_board(const Board* board) {
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      char cell;
      switch (board->cells[i][j]) {
        case X:
          cell = 'X';
          break;
        case O:
          cell = 'O';
          break;
        case EMPTY:
        default:
          // ASCII '1' (49) + cell index (0-8)
          cell = '1' + (i * SIZE + j);
          break;
      }
      printf("%c", cell);
      // Print column separator
      if (j < SIZE - 1) printf("|");
    }
    printf("\n");
    // Print row separator
    if (i < SIZE - 1) printf("---------\n");
  }
}

void copy_board(const Board* src, Board* dest) {
  memcpy(dest->cells, src->cells, sizeof(src->cells));
  dest->current_player = src->current_player;
  dest->move_count = src->move_count;
}

bool check_cell(const Board* board, Cell* cell) {
  if (board->cells[cell->row][cell->col] == EMPTY) {
    return true;
  }
  return false;
}

bool make_move(Board* board, Cell* cell) {
  if (!check_cell(board, cell)) {
    return false;
  }
  board->cells[cell->row][cell->col] = board->current_player;
  board->move_count++;
  board->current_player =
      (board->current_player == PLAYER_X) ? PLAYER_O : PLAYER_X;
  return true;
}

int find_empty_cells(const Board* board, Cell empty_cells[]) {
  size_t max_cells;
  if (empty_cells != NULL) {
    max_cells = sizeof(empty_cells) / sizeof(empty_cells[0]);
  } else {
    max_cells = SIZE * SIZE;
  }

  int count = 0;
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      if (count >= (int)max_cells) {
        return count;
      }
      if (board->cells[i][j] == EMPTY) {
        if (empty_cells != NULL) {
          empty_cells[count++] = (Cell){i, j};
        } else {
          count++;
        }
      }
    }
  }
  return count;
}

Winner check_winner(const Board* board) {
  size_t patterns = sizeof(WIN_PATTERNS) / sizeof(WIN_PATTERNS[0]);

  for (size_t i = 0; i < patterns; ++i) {
    const Cell* pattern = WIN_PATTERNS[i];
    CellState a = board->cells[pattern[0].row][pattern[0].col];
    CellState b = board->cells[pattern[1].row][pattern[1].col];
    CellState c = board->cells[pattern[2].row][pattern[2].col];

    if (a != EMPTY && a == b && b == c) {
      return (a == X) ? WIN_X : WIN_O;
    }
  }

  int empty_count = find_empty_cells(board, NULL);
  if (empty_count > 0) {
    return ONGOING;
  }

  return DRAW;
}
