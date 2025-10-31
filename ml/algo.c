#include <stdio.h>
#include <stdlib.h>

#include "dataset.h"

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
