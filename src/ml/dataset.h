#ifndef DATASET_H
#define DATASET_H

#include <stddef.h>

#include "../shared/board.h"
#include "../shared/naive_bayes.h"

#define DATASET_SHUFFLE_SEED 1234  // Fixed seed for reproducibility
#define MAX_DATASET_LINE_LENGTH 256
#define TRAINING_SPLIT_RATIO 0.8

typedef struct {
  CellState cells[SIZE][SIZE];
  Outcome outcome;
} DataEntry;

/**
 * @brief Process a dataset from the given file path.
 * @param filepath Path to the dataset file.
 * @param data_entries_size Pointer to size_t to store the number of data
 * entries.
 * @return Pointer to an array of DataEntry structs.
 */
DataEntry* process_dataset(const char* filepath, size_t* data_entries_size);

/**
 * @brief Shuffle the dataset entries in place for training/testing split.
 * @param data_entries Pointer to the array of DataEntry structs.
 * @param size Number of entries in the dataset.
 * @return 0 on success, -1 on failure.
 */
int shuffle_dataset(DataEntry* data_entries, size_t size);

#endif  // DATASET_H
