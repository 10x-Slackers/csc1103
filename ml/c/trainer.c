/**
 * @author: kitsuneez
 * @date: 2025-11-04
 * @description: Trains and tests a Naive Bayes classifier for Tic-Tac-Toe
 * outcomes
 */

#include "trainer.h"

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
  int testing_len = dataset_size * 0.2;
  if (dataset_size % 10 != 0) {
    testing_len += 1;
  }
  int (*board_vectors)[9] = malloc(dataset_size * sizeof(*board_vectors));
  if (!board_vectors) {
    return free_memory(data_entries, NULL,
                       "Error allocating memory for board vectors");
  }
  int outcomes_vector[dataset_size];
  prepare_vectors(board_vectors, outcomes_vector, data_entries, dataset_size);
  NaiveBayesModel model = {.prior = {0.0}, .likelihood = {{{0.0}}}};
  train_model(board_vectors, outcomes_vector, training_len, &model);
  int res = save_model(model);
  if (res != 0) {
    return free_memory(data_entries, board_vectors, "Error saving model");
  }
  prediction_struct* test_result = test_model(
      board_vectors, outcomes_vector, training_len, dataset_size, &model);
  if (test_result == NULL) {
    return free_memory(data_entries, board_vectors,
                       "Error during model testing");
  }

  confusion_matrix cm =
      calculate_confusion_matrix(&test_result[0], testing_len);
  print_confusion_matrix(cm);
  free(test_result);
  free_memory(data_entries, board_vectors, NULL);
  return 0;
}

/**
 * @brief: Calculates the confusion matrix from the predictions.
 *
 * @param predictions: array of prediction_struct containing actual and
 * predicted outcomes
 * @param size: size of the predictions array
 *
 * @return: confusion_matrix struct containing the calculated metrics
 */
confusion_matrix calculate_confusion_matrix(
    const prediction_struct* predictions, int size) {
  confusion_matrix cm = {0};
  for (int i = 0; i < size; i++) {
    int actual = predictions[i].outcome;
    int predicted = predictions[i].prediction;
    if (actual == 1 && predicted == 1) {
      cm.tp++;
    } else if (actual == 0 && predicted == 0) {
      cm.tn++;
    } else if (actual == 0 && predicted == 1) {
      cm.fp++;
    } else if (actual == 1 && predicted == 0) {
      cm.fn++;
    }
  }
  cm.total = size;
  cm.prevalence = (float)(cm.tp + cm.fn) / size;
  cm.accuracy = (float)(cm.tp + cm.tn) / size;
  cm.true_positive_rate =
      cm.tp + cm.fn > 0 ? (float)cm.tp / (cm.tp + cm.fn) : 0.0;
  cm.true_negative_rate =
      cm.tn + cm.fp > 0 ? (float)cm.tn / (cm.tn + cm.fp) : 0.0;
  cm.positive_predictive_value =
      cm.tp + cm.fp > 0 ? (float)cm.tp / (cm.tp + cm.fp) : 0.0;
  cm.negative_predictive_value =
      cm.tn + cm.fn > 0 ? (float)cm.tn / (cm.tn + cm.fn) : 0.0;
  cm.f1_score = (2 * cm.tp + cm.fp + cm.fn) > 0
                    ? (2.0 * cm.tp) / (2 * cm.tp + cm.fp + cm.fn)
                    : 0.0;
  cm.false_positive_rate =
      cm.fp + cm.tn > 0 ? (float)cm.fp / (cm.fp + cm.tn) : 0.0;
  cm.false_negative_rate =
      cm.fn + cm.tp > 0 ? (float)cm.fn / (cm.fn + cm.tp) : 0.0;
  cm.balanced_accuracy = (cm.true_positive_rate + cm.true_negative_rate) / 2.0;
  return cm;
}

/**
 * @brief: Prints the confusion matrix and its metrics.
 *
 * @param cm: confusion_matrix struct containing the metrics to be printed
 *
 * @return: None
 */
