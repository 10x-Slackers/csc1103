#include "naive_bayes.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#ifdef EMBED_NB_MODEL
#include "nb_model.h"
extern unsigned char nb_model_bin[];
extern unsigned int nb_model_bin_len;
#else
static const unsigned char* nb_model_bin = NULL;
static const unsigned int nb_model_bin_len = 0;
#endif

/**
 * @brief Invert the board state by swapping X and O.
 *
 * This is necessary because the Naive Bayes model is trained on X as the
 * positive player.
 *
 * @param board Pointer to the Board structure to invert.
 */
static void invert_board(Board* board) {
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      if (board->cells[i][j] == X) {
        board->cells[i][j] = O;
      } else if (board->cells[i][j] == O) {
        board->cells[i][j] = X;
      }
    }
  }
}

int load_nb_model(NaiveBayesModel* model, const char* model_path) {
  // Load from embedded binary data if file path is not provided
  if (model_path == NULL && nb_model_bin_len == sizeof(NaiveBayesModel)) {
    memcpy(model, nb_model_bin, sizeof(NaiveBayesModel));
    return 0;
  }

  FILE* file = fopen(model_path, "rb");
  if (!file) return -1;

  size_t read_count = fread(model, sizeof(NaiveBayesModel), 1, file);
  fclose(file);
  if (read_count != 1) return -1;

  return 0;
}

float naive_bayes(Board* board, const NaiveBayesModel* model) {
  double log_scores[OUTCOMES] = {0.0};
  // Calculate log probability for each outcome
  for (int outcome = 0; outcome < OUTCOMES; outcome++) {
    // Start with log prior
    double score = log(model->prior[outcome]);
    // Add log likelihoods for each cell in the board
    for (int row = 0; row < SIZE; row++) {
      for (int col = 0; col < SIZE; col++) {
        int cell_value = board->cells[row][col];
        score += log(model->likelihood[outcome][row][col][cell_value]);
      }
    }
    log_scores[outcome] = score;
  }

  // Compute the sum of exponentials
  double max_score =
      log_scores[0] > log_scores[1] ? log_scores[0] : log_scores[1];
  double total_log_score = 0;
  for (int outcome = 0; outcome < OUTCOMES; outcome++) {
    total_log_score += exp(log_scores[outcome] - max_score);
  }
  // Compute log-sum-exp: log(sum(exp(x_i))) = max + log(sum(exp(x_i - max)))
  double lse = max_score + log(total_log_score);
  // Convert log probabilities back to regular probabilities (normalized)
  for (int outcome = 0; outcome < OUTCOMES; outcome++) {
    log_scores[outcome] = exp(log_scores[outcome] - lse);
  }

  // Return positive probability if it's higher, else return negative
  // probability as negative value
  double prob_negative = log_scores[NEGATIVE];
  double prob_positive = log_scores[POSITIVE];
  if (prob_positive >= prob_negative) {
    return (float)prob_positive;
  } else {
    return -(float)prob_negative;
  }
}

Cell nb_find_move(const Board* board, const NaiveBayesModel* model) {
  Player ai_player = board->current_player;
  Cell best_move = {-1, -1};
  float best_prob = -1.0;

  Cell empty_cells[SIZE * SIZE];
  int num_moves = find_empty_cells(board, empty_cells, SIZE * SIZE);

  Board board_copy;
  copy_board(board, &board_copy);
  // If AI is PLAYER_O, invert the board for evaluation
  if (ai_player == PLAYER_O) invert_board(&board_copy);

  // Evaluate moves
  for (int m = 0; m < num_moves; m++) {
    Cell cell = empty_cells[m];
    make_move(&board_copy, &cell);
    float score = naive_bayes(&board_copy, model);
    undo_move(&board_copy);
    // Update best move if this move has a better win probability
    if (score > best_prob) {
      best_prob = score;
      best_move = cell;
    }
  }

  return best_move;
}
