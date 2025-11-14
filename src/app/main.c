#include <stdio.h>
#include <stdlib.h>

#include "../shared/board.h"
#include "../shared/minimax.h"
#include "../shared/naive_bayes.h"

int main(int argc, const char* argv[]) {
  const char* weights_path = "src/ml/naive_bayes.bin";
  if (argc > 1) {
    weights_path = argv[1];
  }
  NaiveBayesModel model;
  int res = load_nb_model(&model, weights_path);
  if (res != 0) {
    printf("Error loading model from '%s'\n", weights_path);
    return EXIT_FAILURE;
  }

  Board board;
  init_board(&board, PLAYER_X);
  // Let Player X make a random first move
  Cell rand_move = random_move(&board);
  make_move(&board, &rand_move);
  // Let Player O make a random second move
  rand_move = random_move(&board);
  make_move(&board, &rand_move);

  while (board.move_count < SIZE * SIZE) {
    print_board(&board);
    printf("\n");

    if (board.current_player == PLAYER_X) {
      Cell ai_move = minimax_find_move(&board, true);
      make_move(&board, &ai_move);
    } else {
      // Cell ai_move = random_move(&board);
      Cell ai_move = nb_find_move(&board, &model);
      make_move(&board, &ai_move);
    }

    Winner result = check_winner(&board);
    if (result != ONGOING) {
      print_board(&board);
      if (result == WIN_X) {
        printf("Player X wins!\n");
      } else if (result == WIN_O) {
        printf("Player O wins!\n");
      } else {
        printf("It's a draw!\n");
      }
      break;
    }
  }

  return EXIT_SUCCESS;
}
