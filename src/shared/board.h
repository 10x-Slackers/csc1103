#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>
#include <stddef.h>

#define SIZE 3

typedef enum { EMPTY, X, O } CellState;
typedef enum { PLAYER_X = 1, PLAYER_O = 2 } Player;
typedef enum { ONGOING, WIN_X, WIN_O, DRAW } Winner;

typedef struct {
  int row;
  int col;
} Cell;

typedef struct {
  CellState cells[SIZE][SIZE];
  Player current_player;
  Cell last_move;
  int move_count;
} Board;

typedef struct {
  int score_X;
  int score_O;
  int score_tie;
} GameStats;

/**
 * @brief Initialize a game board to an empty state.
 * @param board Pointer to the Board structure to initialize.
 * @param starting_player The player who will make the first move.
 */
void init_board(Board* board, Player starting_player);

/**
 * @brief Print the current state of the game board.
 *
 * Displays 'X', 'O', or the number of the empty cell (1-9).
 * Cells in a row are separated by '|' and rows by lines.
 *
 * @param board Pointer to the Board structure to print.
 */
void print_board(const Board* board);

/**
 * @brief Copy the contents of one board to another.
 * @param src Pointer to the source Board structure.
 * @param dest Pointer to the destination Board structure.
 */
void copy_board(const Board* src, Board* dest);

/**
 * @brief Find all empty cells on the board.
 * @param board Pointer to the Board structure.
 * @param empty_cells Output array to store empty cell positions. Can be NULL to
 *                    just count empty cells.
 * @param max_cells Maximum number of cells the array can hold.
 * @return int The number of empty cells found.
 */
int find_empty_cells(const Board* board, Cell empty_cells[], size_t max_cells);

/**
 * @brief Make a move on the board at the specified row and column.
 * @param board Pointer to the Board structure.
 * @param cell Pointer to the Cell structure indicating where to make the move.
 * @return true if the move was successful, false otherwise.
 */
bool make_move(Board* board, const Cell* cell);

/**
 * @brief Undo the last move made on the board.
 * @param board Pointer to the Board structure.
 * @return true if the undo was successful, false otherwise.
 */
bool undo_move(Board* board);

/**
 * @brief Check if there is a winner or if the game is a draw.
 * @param board Pointer to the Board structure.
 * @return Winner Returns WIN_X if player X wins, WIN_O if player O wins,
 *                DRAW if it's a draw, or ONGOING if the game is still ongoing.
 */
Winner check_winner(const Board* board);

/**
 * @brief Select a random valid move from the available empty cells.
 * @param board Pointer to the Board structure.
 * @return Cell The selected random move.
 */
Cell random_move(const Board* board);

#endif  // BOARD_H
