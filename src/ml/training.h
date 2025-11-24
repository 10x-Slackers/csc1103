#ifndef TRAINING_H
#define TRAINING_H

#include <stddef.h>

#include "../shared/naive_bayes.h"
#include "dataset.h"

// OUTCOME and STATE are defined in naive_bayes.h
#define ALPHA 1.0   // Laplace smoothing factor
#define FEATURES 9  // 3x3 Tic-Tac-Toe board has 9 features

/**
 * @brief Trains a Naive Bayes classifier for Tic-Tac-Toe using the provided
 * dataset.
 * @param data_entries Pointer to an array of DataEntry structs containing the
 * training data.
 * @param data_entries_size Number of entries in the training data.
 * @return NaiveBayesModel The trained model containing prior and likelihood
 *         probabilities.
 */
NaiveBayesModel train_model(const DataEntry* data_entries,
                            const size_t data_entries_size);

/**
 * @brief Saves the trained Naive Bayes model (struct) to a binary file.
 * @param model Pointer to the Naive Bayes model.
 * @param model_path Path to the output binary file.
 * @return int 0 on success, -1 on failure.
 */
int save_model(const NaiveBayesModel* model, const char* model_path);

#endif  // TRAINING_H
