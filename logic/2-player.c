/*
 * Tic-Tac-Toe game for two players.
 *
 * Author: kaiwenteoo
 * Date: 2025-10-21
 * Version: 1
 */

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"

void play_game(void);
void get_valid_move(int current_player, int board[SIZE][SIZE], int* out_row,
                    int* out_col);
score_board_t score_board = {0, 0, 0};

int main() {
  char choice;
  do {
    play_game();
    do {
      printf("Play again? (Y/N): ");
      scanf(" %c", &choice);
      while (getchar() != '\n');
      choice = tolower(choice);
      if (choice != 'y' && choice != 'n') {
        printf("Invalid input. Please enter Y or N.\n");
      }
    } while (choice != 'y' && choice != 'n');
  } while (choice == 'y');
  return 0;
}

/**
 * @brief Runs the main game loop for Tic-Tac-Toe.
 *
 * Initializes the board, alternates player turns,
 * and determines when the game ends (win or draw).
 */
void play_game() {
  int board[SIZE][SIZE];

  // Initialize board to empty spaces
  for (int i = 0; i < SIZE; i++)
    for (int j = 0; j < SIZE; j++) board[i][j] = EMPTY;

  int current_player = X;
  int winner = -1;
  int row, col;

  while (true) {
    print_board(board, &score_board);
    get_valid_move(current_player, board, &row, &col);
    board[row][col] = current_player;

    winner = check_winner(board);
    if (winner != -1) {
      if (winner == EMPTY) {
        score_board.score_tie++;
        print_board(board, &score_board);
        printf("It's a draw!\n");
      } else if (winner == X) {
        score_board.score_X++;
        print_board(board, &score_board);
        printf("Player X wins!\n");
      } else if (winner == O) {
        score_board.score_O++;
        print_board(board, &score_board);
        printf("Player O wins!\n");
      }
      break;
    }

    current_player = (current_player == X) ? O : X;
  }
}

/**
 * @brief Prompts the current player to make a valid move.
 *
 * Validates the input to ensure it's a non-empty, numeric input within range
 * and not already occupied.
 *
 * @param current_player The character representing the current player ('X' or
 * 'O').
 * @param board The current game board.
 * @param out_row Pointer to store the chosen row index.
 * @param out_col Pointer to store the chosen column index.
 */
void get_valid_move(int current_player, int board[SIZE][SIZE], int* out_row,
                    int* out_col) {
  char buffer[100];
  int move;
  char player = (current_player == X) ? 'X' : 'O';

  while (true) {
    printf("Player %c, choose your box (1-9): ", player);
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
      printf("Please enter a valid number.\n");
      continue;
    }

    // Remove trailing newline if present
    buffer[strcspn(buffer, "\n")] = '\0';

    // Check for empty input
    if (strlen(buffer) == 0) {
      printf("Please enter a valid number.\n");
      continue;
    }

    // Convert input to integer
    char* endptr;
    move = strtol(buffer, &endptr, 10);

    // Check if entire input was a number or out of range 1-9
    if (*endptr != '\0' || move < 1 || move > 9) {
      printf("Invalid input. Choose a number between 1 and 9.\n");
      continue;
    }

    int row = (move - 1) / SIZE;
    int col = (move - 1) % SIZE;

    if (board[row][col] != EMPTY) {
      printf("That box is already taken. Choose another.\n");
      continue;
    }

    *out_row = row;
    *out_col = col;
    break;
  }
}
