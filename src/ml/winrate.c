#include "winrate.h"

#include <stdio.h>
#include <stdlib.h>

/**
 * @brief: Initialize win rate statistics.
 * @param: stats Pointer to WinRateStats structure to initialize.
 *
 * @return: void
 **/
void init_winrate_stats(WinRateStats* stats) {
  stats->wins = 0;
  stats->losses = 0;
  stats->draws = 0;
}

/**
 * @brief: Run episodes using Naive Bayes model and update win rate statistics.
 * @param: model Pointer to NaiveBayesModel to use for making moves.
 *
 * @return: void
 **/
void run_episodes(NaiveBayesModel* model) {
  Board board;
  WinRateStats stats;
  init_winrate_stats(&stats);
  if (model == NULL) {
    init_winrate_stats(&stats);
    for (int episode = 0; episode < EPISODES; episode++) {
      init_board(&board, PLAYER_X);
      while (check_winner(&board, NULL) == ONGOING) {
        Cell move = minimax_find_move(&board, false);
        make_move(&board, &move);
        if (check_winner(&board, NULL) != ONGOING) break;
        move = random_move(&board);
        make_move(&board, &move);
      }
      update_stats(&stats, &board);
    }
    display_result(&stats);
    return;
  }
  for (int episode = 0; episode < EPISODES; episode++) {
    init_board(&board, PLAYER_X);
    while (check_winner(&board, NULL) == ONGOING) {
      Cell move = nb_find_move(&board, model);
      make_move(&board, &move);
      if (check_winner(&board, NULL) != ONGOING) break;
      move = random_move(&board);
      make_move(&board, &move);
    }
    update_stats(&stats, &board);
  }
  display_result(&stats);
}

/**
 * @brief: Update win rate statistics based on the outcome of a game.
 * @param: stats Pointer to WinRateStats structure to update.
 * @param: board Pointer to Board structure representing the game state.
 *
 * @return: void
 **/
void update_stats(WinRateStats* stats, Board* board) {
  Winner winner = check_winner(board, NULL);
  if (winner == PLAYER_X) {
    stats->wins++;
  } else if (winner == PLAYER_O) {
    stats->losses++;
  } else {
    stats->draws++;
  }
};

/**
 * @brief: Display win rate statistics.
 * @param: stats Pointer to WinRateStats structure to display.
 *
 * @return: void
 **/
void display_result(WinRateStats* stats) {
  printf("Win Rate Results after %d episodes:\n", EPISODES);
  printf("Wins: %d\n", stats->wins);
  printf("Losses: %d\n", stats->losses);
  printf("Draws: %d\n", stats->draws);
  double win_rate = (double)stats->wins / EPISODES * 100.0;
  printf("Win Rate: %.2f%%\n", win_rate);
}

/**
 * @brief: Test Minimax and Naive Bayes models and display their win rates.
 * @param: model_path Path to the Naive Bayes model file.
 *
 * @return: EXIT_SUCCESS on success, EXIT_FAILURE on error.
 **/
int test_models(const char* model_path) {
  NaiveBayesModel model;
  if (load_nb_model(&model, model_path) != 0) {
    fprintf(stderr, "Error: Failed to load model\n");
    return EXIT_FAILURE;
  }
  srand(1234);
  run_episodes(NULL);
  srand(1234);
  run_episodes(&model);
  return EXIT_SUCCESS;
}
