#ifndef NAIVE_BAYES_H
#define NAIVE_BAYES_H

#include <math.h>

#include "board.h"

// Positive (win), negative (draw or loss)
#define OUTCOMES 2
// BLANK, X, O
#define STATE 3
// Embedded model file (done via xxd -i)
extern unsigned char weights_bin[];
extern unsigned int weights_bin_len;

typedef struct {
  float prior[OUTCOMES];
  double likelihood[OUTCOMES][SIZE][SIZE][STATE];
} NaiveBayesModel;

/**
 * @brief Load a Naive Bayes model from embedded binary data.
 * @param model Pointer to the NaiveBayesModel structure to load data into.
 * @param data Pointer to the embedded binary data (weights_bin).
 * @param data_len Length of the embedded binary data (weights_bin_len).
 * @return int 0 on success, -1 on failure.
 */
int load_nb_model(NaiveBayesModel* model, const unsigned char* data,
                  unsigned int data_len);

/**
 * @brief Compute log-sum-exp of two log values to prevent underflow.
 * @param log_a Logarithm of the first value.
 * @param log_b Logarithm of the second value.
 * @return float Logarithm of the sum of the exponentials of the inputs.
 */
static float log_sum_exp(float log_a, float log_b);

/**
 * @brief Evaluate the board state using the Naive Bayes model.
 * @param board Pointer to the Board structure.
 * @param model Pointer to the Naive Bayes Model.
 * @return float Positive probability if positive outcome is more likely,
 *               negative probability (as negative value) if negative outcome
 */
static float naive_bayes(Board* board, const NaiveBayesModel* model);

#endif  // NAIVE_BAYES_H
