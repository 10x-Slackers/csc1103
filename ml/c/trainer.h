#ifndef TRAINER_H
#define TRAINER_H

#include "ml.h"

#define BUFFER 100
#define MAX_LINES 1000

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
  float true_positive_rate;
  float true_negative_rate;
  float f1_score;
  float false_positive_rate;
  float false_negative_rate;
  int total;
} confusion_matrix;

static DataEntry* process_dataset(const char* filename, int* dataentries_size);
static char*** convert_ttt_matrix(char** ttt);
static char** str_split(char* a_str, char a_delim);
static void shuffle(DataEntry* data_entries, int size);
static void vectorize(DataEntry data_entry, int board_vector[9], int formatted);
static int get_vector_value(const char* symbol);
static int free_memory(DataEntry* data_entries, int (*board_vectors)[9],
                       const char* error_message);
static int prepare_vectors(int (*board_vectors)[9], int* outcomes_vector,
                           const DataEntry* data_entries, int training_len);
static void train_model(const int (*board_vectors)[9],
                        const int* outcomes_vector, int training_len,
                        NaiveBayesModel* model);
static int save_model(NaiveBayesModel model);
static prediction_struct* test_model(const int (*board_vectors)[9],
                                     const int* outcomes_vector,
                                     int training_len, int dataset_size,
                                     const NaiveBayesModel* model);
static confusion_matrix calculate_confusion_matrix(
    const prediction_struct* predictions, int size);
static void print_confusion_matrix(confusion_matrix cm);
#endif  // TRAINER_H
