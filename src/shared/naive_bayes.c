#include "naive_bayes.h"

#include <string.h>

int load_nb_model(NaiveBayesModel* model, const unsigned char* data,
                  unsigned int data_len) {
  if (data_len != sizeof(NaiveBayesModel)) {
    return -1;
  }
  memcpy(model, data, sizeof(NaiveBayesModel));
  return 0;
}

static float naive_bayes(Board* board, const NaiveBayesModel* model) {
  float log_scores[OUTCOMES] = {0.0};
  // Calculate log probability for each outcome
  for (int outcome = 0; outcome < OUTCOMES; outcome++) {
    // Start with log prior
    float score = logf(model->prior[outcome]);
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
  float max_score =
      log_scores[0] > log_scores[1] ? log_scores[0] : log_scores[1];
  float total_log_score = 0;
  for (int outcome = 0; outcome < OUTCOMES; outcome++) {
    total_log_score += expf(log_scores[outcome] - max_score);
  }
  // Compute log-sum-exp: log(sum(exp(x_i))) = max + log(sum(exp(x_i - max)))
  float lse = max_score + logf(total_log_score);
  // Convert log probabilities back to regular probabilities (normalized)
  for (int outcome = 0; outcome < OUTCOMES; outcome++) {
    log_scores[outcome] = expf(log_scores[outcome] - lse);
  }

  // Return positive probability if it's higher, else return negative
  // probability as negative value
  float prob_negative = log_scores[0];
  float prob_positive = log_scores[1];
  if (prob_positive >= prob_negative) {
    return prob_positive;
  } else {
    return -prob_negative;
  }
}
