/**
 * @author: kitsuneez
 * @date: 2025-11-04
 * @description: Common functions used by algorithm and training of model.
 */

#include "ml.h"

/**
 * @brief: Calculates the probability scores for each outcome given a board
 * vector and a trained Naive Bayes model.
 *
 * @param board_vector  1D integer array (length 9) representing the
 *                      vectorized board
 * @param model         Pointer to the trained NaiveBayesModel
 * @return score_struct Containing the probability scores for negative and
 *                      positive outcomes
 */
score_struct probability(const int board_vector[9],
                         const NaiveBayesModel* model) {
  float log_scores[OUTCOMES] = {0.0};
  for (int outcome = 0; outcome < OUTCOMES; outcome++) {
    float s = log(model->prior[outcome]);
    for (int i = 0; i < CELLS; i++) {
      int cell_value = board_vector[i];
      s += log(model->likelihood[outcome][i][cell_value]);
    }
    log_scores[outcome] = s;
  }

  float max_score =
      log_scores[0] > log_scores[1] ? log_scores[0] : log_scores[1];
  float total_log_score = 0;
  for (int outcome = 0; outcome < OUTCOMES; outcome++) {
    total_log_score += exp(log_scores[outcome] - max_score);
  }
  float lse = max_score + log(total_log_score);
  for (int outcome = 0; outcome < OUTCOMES; outcome++) {
    log_scores[outcome] = exp(log_scores[outcome] - lse);
  }
  score_struct score = {.negative = log_scores[0], .positive = log_scores[1]};
  return score;
}
