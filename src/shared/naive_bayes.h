#ifndef NAIVE_BAYES_H
#define NAIVE_BAYES_H

#include "board.h"

#define OUTCOMES 2  // Negative (loss/draw), Positive (win)
#define STATE 3     // BLANK, X, O

typedef enum { NEGATIVE, POSITIVE } Outcome;

typedef struct {
  double prior[OUTCOMES];
  double likelihood[OUTCOMES][SIZE][SIZE][STATE];
} NaiveBayesModel;

/**
 * @brief Load a Naive Bayes model from embedded binary data.
 * @param model Pointer to the NaiveBayesModel structure to load data into.
 * @param model_path Path to the binary model file.
 * @return int 0 on success, -1 on failure.
 */
int load_nb_model(NaiveBayesModel* model, const char* model_path);

/**
 * @brief Evaluate the board state using the Naive Bayes model.
 * @param board Pointer to the Board structure.
 * @param model Pointer to the Naive Bayes Model.
 * @return float Positive probability if positive outcome is more likely,
 *               negative probability (as negative value) if negative outcome
 */
float naive_bayes(Board* board, const NaiveBayesModel* model);

/**
 * @brief Select a move for the AI player using the Naive Bayes model.
 * @param board Pointer to the Board structure.
 * @param model Pointer to the Naive Bayes Model.
 * @return Cell The selected move.
 */
Cell nb_find_move(const Board* board, const NaiveBayesModel* model);

#endif  // NAIVE_BAYES_H