void print_confusion_matrix(confusion_matrix cm) {
  printf("Confusion Matrix:\n");
  printf("TP: %.0f, TN: %.0f, FP: %.0f, FN: %.0f\n", cm.tp, cm.tn, cm.fp,
         cm.fn);
  printf("Prevalence: %.4f\n", cm.prevalence);
  printf("Accuracy: %.4f\n", cm.accuracy);
  printf("Balanced Accuracy: %.4f\n", cm.balanced_accuracy);
  printf("Positive Predictive Value: %.4f\n", cm.positive_predictive_value);
  printf("Negative Predictive Value: %.4f\n", cm.negative_predictive_value);
  printf("True Positive Rate: %.4f\n", cm.true_positive_rate);
  printf("True Negative Rate: %.4f\n", cm.true_negative_rate);
  printf("F1 Score: %.4f\n", cm.f1_score);
  printf("False Positive Rate: %.4f\n", cm.false_positive_rate);
  printf("False Negative Rate: %.4f\n", cm.false_negative_rate);
}

/**
 * @brief: Tests the trained model on the held-out portion of the dataset.
 *
 * @param board_vectors   2D array (N x 9) of vectorized boards
 * @param outcomes_vector Array of ground-truth outcomes
 * @param training_len    Number of training examples (index where testing
 *                        begins)
 * @param dataset_size    Total number of examples in dataset
 * @param model           Pointer to a trained NaiveBayesModel
 * @return prediction_struct* Pointer to a malloc'd array of prediction_struct
 *         of length (dataset_size - training_len). Caller must free it.
 */
prediction_struct* test_model(const int (*board_vectors)[9],
                              const int* outcomes_vector, int training_len,
                              int dataset_size, const NaiveBayesModel* model) {
  prediction_struct* result =
      malloc((dataset_size - training_len) * sizeof(prediction_struct));
  if (!result) {
    printf("Error allocating memory for test results\n");
    return NULL;
  }
  int cur = training_len;
  for (int i = 0; i < dataset_size - training_len; i++) {
    result[i].outcome = outcomes_vector[cur];
    score_struct predict_res = probability(board_vectors[cur], model);
    result[i].prediction = predict_res.negative > predict_res.positive ? 0 : 1;
    cur++;
  }
  return result;
}

/**
 * @brief: Saves the trained model to a binary file.
 *
 * @param model: NaiveBayesModel instance to be saved. The function writes the
 *               model to the path defined by MODEL_FILE.
 *
 * @return int: 0 on success, 1 on failure
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
  for (int i = 0; i < training_len; i++) {
    int outcome = outcomes_vector[i];
    // keep track of outcome
    outcome_count[outcome] += 1;
    for (int cell = 0; cell < CELLS; cell++) {
      // increment outcome-cell-state count
      int state = board_vectors[i][cell];
      cells_counts[outcome][cell][state] += 1;
    }
  }
  // compute prior probabilities with Laplace smoothing
  for (int outcome = 0; outcome < OUTCOMES; outcome++) {
    // Formula used: (number of occurrences of outcome + ALPHA) /
    // (total number of training examples + number of outcomes * ALPHA)
    model->prior[outcome] = ((float)outcome_count[outcome] + ALPHA) /
                            (training_len + OUTCOMES * ALPHA);
  }

  // Compute likelihood probabilities with Laplace smoothing
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
 * @brief: Prepares the board vectors and outcomes vector from data entries.
 *
 * @param board_vectors   2D array (N x 9) to hold the vectorized boards
 * @param outcomes_vector Array to hold the vectorized outcomes
 * @param data_entries    Array of DataEntry objects to be vectorized
 * @param training_len    Number of entries to process from data_entries
 * @return int: 0 on success, non-zero on failure
 */
int prepare_vectors(int (*board_vectors)[9], int* outcomes_vector,
                    const DataEntry* data_entries, int training_len) {
  for (int i = 0; i < training_len; i++) {
    // temporary board vector to hold the vectorized board
    int board_vector[9];
    // vectorize the board and store it in the flattened 3D array
    vectorize(data_entries[i], board_vector, 0);
    // copy temporary board vector to allocated flatten 3D array
    for (int cell = 0; cell < 9; cell++) {
      board_vectors[i][cell] = board_vector[cell];
    }
    outcomes_vector[i] = get_vector_value(data_entries[i].outcome);
  }
  return 0;
}

/**
 * @brief: Free allocated memory and optionally print an error message.
 *
 * @param data_entries   Array of DataEntry objects to be freed (may be NULL)
 * @param board_vectors  2D array of board vectors to be freed (may be NULL)
 * @param error_message  Optional error message; if non-NULL it will be
 *                       printed to stderr
 *
 * @return: 1
 */
