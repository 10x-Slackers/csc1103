#include <stdio.h>
#include <stdlib.h>

#include "dataset.h"
#define OUTCOMES 2
#define CELLS 9
#define STATE 3
#define ALPHA 1.0
#define MODEL_FILE "ml/naive_bayes.bin"

typedef struct {
  float prior[OUTCOMES];
  float likelihood[OUTCOMES][CELLS][STATE];
} NaiveBayesModel;

static void shuffle(DataEntry* data_entries, int size);
static void vectorize(DataEntry data_entry, int board_vector[9]);
static int get_vector_value(const char* symbol);
static int handle_allocation_error(DataEntry* data_entries,
                                   int (*board_vectors)[9], int depth);
static int prepare_vectors(int (*board_vectors)[9], int* outcomes_vector,
                           const DataEntry* data_entries, int training_len);
static void train_model(const int (*board_vectors)[9],
                        const int* outcomes_vector, int training_len,
                        NaiveBayesModel* model);
static int save_model(NaiveBayesModel model);

/**
 * @brief: Main entry point of the program.
 *
 * @return: int: 0 on success, 1 on failure
 */
int main() {
  int dataset_size;
  DataEntry* data_entries =
      process_dataset("dataset/tic-tac-toe.data", &dataset_size);
  if (data_entries == NULL) {
    return 1;
  }

  shuffle(data_entries, dataset_size);
  int training_len = dataset_size * 0.8;
  // int testing_len = dataset_size * 0.2;
  // if (dataset_size % 10 != 0) {
  //   testing_len += 1;
  // }
  int(*board_vectors)[9] = malloc(training_len * sizeof(*board_vectors));
  if (!board_vectors) {
    return handle_allocation_error(data_entries, NULL, 0);
  }
  int outcomes_vector[training_len];
  prepare_vectors(board_vectors, outcomes_vector, data_entries, training_len);
  NaiveBayesModel model = {.prior = {0.0}, .likelihood = {{{0.0}}}};
  train_model(board_vectors, outcomes_vector, training_len, &model);
  int res = save_model(model);
  if (res != 0) {
    return handle_allocation_error(data_entries, board_vectors,
                                   training_len - 1);
  }
  return 0;
}

/**
 * @brief: Saves the trained model to a binary file.
 *
 * @param prior: Array of prior probabilities for each outcome.
 * @param likelihood: 3D array of likelihood probabilities for each outcome,
 * cell, and state.
 *
 * @return: int: 0 on success, 1 on failure
 */
int save_model(NaiveBayesModel model) {
  FILE* fp = fopen(MODEL_FILE, "wb");
  if (fp == NULL) {
    printf("Could not open file to save model\n");
    return 1;
  }
  fwrite(&model, sizeof(NaiveBayesModel), 1, fp);
  fclose(fp);
  return 0;
}

/**
 * @brief: Train the Naive Bayes classifier by updating counts based on the
 * provided data.
 *
 * @param board_vectors: 3D array of vectorized boards
 * @param outcomes_vector: array of vectorized outcomes
 * @param training_len: length of training data
 * @param model: NaiveBayesModel struct to hold the trained model
 *
 * @return: None
 */
void train_model(const int (*board_vectors)[9], const int* outcomes_vector,
                 int training_len, NaiveBayesModel* model) {
  // initializes count for positive and negative classes
  int outcome_count[OUTCOMES] = {0};
  // count of each occurence for each possible state (0,1,2)
  int cells_counts[OUTCOMES][CELLS][STATE] = {{{0}}};
  // count occurences of every outcome and cell state
  for (int i = 0; i < training_len - 1; i++) {
    int outcome = outcomes_vector[i];
    // keep track of outcome
    outcome_count[outcome] += 1;
    for (int cell = 0; cell < CELLS; cell++) {
      // increment outcome-cell-state count
      int state = board_vectors[i][cell];
      cells_counts[outcome][cell][state] += 1;
    }
  }
  // computer prior probabilities with Laplace smoothing
  for (int outcome = 0; outcome < OUTCOMES; outcome++) {
    // Formula used: (number of occurrences of outcome + ALPHA) /
    // (total number of training examples + number of outcomes * ALPHA)
    model->prior[outcome] = ((float)outcome_count[outcome] + ALPHA) /
                            (training_len + OUTCOMES * ALPHA);
  }

  // Computer likelihood probabilities with Laplace smoothing
  for (int outcome = 0; outcome < OUTCOMES; outcome++) {
    for (int cell = 0; cell < CELLS; cell++) {
      for (int state = 0; state < STATE; state++) {
        model->likelihood[outcome][cell][state] =
            ((float)cells_counts[outcome][cell][state] + ALPHA) /
            (outcome_count[outcome] + STATE * ALPHA);
      }
    }
  }
}

