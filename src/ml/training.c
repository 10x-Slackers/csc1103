#include "training.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

NaiveBayesModel train_model(const DataEntry* data_entries,
                            const size_t data_entries_size) {
  NaiveBayesModel model;

  // Initialize the model with zeros
  memset(&model, 0, sizeof(NaiveBayesModel));
  // Initialize outcome counts
  size_t outcome_count[OUTCOMES] = {0};
  // Initialize state counts (outcome * row * col * state)
  size_t state_count[OUTCOMES][SIZE][SIZE][STATE] = {{{{0}}}};

  // Count all occurrences
  for (size_t i = 0; i < data_entries_size; i++) {
    // Outcome
    Outcome outcome = data_entries[i].outcome;
    outcome_count[outcome]++;
    // Cell states
    for (int row = 0; row < SIZE; row++) {
      for (int col = 0; col < SIZE; col++) {
        CellState state = data_entries[i].cells[row][col];
        state_count[outcome][row][col][state]++;
      }
    }
  }

  // Compute prior probabilities for each outcome with Laplace smoothing
  // P(outcome) = (count(outcome) + alpha) / (total_samples + alpha *
  //              num_outcomes)
  for (int outcome = 0; outcome < OUTCOMES; outcome++) {
    model.prior[outcome] = (double)(outcome_count[outcome] + ALPHA) /
                           (double)(data_entries_size + ALPHA * OUTCOMES);
  }

  // Compute likelihood probabilities for each cell state given an outcome with
  // Laplace smoothing
  // P(cell_state | outcome) = (count(cell_state | outcome) + alpha) /
  //                           (count(outcome) + alpha * num_states)
  for (int outcome = 0; outcome < OUTCOMES; outcome++) {
    for (int row = 0; row < SIZE; row++) {
      for (int col = 0; col < SIZE; col++) {
        for (int state = 0; state < STATE; state++) {
          model.likelihood[outcome][row][col][state] =
              (double)(state_count[outcome][row][col][state] + ALPHA) /
              (double)(outcome_count[outcome] + ALPHA * STATE);
        }
      }
    }
  }

  return model;
}

int save_model(const NaiveBayesModel* model, const char* model_path) {
  FILE* fp = fopen(model_path, "wb");
  if (fp == NULL) {
    fprintf(stderr, "Error: Failed to open output file path '%s'\n",
            model_path);
    return -1;
  }

  size_t written = fwrite(model, sizeof(NaiveBayesModel), 1, fp);
  fclose(fp);

  if (written != 1) {
    fprintf(stderr, "Error: Failed to write model to file path '%s'\n",
            model_path);
    return -1;
  }

  return 0;
}
