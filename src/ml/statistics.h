#ifndef STATISTICS_H
#define STATISTICS_H

#include <stddef.h>

#include "../shared/naive_bayes.h"
#include "dataset.h"

typedef struct {
  int tp;
  int tn;
  int fp;
  int fn;
} ConfusionMatrix;

typedef struct {
  float accuracy;
  float precision;
  float recall;
  float f1_score;
} Metrics;

typedef struct {
  Outcome actual;
  Outcome prediction;
} Prediction;

/**
 * @brief Evaluate the Naive Bayes model on the given dataset.
 * @param data_entries Pointer to an array of DataEntry structs.
 * @param data_entries_size Number of entries in the dataset.
 * @param model Pointer to the trained Naive Bayes model.
 * @return Pointer to an array of Prediction structs containing actual and
 *         predicted outcomes (same size as data_entries_size).
 */
Prediction* evaluate_model(const DataEntry* data_entries,
                           const size_t data_entries_size,
                           const NaiveBayesModel* model);

/**
 * @brief Calculate the confusion matrix from the predictions.
 * @param predictions Pointer to an array of Prediction structs.
 * @param size Number of predictions.
 * @return ConfusionMatrix struct containing true positives, true negatives,
 *         false positives, and false negatives.
 */
ConfusionMatrix calculate_confusion_matrix(const Prediction* predictions,
                                           const size_t size);

/**
 * @brief Calculate evaluation metrics from the confusion matrix.
 * @param cm ConfusionMatrix struct.
 * @return Metrics struct containing accuracy, precision, recall, and F1 score.
 */
Metrics calculate_metrics(const ConfusionMatrix cm);

#endif  // STATISTICS_H
