/*
 * Tic-Tac-Toe game for two players.
 *
 * Author: kaiwenteoo
 * Date: 2025-10-19
 * Version: 1
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define SIZE 3

/**
 * @struct Cell
 * @brief Represents a cell's position on the Tic-Tac-Toe board.
 *
 * A simple structure that holds the row and column indices of a cell.
 */
typedef struct
{
    int row; /**< Row index of the cell (0-based) */
    int col; /**< Column index of the cell (0-based) */
} Cell;

void play_game(void);
void print_board(char board[SIZE][SIZE]);
void get_valid_move(char current_player, char board[SIZE][SIZE], int *out_row, int *out_col);
const char *check_winner(char board[SIZE][SIZE]);

int main()
{
    play_game();
    return 0;
}

/**
 * @brief Runs the main game loop for Tic-Tac-Toe.
 *
 * Initializes the board, alternates player turns,
 * and determines when the game ends (win or draw).
 */
void play_game()
{
    char board[SIZE][SIZE];

    // Initialize board to empty spaces
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            board[i][j] = ' ';

    char current_player = 'X';
    const char *winner = NULL;
    int row, col;

    while (true)
    {
        print_board(board);
        get_valid_move(current_player, board, &row, &col);
        board[row][col] = current_player;

        winner = check_winner(board);
        if (winner != NULL)
        {
            print_board(board);
            if (strcmp(winner, "Draw") == 0)
            {
                printf("It's a draw!\n");
            }
            else if (winner[0] == 'X')
            {
                printf("Player X wins!\n");
            }
            else if (winner[0] == 'O')
            {
                printf("Player O wins!\n");
            }
            break;
        }

        current_player = (current_player == 'X') ? 'O' : 'X';
    }
}

/**
 * @brief Prints the current Tic-Tac-Toe board.
 *
 * Empty cells are displayed as their number (1-9).
 * Occupied cells display 'X' or 'O'.
 *
 * @param board The current game board.
 */
void print_board(char board[SIZE][SIZE])
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            char cell = board[i][j];
            int cell_num = i * SIZE + j + 1;
            if (cell == ' ')
            {
                printf("%d", cell_num);
            }
            else
            {
                printf("%c", cell);
            }
            if (j < SIZE - 1)
                printf(" | ");
        }
        printf("\n");
        if (i < SIZE - 1)
            printf("---------\n");
    }
}

/**
 * @brief Prompts the current player to make a valid move.
 *
 * Validates the input to ensure it's within range and not already occupied.
 *
 * @param current_player The character representing the current player ('X' or 'O').
 * @param board The current game board.
 * @param out_row Pointer to store the chosen row index.
 * @param out_col Pointer to store the chosen column index.
 */
void get_valid_move(char current_player, char board[SIZE][SIZE], int *out_row, int *out_col)
{
    int move;
    while (true)
    {
        printf("Player %c, choose your box (1-9): ", current_player);
        int result = scanf("%d", &move);
        while (getchar() != '\n')
            ; // clear input buffer

        if (result != 1 || move < 1 || move > 9)
        {
            printf("Invalid input. Choose a number between 1 and 9.\n");
            continue;
        }

        int row = (move - 1) / SIZE;
        int col = (move - 1) % SIZE;

        if (board[row][col] != ' ')
        {
            printf("That box is already taken. Choose another.\n");
            continue;
        }

        *out_row = row;
        *out_col = col;
        break;
    }
}

/**
 * @brief Check if there is a winner or if the game is a draw.
 *
 * Checks all rows, columns, and diagonals for three matching 'X' or 'O'.
 * If no winner and no empty cells remain, declares a draw.
 *
 * @param board The current game board.
 * @return const char* Returns:
 *           - "X" if player X wins,
 *           - "O" if player O wins,
 *           - "Draw" if the board is full with no winner,
 *           - NULL if the game is ongoing.
 */
const char *check_winner(char board[SIZE][SIZE])
{
    // Check rows and columns for a win
    for (int i = 0; i < SIZE; i++)
    {
        // Check rows
        if (board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][1] == board[i][2])
        {
            return (board[i][0] == 'X') ? "X" : "O";
        }
        // Check columns
        if (board[0][i] != ' ' && board[0][i] == board[1][i] && board[1][i] == board[2][i])
        {
            return (board[0][i] == 'X') ? "X" : "O";
        }
    }

    // Check diagonals
    if (board[0][0] != ' ' && board[0][0] == board[1][1] && board[1][1] == board[2][2])
    {
        return (board[0][0] == 'X') ? "X" : "O";
    }
    if (board[0][2] != ' ' && board[0][2] == board[1][1] && board[1][1] == board[2][0])
    {
        return (board[0][2] == 'X') ? "X" : "O";
    }

    // Check for a draw (no empty cells left)
    int empty_cells = 0;
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            if (board[i][j] == ' ')
                empty_cells++;

    if (empty_cells == 0)
        return "Draw";

    return NULL;
}
