#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../shared/board.h"
#include "../shared/minimax.h"
#include "../shared/naive_bayes.h"

typedef enum {
  MODE_1_PLAYER,
  MODE_2_PLAYER,
} GameMode;

typedef enum {
  DIFF_NONE,
  DIFF_EASY,
  DIFF_MEDIUM,
  DIFF_HARD,
} DifficultyLevel;

typedef struct {
  GameMode mode;
  DifficultyLevel difficulty;
  Board board;
} GameState;

static NaiveBayesModel nb_model;
static GameState game_state;

/**
 * @brief Prompts the user to select the first player for the game.
 *
 * @param is_single_player True if it's a single-player game, false otherwise.
 * @return The selected first player (PLAYER_X or PLAYER_O).
 */
Player get_first_player(bool is_single_player) {
  int choice;
  if (is_single_player) {
    printf(
        "\nSelect first player:\n1. You (Player X)\n2. AI (Player O)\nEnter "
        "your "
        "choice: ");
  } else {
    printf(
        "\nSelect first player:\n1. Player X\n2. Player O\nEnter your "
        "choice: ");
  }
  scanf("%d", &choice);
  return (choice == 1) ? PLAYER_X : PLAYER_O;
}

/**
 * @brief Prompts the user to select the difficulty level for the AI.
 *
 * @return The selected difficulty level.
 */
DifficultyLevel get_difficulty_level() {
  int choice;
  printf(
      "\nSelect difficulty level:\n1. Easy\n2. Medium\n3. Hard\nEnter your "
      "choice: ");
  scanf("%d", &choice);
  switch (choice) {
    case 1:
      return DIFF_EASY;
    case 2:
      return DIFF_MEDIUM;
    case 3:
      return DIFF_HARD;
    default:
      printf("Invalid choice. Defaulting to Easy.\n");
      return DIFF_EASY;
  }
}

/**
 * @brief Prompts the current player to enter their move.
 *
 * @param board Pointer to the current game board.
 * @return The cell where the player wants to make their move.
 */
Cell get_player_move(const Board* board) {
  int cell_num;
  printf("Player %s, enter your move (1-9): ",
         board->current_player == PLAYER_X ? "X" : "O");
  scanf("%d", &cell_num);
  cell_num--;  // Convert to 0-based index
  return (Cell){cell_num / SIZE, cell_num % SIZE};
}

/**
 * @brief Determines the AI's move based on the difficulty level.
 *
 * @param board Pointer to the current game board.
 * @param difficulty The selected difficulty level.
 * @return The cell where the AI will make its move.
 */
Cell get_ai_move(const Board* board, DifficultyLevel difficulty) {
  if (board->move_count == 0) {
    return random_move(board);
  }

  switch (difficulty) {
    case DIFF_EASY:
      // Random move for easy
      return random_move(board);
    case DIFF_MEDIUM:
      // Naive Bayes move for medium
      return nb_find_move(board, &nb_model);
    case DIFF_HARD:
      // Minimax move for hard
      return minimax_find_move(board, true);
    default:
      return random_move(board);
  }
}

/**
 * @brief Announces the winner of the game or declares a draw.
 *
 * @param winner The winner of the game.
 */
void announce_winner(Winner winner) {
  switch (winner) {
    case WIN_X:
      printf("Player X wins!\n");
      break;
    case WIN_O:
      printf("Player O wins!\n");
      break;
    case DRAW:
      printf("It's a draw!\n");
      break;
    default:
      break;
  }
}

/**
 * @brief Resets the game state with a new starting player.
 *
 * @param state Pointer to the game state to reset.
 * @param starting_player The player who starts the new game.
 */
void reset_game_state(GameState* state, Player starting_player) {
  init_board(&state->board, starting_player);
}

/**
 * @brief Handles the main game loop, including moves and win checking.
 */
void play_game() {
  printf("\n");
  print_board(&game_state.board);
  while (1) {
    Cell move;
    if (game_state.mode == MODE_1_PLAYER &&
        game_state.board.current_player == PLAYER_O) {
      printf("AI is making a move...\n");
      move = get_ai_move(&game_state.board, game_state.difficulty);
    } else {
      move = get_player_move(&game_state.board);
    }

    if (!make_move(&game_state.board, &move)) {
      printf("Invalid move. Try again.\n");
      continue;
    }

    print_board(&game_state.board);

    // Check for winner
    Winner winner = check_winner(&game_state.board, NULL);
    if (winner != ONGOING) {
      announce_winner(winner);

      printf("\nPlay again? (y/n): ");
      char choice;
      scanf(" %c", &choice);

      if (choice == 'y' || choice == 'Y') {
        Player starting_player =
            get_first_player(game_state.mode == MODE_1_PLAYER ? true : false);
        reset_game_state(&game_state, starting_player);
        print_board(&game_state.board);
      } else {
        break;
      }
    }
  }
}

/**
 * @brief Initalises and starts a single-player game.
 */
void one_player() {
  GameMode mode = MODE_1_PLAYER;
  Player first_player = get_first_player(true);
  DifficultyLevel difficulty = get_difficulty_level();
  printf("Starting 1 Player game. First player: %s, Difficulty: %s\n",
         first_player == PLAYER_X ? "X" : "O",
         difficulty == DIFF_EASY
             ? "Easy"
             : (difficulty == DIFF_MEDIUM ? "Medium" : "Hard"));

  Board board;
  init_board(&board, first_player);
  game_state = (GameState){mode, difficulty, board};
  play_game();
}

/**
 * @brief Initalises and starts a two-player game.
 */
void two_player() {
  GameMode mode = MODE_2_PLAYER;
  Player first_player = get_first_player(false);
  printf("Starting 2 Player game. First player: %s\n",
         first_player == PLAYER_X ? "X" : "O");

  Board board;
  init_board(&board, first_player);
  game_state = (GameState){mode, DIFF_NONE, board};
  play_game();
}

/**
 * @brief Main entry point of the application, handles menu and game selection.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line arguments.
 * @return Exit status.
 */
int main(int argc, char* argv[]) {
  // Seed RNG
  srand(time(NULL));
  // Load Naive Bayes model
  int res = load_nb_model(&nb_model, "src/ml/naive_bayes.bin");
  if (res != 0) {
    fprintf(stderr, "Failed to load Naive Bayes model.\n");
    return EXIT_FAILURE;
  }

  while (1) {
    const char* menu_message =
        "\nSelect an option:\n"
        "1. 1 Player\n"
        "2. 2 Player\n"
        "3. Exit\n"
        "Enter your choice: ";
    printf("%s", menu_message);
    int choice;
    scanf("%d", &choice);
    switch (choice) {
      case 1: {
        one_player();
        break;
      }
      case 2: {
        two_player();
        break;
      }
      case 3:
        printf("Exiting the game. Goodbye!\n");
        exit(0);
      default:
        printf("Invalid choice. Please try again.\n");
    }
  }

  return 0;
}
