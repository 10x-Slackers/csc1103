#ifndef GAME_H
#define GAME_H

#include <gtk/gtk.h>

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
  Player starting_player;
  Board board;
  GameStats stats;
  GtkBuilder* builder;
  NaiveBayesModel* nb_model;
} GameState;

/**
 * @brief Initialize the global game state with default values.
 * @param builder Pointer to the GtkBuilder instance.
 * @param model Pointer to the Naive Bayes model.
 * @return 0 on success, -1 if already initialized or invalid parameters.
 */
int init_game_state(GtkBuilder* builder, NaiveBayesModel* model);

/**
 * @brief Get the global game state.
 * @return Pointer to the global game state or NULL if not initialized.
 */
GameState* get_game_state();

/**
 * @brief Set the game mode.
 * @param mode The game mode to set.
 * @return 0 on success, -1 if game state is not initialized.
 */
int set_game_mode(GameMode mode);

/**
 * @brief Set the first player.
 * @param player The player to start first.
 * @return 0 on success, -1 if game state is not initialized.
 */
int set_first_player(Player player);

/**
 * @brief Set the difficulty level.
 * @param difficulty The difficulty level to set.
 * @return 0 on success, -1 if game state is not initialized.
 */
int set_difficulty(DifficultyLevel difficulty);

/**
 * @brief Get the AI move based on difficulty level.
 * @return The cell chosen by the AI.
 */
Cell get_ai_move();

#endif  // GAME_H
