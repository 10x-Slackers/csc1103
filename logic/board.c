#include <stdio.h>
#include <stdbool.h>

#define SIZE 3

/**
 * @struct Cell
 * @brief Represents a cell's position on the Tic-Tac-Toe board.
 *
 * Holds the row and column indices of a cell.
 */
typedef struct
{
    int row; /**< Row index of the cell (0-based) */
    int col; /**< Column index of the cell (0-based) */
} Cell;

/**
 * @brief Print the Tic-Tac-Toe board in a readable format.
 *
 * Displays 'X', 'O', or '_' for empty cells.
 * Cells in a row are separated by '|'.
 *
 * @param board The 3x3 Tic-Tac-Toe board.
 */
void print_board(char board[SIZE][SIZE])
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            char cell = board[i][j];
            printf("%c", (cell == ' ' || cell == '\0') ? '_' : cell);
            if (j < SIZE - 1)
                printf("|");
        }
        printf("\n");
    }
}

/**
 * @brief Find all empty cells on the Tic-Tac-Toe board.
 *
 * An empty cell contains either a space (' ') or null character ('\0').
 * Stores found empty cells in the provided array.
 *
 * @param board The 3x3 Tic-Tac-Toe board.
 * @param empty_cells Output array to hold empty cell positions.
 * @param max_cells Maximum number of cells the empty_cells array can hold.
 * @return int The number of empty cells found.
 */
int find_empty_cells(char board[SIZE][SIZE], Cell empty_cells[], int max_cells)
{
    int count = 0;
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            if (board[i][j] == ' ' || board[i][j] == '\0')
            {
                if (count < max_cells)
                {
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
 * Checks rows, columns, and diagonals for a winner.
 * Checks for draw if no empty cells remain.
 *
 * @param board The 3x3 Tic-Tac-Toe board.
 * @return const char* "X" if X wins, "O" if O wins, "Draw" if draw,
 *         or NULL if game is ongoing.
 */
const char *check_winner(char board[SIZE][SIZE])
{
    // Check rows and columns for a win
    for (int i = 0; i < SIZE; i++)
    {
        if (board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][1] == board[i][2])
        {
            return (board[i][0] == 'X') ? "X" : "O";
        }
        if (board[0][i] != ' ' && board[0][i] == board[1][i] && board[1][i] == board[2][i])
        {
            return (board[0][i] == 'X') ? "X" : "O";
        }
    }

    // Check diagonals for a win
    if (board[0][0] != ' ' && board[0][0] == board[1][1] && board[1][1] == board[2][2])
    {
        return (board[0][0] == 'X') ? "X" : "O";
    }
    if (board[0][2] != ' ' && board[0][2] == board[1][1] && board[1][1] == board[2][0])
    {
        return (board[0][2] == 'X') ? "X" : "O";
    }

    // Check for draw
    Cell empty_cells[SIZE * SIZE];
    if (find_empty_cells(board, empty_cells, SIZE * SIZE) == 0)
    {
        return "Draw";
    }

    return NULL;
}
