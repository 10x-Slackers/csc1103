/**
 * @file game.c
 * @brief Manages the global game state.
 * @authors kaiwenteoo, commit2main
 * @date 2025-11-11
 */
#include "game.h"

#include <stdbool.h>
#include <stdlib.h>

/* Global game state */
static GameState game_state;
static bool game_state_initialised = false;

int init_game_state(GtkBuilder* builder, NaiveBayesModel* model) {
  if (game_state_initialised) return -1;
  if (!builder || !model) return -1;

  game_state = (GameState){
      .mode = 0,
      .difficulty = 0,
      .starting_player = 0,
      .stats = {0},
      .builder = builder,
      .nb_model = model,
  };
  init_board(&game_state.board, PLAYER_X);
  game_state_initialised = true;

  return 0;
}

GameState* get_game_state() {
  return game_state_initialised ? &game_state : NULL;
}

int set_game_mode(GameMode mode) {
  if (!game_state_initialised) return -1;
  game_state.mode = mode;
  return 0;
}

int set_first_player(Player player) {
  if (!game_state_initialised) return -1;
  game_state.starting_player = player;
  init_board(&game_state.board, player);
  return 0;
}

int set_difficulty(DifficultyLevel difficulty) {
  if (!game_state_initialised) return -1;
  game_state.difficulty = difficulty;
  return 0;
}

Cell get_ai_move() {
  if (!game_state_initialised) return (Cell){-1, -1};

  // Random move for first turn or easy difficulty
  if (game_state.board.move_count == 0 || game_state.difficulty == DIFF_EASY) {
    return random_move(&game_state.board);
  }

  // Naive Bayes for medium difficulty
  if (game_state.difficulty == DIFF_MEDIUM)
    return nb_find_move(&game_state.board, game_state.nb_model);

  // Imperfect minimax for hard difficulty
  return minimax_find_move(&game_state.board, true);
}

int reset_scoreboard() {
  if (!game_state_initialised) return -1;
  game_state.stats.score_X = 0;
  game_state.stats.score_O = 0;
  game_state.stats.score_tie = 0;
  return 0;
}
