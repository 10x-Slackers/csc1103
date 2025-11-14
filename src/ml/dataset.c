#include "dataset.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static DataEntry process_line(char* line) {
  DataEntry entry = {0};

  char* token;
  token = strtok(line, ",");
  // Parse the line into cells and outcome
  for (size_t i = 0; i < SIZE; i++) {
    for (size_t j = 0; j < SIZE; j++) {
      if (token) {
        if (strcmp(token, "x") == 0) {
          entry.cells[i][j] = X;
        } else if (strcmp(token, "o") == 0) {
          entry.cells[i][j] = O;
        } else {
          entry.cells[i][j] = EMPTY;
        }
        token = strtok(NULL, ",");
      }
    }
  }
  if (token) {
    if (strncmp(token, "positive", 8) == 0) {
      entry.outcome = POSITIVE;
    } else {
      entry.outcome = NEGATIVE;
    }
  }
  return entry;
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
      data_entries[i] = process_line(line);
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
  if (size < 2) {
    return -1;
  }
  // Fisher-Yates shuffle
  for (size_t i = 0; i < size - 1; i++) {
    size_t j = i + rand() % (size - i);
    DataEntry temp = data_entries[j];
    data_entries[j] = data_entries[i];
    data_entries[i] = temp;
  }

  return 0;
}
