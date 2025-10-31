#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define AI_PLAYER 'O'
#define HUMAN_PLAYER 'X'
#define ALPHA 0.1

#define STATE 3
#define FEATURES 9
#define CLASSES 2

#define BUFFER 100
#define MAX_LINES 1000

// typedef struct
// {
//     int outcome;
//     int prediction;
// } outcome_predict;

// typedef struct
// {
//     outcome_predict key;
//     char *value;
// } MAPPING_STRUCT;

// typedef struct
// {
//     char *key;
//     int value;
// } VECTORS_STRUCT;

// typedef struct
// {
//     float *Prior;
//     float ***Likelihood;
// } TypedDict;

typedef struct {
  char*** board;
  char* outcome;
} DataEntry;

// MAPPING_STRUCT MAPPING[] = {
//     {{0, 0}, "TN"},
//     {{0, 1}, "FP"},
//     {{1, 0}, "FN"},
//     {{1, 1}, "TP"}};

// VECTORS_STRUCT VECTORS[] = {
//     {"", 0},
//     {"X", 1},
//     {"O", 2},
//     {"positive", 1},
//     {"negative", 0}};

static DataEntry* process_dataset(const char* filename);
static char*** convert_ttt_matrix(char** ttt);
static char** str_split(char* a_str, char a_delim);

/**
 * @brief: Main entry point of the program.
 *
 * @return: int: 0 on success
 */
int main() {
  const DataEntry* data_entries = process_dataset("dataset/tic-tac-toe.data");
  if (data_entries == NULL) {
    return 1;
  }
  //   printf("%s\n", data_entries[957].outcome);
  return 0;
}

/**
 * @brief: Parses the dataset file and returns an array of DataEntry structs.
 *
 * @param filename: The name of the file to parse.
 * @return: DataEntry*: Pointer to an array of DataEntry structs.
 */
DataEntry* process_dataset(const char* filename) {
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
  for (int i = 0; i < FEATURES; i += 3) {
    formatted[i] = (char**)malloc(3 * sizeof(char*));
    for (int j = 0; j < 3; j++) {
      formatted[i][j] = row[i + j];
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
