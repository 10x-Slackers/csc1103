#include "board.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static const Cell WIN_PATTERNS[8][3] = {
    // Rows
    {{0, 0}, {0, 1}, {0, 2}},
    {{1, 0}, {1, 1}, {1, 2}},
    {{2, 0}, {2, 1}, {2, 2}},
    // Columns
    {{0, 0}, {1, 0}, {2, 0}},
    {{0, 1}, {1, 1}, {2, 1}},
    {{0, 2}, {1, 2}, {2, 2}},
    // Diagonals
    {{0, 0}, {1, 1}, {2, 2}},
    {{0, 2}, {1, 1}, {2, 0}}};

/**
 * @brief Check if a specific cell is empty.
 * @param board Pointer to the Board structure.
 * @param cell Pointer to the Cell structure to check.
 * @return true if the cell is empty, false otherwise.
 */
static bool check_cell(const Board* board, const Cell* cell) {
  // Check bounds
  if (cell->row < 0 || cell->row >= SIZE || cell->col < 0 ||
      cell->col >= SIZE) {
    return false;
  }
  // Check if cell is empty
  if (board->cells[cell->row][cell->col] == EMPTY) {
    return true;
  }
  return false;
}

void init_board(Board* board, Player starting_player) {
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      board->cells[i][j] = EMPTY;
    }
  }
  board->current_player = starting_player;
  board->last_move = (Cell){-1, -1};
  board->move_count = 0;
  // WIP: Move srand to program initialization
  srand((unsigned int)time(NULL));
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
          cell = ' ';
          // ASCII '1' (49) + cell index (0-8)
          // cell = '1' + (i * SIZE + j);
          break;
      }
      printf(" %c ", cell);
      // Print column separator
      if (j < SIZE - 1) printf("|");
    }
    printf("\n");
    // Print row separator
    if (i < SIZE - 1) {
      for (int col = 0; col < SIZE; ++col) {
        printf("---");
        if (col < SIZE - 1) printf("+");
      }
      printf("\n");
    }
  }
}

void copy_board(const Board* src, Board* dest) {
  memcpy(dest->cells, src->cells, sizeof(src->cells));
  dest->current_player = src->current_player;
  dest->last_move = src->last_move;
  dest->move_count = src->move_count;
}

int find_empty_cells(const Board* board, Cell empty_cells[], size_t max_cells) {
  if (empty_cells == NULL) {
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

bool make_move(Board* board, const Cell* cell) {
  if (!check_cell(board, cell)) {
    return false;
  }
  board->cells[cell->row][cell->col] =
      (board->current_player == PLAYER_X) ? X : O;
  board->move_count++;
  board->current_player =
      (board->current_player == PLAYER_X) ? PLAYER_O : PLAYER_X;
  board->last_move = *cell;
  return true;
}

bool undo_move(Board* board) {
  Cell last_move = board->last_move;
  if (last_move.row == -1 || last_move.col == -1) {
    return false;
  }
  board->cells[last_move.row][last_move.col] = EMPTY;
  board->move_count--;
  board->current_player =
      (board->current_player == PLAYER_X) ? PLAYER_O : PLAYER_X;
  // Reset last move
  board->last_move = (Cell){-1, -1};
  return true;
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

  int empty_count = find_empty_cells(board, NULL, 0);
  if (empty_count > 0) {
    return ONGOING;
  }

  return DRAW;
}

Cell random_move(const Board* board) {
  Cell empty_cells[SIZE * SIZE];
  int empty_count = find_empty_cells(board, empty_cells, SIZE * SIZE);
  if (empty_count == 0) {
    return (Cell){-1, -1};
  }

  int rand_index = rand() % empty_count;
  return empty_cells[rand_index];
}
