/**
 * @author: kitsuneez
 * @date: 2025-11-04
 * @description: Trains and tests a Naive Bayes classifier for Tic-Tac-Toe
 * outcomes
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 1000
#define BUFFER 100
#define OUTCOMES 2
#define CELLS 9
#define STATE 3
#define ALPHA 1.0
#define FEATURES 9
#define MODEL_FILE "ml/naive_bayes.bin"

typedef struct {
  float negative;
  float positive;
} score_struct;

typedef struct {
  char*** board;
  char* outcome;
} DataEntry;

typedef struct {
  float prior[OUTCOMES];
  float likelihood[OUTCOMES][CELLS][STATE];
} NaiveBayesModel;

typedef struct {
  int outcome;
  int prediction;
} prediction_struct;

typedef struct {
  float tn;
  float tp;
  float fp;
  float fn;
  float prevalence;
  float accuracy;
  float balanced_accuracy;
  float positive_predictive_value;
  float negative_predictive_value;
  float recall;
  float specificity;
  float f1_score;
  float false_positive_rate;
  float false_negative_rate;
  int total;
} statistics;

static DataEntry* process_dataset(const char* filename, int* dataentries_size);
static char*** convert_ttt_matrix(char** ttt);
static char** str_split(char* a_str, const char a_delim);
static void shuffle(DataEntry* data_entries, int size);
static void vectorize(DataEntry data_entry, int board_vector[9], int formatted);
static int get_vector_value(const char* symbol);
static int free_memory(DataEntry* data_entries, int (*board_vectors)[9],
                       const char* error_message);
static int prepare_vectors(int (*board_vectors)[9], int* outcomes_vector,
                           const DataEntry* data_entries,
                           const int training_len);
static void train_model(const int (*board_vectors)[9],
                        const int* outcomes_vector, const int training_len,
                        NaiveBayesModel* model);
static int save_model(const NaiveBayesModel model);
static prediction_struct* test_model(const int (*board_vectors)[9],
                                     const int* outcomes_vector,
                                     int training_len, int dataset_size,
                                     const NaiveBayesModel* model);
static statistics calculate_statistics(const prediction_struct* predictions,
                                       const int size);
static void print_statistics(const statistics stats);
score_struct probability(const int board_vector[9],
                         const NaiveBayesModel* model);
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
  int(*board_vectors)[9] = malloc(dataset_size * sizeof(*board_vectors));
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

  statistics stats = calculate_statistics(&test_result[0], testing_len);
  print_statistics(stats);
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
statistics calculate_statistics(const prediction_struct* predictions,
                                const int size) {
  statistics stats = {0};
  for (int i = 0; i < size; i++) {
    int actual = predictions[i].outcome;
    int predicted = predictions[i].prediction;
    if (actual == 1 && predicted == 1) {
      stats.tp++;
    } else if (actual == 0 && predicted == 0) {
      stats.tn++;
    } else if (actual == 0 && predicted == 1) {
      stats.fp++;
    } else if (actual == 1 && predicted == 0) {
      stats.fn++;
    }
  }
  stats.total = size;
  stats.prevalence = (float)(stats.tp + stats.fn) / size;
  stats.accuracy = (float)(stats.tp + stats.tn) / size;
  stats.recall =
      stats.tp + stats.fn > 0 ? (float)stats.tp / (stats.tp + stats.fn) : 0.0;
  stats.specificity =
      stats.tn + stats.fp > 0 ? (float)stats.tn / (stats.tn + stats.fp) : 0.0;
  stats.positive_predictive_value =
      stats.tp + stats.fp > 0 ? (float)stats.tp / (stats.tp + stats.fp) : 0.0;
  stats.negative_predictive_value =
      stats.tn + stats.fn > 0 ? (float)stats.tn / (stats.tn + stats.fn) : 0.0;
  stats.f1_score = (2 * stats.tp + stats.fp + stats.fn) > 0
                       ? (2.0 * stats.tp) / (2 * stats.tp + stats.fp + stats.fn)
                       : 0.0;
  stats.false_positive_rate =
      stats.fp + stats.tn > 0 ? (float)stats.fp / (stats.fp + stats.tn) : 0.0;
  stats.false_negative_rate =
      stats.fn + stats.tp > 0 ? (float)stats.fn / (stats.fn + stats.tp) : 0.0;
  stats.balanced_accuracy = (stats.recall + stats.specificity) / 2.0;
  return stats;
}

/**
 * @brief: Prints the confusion matrix and its metrics.
 *
 * @param cm: confusion_matrix struct containing the metrics to be printed
 *
 * @return: None
 */
void print_statistics(const statistics stats) {
  printf("Confusion Matrix:\n");
  printf("TP: %.0f, TN: %.0f, FP: %.0f, FN: %.0f\n", stats.tp, stats.tn,
         stats.fp, stats.fn);
  printf("Prevalence: %.4f\n", stats.prevalence);
  printf("Accuracy: %.4f\n", stats.accuracy);
  printf("Balanced Accuracy: %.4f\n", stats.balanced_accuracy);
  printf("Positive Predictive Value: %.4f\n", stats.positive_predictive_value);
  printf("Negative Predictive Value: %.4f\n", stats.negative_predictive_value);
  printf("Recall: %.4f\n", stats.recall);
  printf("Specificity: %.4f\n", stats.specificity);
  printf("F1 Score: %.4f\n", stats.f1_score);
  printf("False Positive Rate: %.4f\n", stats.false_positive_rate);
  printf("False Negative Rate: %.4f\n", stats.false_negative_rate);
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
int save_model(const NaiveBayesModel model) {
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
                 const int training_len, NaiveBayesModel* model) {
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
char** str_split(char* a_str, const char a_delim) {
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
