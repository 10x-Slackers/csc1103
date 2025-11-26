/**
 * @file dataset.c
 * @brief Functions for processing the dataset.
 * @authors kitsuneez
 * @date 2025-11-08
 */
#include "dataset.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Process a single line from the dataset file into a DataEntry struct.
 * @param line The line from the dataset file.
 * @param entry Pointer to the DataEntry struct to populate.
 * @return 0 on success, -1 on failure.
 */
static int process_line(char* line, DataEntry* entry) {
  if (!entry) return -1;

  const char* token;
  token = strtok(line, ",");
  // Parse cell states from the line
  for (size_t i = 0; i < SIZE; i++) {
    for (size_t j = 0; j < SIZE; j++) {
      // Check if token is NULL or longer than 1 character
      if (!token || token[1] != '\0') {
        goto dataset_invalid;
      }
      switch (token[0]) {
        case 'x':
          entry->cells[i][j] = X;
          break;
        case 'o':
          entry->cells[i][j] = O;
          break;
        case 'b':
          entry->cells[i][j] = EMPTY;
          break;
        default:
          fprintf(stderr, "Error: Invalid cell state in dataset: %s\n", token);
          return -1;
      }
      token = strtok(NULL, ",");
    }
  }
  // Parse outcome from the line
  if (!token) goto dataset_invalid;
  if (strncmp(token, "positive", 8) == 0) {
    entry->outcome = POSITIVE;
  } else if (strncmp(token, "negative", 8) == 0) {
    entry->outcome = NEGATIVE;
  } else {
    fprintf(stderr, "Error: Invalid outcome in dataset: %s\n", token);
    return -1;
  }

  return 0;

dataset_invalid:
  fprintf(stderr, "Error: Dataset structure is invalid\n");
  return -1;
}

DataEntry* process_dataset(const char* filepath, size_t* data_entries_size) {
  FILE* dataset = fopen(filepath, "r");
  if (!dataset) {
    fprintf(stderr, "Error: Failed to open dataset\n");
    return NULL;
  }

  // Count the number of entries
  size_t count = 0;
  char line[MAX_DATASET_LINE_LENGTH];
  while (fgets(line, sizeof(line), dataset)) {
    count++;
  }

  // Allocate memory for data entries
  DataEntry* data_entries = (DataEntry*)malloc(count * sizeof(DataEntry));
  if (!data_entries) {
    fprintf(stderr, "Error: Memory allocation failed for data entries\n");
    fclose(dataset);
    return NULL;
  }

  // Reset the file pointer to the beginning
  rewind(dataset);

  // Read the dataset entries
  for (size_t i = 0; i < count; i++) {
    if (fgets(line, sizeof(line), dataset)) {
      if (process_line(line, &data_entries[i]) != 0) {
        free(data_entries);
        fclose(dataset);
        return NULL;
      }
    } else {
      fprintf(stderr, "Error: Failed to read dataset line\n");
      free(data_entries);
      fclose(dataset);
      return NULL;
    }
  }

  fclose(dataset);
  *data_entries_size = count;
  return data_entries;
}

int shuffle_dataset(DataEntry* data_entries, size_t size) {
  if (size < 2) return -1;
  // Fisher-Yates shuffle
  for (size_t i = 0; i < size - 1; i++) {
    size_t j = i + rand() % (size - i);
    DataEntry temp = data_entries[j];
    data_entries[j] = data_entries[i];
    data_entries[i] = temp;
  }

  return 0;
}
