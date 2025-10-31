#include <stdio.h>
#include <stdlib.h>

#include "dataset.h"

static void shuffle(DataEntry* data_entries, int size);
static void vectorize(DataEntry data_entry, int board_vector[3][3]);
static int get_vector_value(const char* symbol);
static void handle_allocation_error(DataEntry* data_entries, int** x,
                                    int current_i, int current_j);

/**
 * @brief: Main entry point of the program.
 *
 * @return: int: 0 on success
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
  int*** x = malloc(training_len * sizeof(int**));
  if (!x) {
    printf("Allocation Error\n");
    free(data_entries);
    return 1;
  }
  // int y;
  for (int i = 0; i < training_len; i++) {
    int board_vector[3][3];
    x[i] = malloc(3 * 3 * sizeof(int*));
    if (!x[i]) {
      handle_allocation_error(data_entries, x, i, 0);
      return 1;
    }
    for (int j = 0; j < 3; j++) {
      x[i][j] = malloc(3 * sizeof(int));
      if (!x[i][j]) {
        handle_allocation_error(data_entries, x, i, j);
        return 1;
      }
    }
    vectorize(data_entries[i], board_vector);
    for (int row = 0; row < 3; row++) {
      for (int cell = 0; cell < 3; cell++) {
        x[i][row][cell] = board_vector[row][cell];
      }
    }
  }
  return 0;
}

void handle_allocation_error(DataEntry* data_entries, int** x, int current_i,
                             int current_j) {
  printf("Allocation Error\n");
  for (int r = 0; r <= current_i; r++) {
    for (int c = 0; c < (r == current_i ? current_j : 3); c++) {
      free(x[r][c]);
    }
    free(x[r]);
  }
  free(x);
  free(data_entries);
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

void vectorize(DataEntry data_entry, int board_vector[3][3]) {
  for (int row = 0; row < 3; row++) {
    for (int cell = 0; cell < 3; cell++) {
      const char* symbol = NULL;
      if (data_entry.board && data_entry.board[row])
        symbol = data_entry.board[row][cell];
      if (!symbol) symbol = "";
      board_vector[row][cell] = get_vector_value(symbol);
    }
  }
}

int get_vector_value(const char* symbol) {
  if (strcmp(symbol, "positive\n") == 0) return 1;
  if (strcmp(symbol, "negative\n") == 0) return 0;
  if (strcmp(symbol, "x") == 0) return 1;
  if (strcmp(symbol, "o") == 0) return 2;
  if (strcmp(symbol, "") == 0) return 0;
  return -1;
}