/**
 * @brief: prepares the board vectors and outcomes vector from data entries
 * @param board_vectors: flattened 3D array to hold the vectorized boards
 * @param outcomes_vector: array to hold the vectorized outcomes
 * @param data_entries: array of data entries to be vectorized
 * @param training_len: length of training data
 * @return: int: 0 on success, 1 on failure
 */
int prepare_vectors(int (*board_vectors)[9], int* outcomes_vector,
                    const DataEntry* data_entries, int training_len) {
  for (int i = 0; i < training_len; i++) {
    // temporary board vector to hold the vectorized board
    int board_vector[9];
    // vectorize the board and store it in the flattened 3D array
    vectorize(data_entries[i], board_vector);
    // copy temporary board vector to allocated flatten 3D array
    for (int cell = 0; cell < 9; cell++) {
      board_vectors[i][cell] = board_vector[cell];
    }
    outcomes_vector[i] = get_vector_value(data_entries[i].outcome);
  }
  return 0;
}

/**
 * @brief: handles allocation errors by freeing previously allocated memory
 *
 * @param data_entries: array of data entries to be freed
 * @param board_vectors: 2D array of board vectors to be freed
 * @param depth: current depth of recursion
 *
 * @return: int: 1 on error
 */
int handle_allocation_error(DataEntry* data_entries, int (*board_vectors)[9],
                            int depth) {
  if (!board_vectors) {
    free(data_entries);
    return 1;
  }
  printf("Allocation Error\n");
  // free all cells in this depth safely
  if (board_vectors[depth]) {
    free(board_vectors[depth]);
  }
  // if we have reached depth 0, free encased arrays and return
  if (depth == 0) {
    free(board_vectors);
    free(data_entries);
    return 1;
  }
  // recursively free previous depths
  return handle_allocation_error(data_entries, board_vectors, depth - 1);
}

/**
 * @brief: shuffles the given array of Dataentry so array will be 'randomized"
 *
 * @param data_entries: list of data entries to be shuffled
 * @param size: size of array
 *
 * @return None
 */
void shuffle(DataEntry* data_entries, int size) {
  if (size > 1) {
    for (int i = 0; i < size - 1; i++) {
      int j = i + rand() % (size - i);
      DataEntry temp = data_entries[j];
      data_entries[j] = data_entries[i];
      data_entries[i] = temp;
    }
  }
}

/**
 * @brief: vectorizes a single data entry's board into a 1D integer array
 *
 * @param data_entry: data entry to be vectorized
 * @param board_vector: 1D integer array to hold the vectorized board
 *
 * @return: None
 */
void vectorize(DataEntry data_entry, int board_vector[9]) {
  for (int i = 0; i < 9; i++) {
    const char* symbol = NULL;
    if (data_entry.board && data_entry.board[i / 3] &&
        data_entry.board[i / 3][i % 3])
      symbol = data_entry.board[i / 3][i % 3];
    if (!symbol) symbol = "";
    board_vector[i] = get_vector_value(symbol);
  }
}

/**
 * @brief: converts a symbol to its corresponding integer value
 *
 * @param symbol: symbol to be converted
 *
 * @return: int: corresponding integer value of the symbol
 */
int get_vector_value(const char* symbol) {
  if (strcmp(symbol, "positive\n") == 0 || strcmp(symbol, "positive") == 0)
    return 1;
  if (strcmp(symbol, "negative\n") == 0 || strcmp(symbol, "negative") == 0)
    return 0;
  if (strcmp(symbol, "x") == 0) return 1;
  if (strcmp(symbol, "o") == 0) return 2;
  if (strcmp(symbol, "b") == 0) return 0;
  return -1;
}