int free_memory(DataEntry* data_entries, int (*board_vectors)[9],
                const char* error_message) {
  if (error_message) {
    fprintf(stderr, "%s\n", error_message);
  }
  if (board_vectors == NULL) {
    printf("Freeing data entries only\n");
    free(data_entries);
    return 1;
  }
  // if we have reached depth 0, free encased arrays and return
  free(board_vectors);
  free(data_entries);
  return 1;
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
void vectorize(DataEntry data_entry, int board_vector[9], int formatted) {
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

/**
 * @brief: Parses the dataset file and returns an array of DataEntry structs.
 *
 * @param filename: The name of the file to parse.
 * @return: DataEntry*: Pointer to an array of DataEntry structs.
 */
DataEntry* process_dataset(const char* filename, int* dataentries_size) {
  FILE* fp;
  // Open the file for reading
  fp = fopen(filename, "r");
  // Check if file opened successfully
  if (fp == NULL) {
    printf("Could not open file %s\n", filename);
    return NULL;
  }
  // Allocate memory for data entries
  DataEntry* data_entries = (DataEntry*)malloc(MAX_LINES * sizeof(DataEntry));
  // if allocation fails for data entries
  if (!data_entries) {
    printf("Allocation Error\n");
    fclose(fp);
    return NULL;
  }
  // number of characters to read per line
  char buffer[BUFFER];
  int line_count = 0;
  // Read each line from the file
  while (fgets(buffer, sizeof(buffer), fp)) {
    // Allocate memory for each row of the board
    data_entries[line_count].board = (char***)malloc(3 * sizeof(char**));
    for (int i = 0; i < 3; i++) {
      // Allocate memory for each cell of each row
      data_entries[line_count].board[i] = (char**)malloc(3 * sizeof(char*));
    }
    // if allocation fails for any row or cell
    if (!data_entries[line_count].board) {
      printf("Allocation Error\n");
      for (int j = 0; j < line_count; j++) {
        free(data_entries[j].board);
      }
      free(data_entries);
      fclose(fp);
      return NULL;
    }
    // splits the line by commas
    char** row = str_split(buffer, ',');
    // converts the row into a 3x3 board
    char*** board = convert_ttt_matrix(row);
    // gets the outcome from the row
    char* outcome = row[FEATURES];
    // creates a data entry and adds it to the array
    DataEntry entry;
    entry.board = board;
    entry.outcome = outcome;
    data_entries[line_count] = entry;
    line_count++;
  }
  fclose(fp);
  *dataentries_size = line_count;
  return data_entries;
}

/**
 * @brief: Converts a flat array of 9 elements into a 3x3 Tic-Tac-Toe board.
 *
 * @param row: The flat array of 9 elements.
 * @return: char***: Pointer to a 3x3 array representing the Tic-Tac-Toe board.
 */
char*** convert_ttt_matrix(char** row) {
  char*** formatted = (char***)malloc(FEATURES * sizeof(char**));
  if (!formatted) {
    printf("Allocation Error\n");
    return NULL;
  }
  for (int i = 0; i < 3; i++) {
    formatted[i] = (char**)malloc(3 * sizeof(char*));
    for (int j = 0; j < 3; j++) {
      formatted[i][j] = row[i * 3 + j];
    }
  }
  return formatted;
}

/**
 * @brief: Splits a string into an array of strings based on a delimiter.
 *
 * @param a_str: The string to split.
 * @param a_delim: The delimiter character.
 * @return: char**: Pointer to an array of strings.
 */
char** str_split(char* a_str, char a_delim) {
  char** result = 0;
  size_t count = 0;
  char* tmp = a_str;
  char const* last_comma = 0;
  char delim[2];
  delim[0] = a_delim;
  delim[1] = 0;

  while (*tmp) {
    if (a_delim == *tmp) {
      count++;
      last_comma = tmp;
    }
    tmp++;
  }
  count += last_comma < (a_str + strlen(a_str) - 1);
  count++;

  result = (char**)malloc(sizeof(char*) * count);

  if (result) {
    size_t idx = 0;
    char const* value = strtok(a_str, delim);

    while (value) {
      *(result + idx++) = strdup(value);
      value = strtok(0, delim);
    }
    *(result + idx) = 0;
  }

  return result;
}
