#include <stdio.h>
#include <stdlib.h>

#include "cli.h"
#include "dataset.h"
#include "statistics.h"
#include "training.h"

int main(int argc, char* argv[]) {
  ProgramMode mode = MODE_NONE;
  const char* dataset_path = NULL;
  const char* model_path = NULL;
  // Parse CLI arguments
  if (parse_arguments(argc, argv, &mode, &dataset_path, &model_path) != 0)
    return EXIT_FAILURE;

  // Parse dataset
  printf("Processing dataset: %s\n", dataset_path);
  size_t data_entries_size = 0;
  DataEntry* data_entries = process_dataset(dataset_path, &data_entries_size);
  if (data_entries == NULL) {
    fprintf(stderr, "Error: Failed to process dataset\n");
    return EXIT_FAILURE;
  }
  printf("Total number of data entries: %zu\n", data_entries_size);

  // Shuffle dataset
  srand(DATASET_SHUFFLE_SEED);
  if (shuffle_dataset(data_entries, data_entries_size) != 0) {
    fprintf(stderr, "Error: Failed to shuffle dataset\n");
    free(data_entries);
    return EXIT_FAILURE;
  }

  // Calculate training-testing split
  int ratio_percentage = (int)(TRAINING_SPLIT_RATIO * 100);
  printf("Training-testing split: %d - %d\n", ratio_percentage,
         100 - ratio_percentage);
  size_t training_split = (size_t)(data_entries_size * TRAINING_SPLIT_RATIO);

  if (mode == MODE_TRAIN) {
    printf("\n===== TRAINING MODE =====\n");
    printf("Training on %zu entries...\n", training_split);

    // Train model
    NaiveBayesModel model = train_model(data_entries, training_split);
    printf("Training completed.\n");

    // Save model
    printf("Saving model to %s...\n", model_path);
    if (save_model(&model, model_path) != 0) {
      fprintf(stderr, "Error: Failed to save model\n");
      free(data_entries);
      return EXIT_FAILURE;
    }
    printf("Model saved successfully.\n");
  }

  if (mode == MODE_STATS) {
    printf("\n===== EVALUATION MODE =====\n");

    // Load model
    printf("Loading model from %s...\n", model_path);
    NaiveBayesModel model;
    if (load_nb_model(&model, model_path) != 0) {
      fprintf(stderr, "Error: Failed to load model\n");
      free(data_entries);
      return EXIT_FAILURE;
    }
    printf("Model loaded successfully.\n");

    // Evaluate model
    size_t testing_split = data_entries_size - training_split;
    printf("Evaluating on %zu entries...\n", testing_split);
    const Prediction* predictions =
        evaluate_model(&data_entries[training_split], testing_split, &model);
    if (predictions == NULL) {
      fprintf(stderr, "Error: Failed to evaluate model\n");
      free(data_entries);
      return EXIT_FAILURE;
    }
    printf("Evaluation completed.\n");

    // Calculate statistics
    ConfusionMatrix cm = calculate_confusion_matrix(predictions, testing_split);
    Metrics metrics = calculate_metrics(cm);

    // Print results
    printf("\n===== Confusion Matrix =====\n");
    printf("TP: %d\tTN: %d\n", cm.tp, cm.tn);
    printf("FP: %d\tFN: %d\n", cm.fp, cm.fn);

    printf("\n===== Metrics =====\n");
    printf("Accuracy: %.4f\n", metrics.accuracy);
    printf("Precision: %.4f\n", metrics.precision);
    printf("Recall: %.4f\n", metrics.recall);
    printf("F1 Score: %.4f\n", metrics.f1_score);
    free((void*)predictions);
  }

  free(data_entries);
  return EXIT_SUCCESS;
}
