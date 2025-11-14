#include "statistics.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

Prediction* evaluate_model(const DataEntry* data_entries,
                           const size_t data_entries_size,
                           const NaiveBayesModel* model) {
  if (!data_entries || !model || data_entries_size == 0) return NULL;

  Prediction* predictions =
      (Prediction*)malloc(data_entries_size * sizeof(Prediction));
  if (!predictions) {
    return NULL;
  }

  for (size_t i = 0; i < data_entries_size; i++) {
    // Copy cells to board structure
    Board board;
    // Use memcpy to copy the entire cell grid efficiently
    memcpy(board.cells, data_entries[i].cells, sizeof(board.cells));

    // Predict outcome using the Naive Bayes model
    float prob = naive_bayes(&board, model);
    // Assign actual and predicted outsomes
    predictions[i].actual = data_entries[i].outcome;
    predictions[i].prediction = (prob >= 0) ? POSITIVE : NEGATIVE;
  }

  return predictions;
}

ConfusionMatrix calculate_confusion_matrix(const Prediction* predictions,
                                           const size_t size) {
  ConfusionMatrix cm = {0, 0, 0, 0};

  for (size_t i = 0; i < size; i++) {
    if (predictions[i].actual == POSITIVE &&
        predictions[i].prediction == POSITIVE) {
      cm.tp++;
    } else if (predictions[i].actual == NEGATIVE &&
               predictions[i].prediction == NEGATIVE) {
      cm.tn++;
    } else if (predictions[i].actual == NEGATIVE &&
               predictions[i].prediction == POSITIVE) {
      cm.fp++;
    } else if (predictions[i].actual == POSITIVE &&
               predictions[i].prediction == NEGATIVE) {
      cm.fn++;
    }
  }

  return cm;
}

Metrics calculate_metrics(const ConfusionMatrix cm) {
  Metrics metrics = {0.0f, 0.0f, 0.0f, 0.0f};
  int total = cm.tp + cm.tn + cm.fp + cm.fn;
  if (total == 0) {
    return metrics;
  }

  // Accuracy: (tp + tn) / total
  metrics.accuracy = (float)(cm.tp + cm.tn) / (float)total;
  // Precision: tp / (tp + fp)
  if (cm.tp + cm.fp != 0)
    metrics.precision = (float)cm.tp / (float)(cm.tp + cm.fp);
  // Recall: tp / (tp + fn)
  if (cm.tp + cm.fn != 0)
    metrics.recall = (float)cm.tp / (float)(cm.tp + cm.fn);
  // F1 Score: 2 * (precision * recall) / (precision + recall)
  if (metrics.precision + metrics.recall != 0)
    metrics.f1_score = 2.0 * (metrics.precision * metrics.recall) /
                       (metrics.precision + metrics.recall);

  return metrics;
}
