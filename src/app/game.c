#include "game.h"

#include <stdbool.h>
#include <stdlib.h>

/* Global game state */
static GameState g_game_state;
static bool g_game_state_initalised = false;

int init_game_state(GtkBuilder* builder, NaiveBayesModel* model) {
  if (g_game_state_initalised) return -1;
  if (!builder || !model) return -1;

  g_game_state = (GameState){
      .mode = 0,
      .difficulty = 0,
      .starting_player = 0,
      .stats = {0},
      .builder = builder,
      .nb_model = model,
  };
  init_board(&g_game_state.board, PLAYER_X);
  g_game_state_initalised = true;

  return 0;
}

GameState* get_game_state() {
  return g_game_state_initalised ? &g_game_state : NULL;
}

int set_game_mode(GameMode mode) {
  if (!g_game_state_initalised) return -1;
  g_game_state.mode = mode;
  return 0;
}

int set_first_player(Player player) {
  if (!g_game_state_initalised) return -1;
  g_game_state.starting_player = player;
  init_board(&g_game_state.board, player);
  return 0;
}

int set_difficulty(DifficultyLevel difficulty) {
  if (!g_game_state_initalised) return -1;
  g_game_state.difficulty = difficulty;
  return 0;
}

Cell get_ai_move() {
  if (!g_game_state_initalised) return (Cell){-1, -1};

  // Random move for first turn or easy difficulty
  if (g_game_state.board.move_count == 0 ||
      g_game_state.difficulty == DIFF_EASY) {
    return random_move(&g_game_state.board);
  }

  // Naive Bayes for medium difficulty
  if (g_game_state.difficulty == DIFF_MEDIUM) {
    return nb_find_move(&g_game_state.board, g_game_state.nb_model);
  }

  // Handicapped minimax for hard difficulty and default
  return minimax_find_move(&g_game_state.board, true);
}

int reset_scoreboard() {
  if (!g_game_state_initalised) return -1;
  g_game_state.stats.score_X = 0;
  g_game_state.stats.score_O = 0;
  g_game_state.stats.score_tie = 0;
  return 0;
}
