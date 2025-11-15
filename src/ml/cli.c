#include "cli.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Print usage information for the program.
 * @param progname Name of the program.
 */
static void print_usage(const char* progname) {
  printf(
      "Usage:\n"
      "  %s train [-d <dataset path>] [-m <model path>]\n"
      "  %s stats [-d <dataset path>] [-m <model path>]\n",
      progname, progname);
}

int parse_arguments(int argc, char* argv[], ProgramMode* mode,
                    const char** dataset_path, const char** model_path) {
  if (argc < 2) {
    print_usage(argv[0]);
    return -1;
  }

  // Parse the mode
  if (strcmp(argv[1], "train") == 0) {
    *mode = MODE_TRAIN;
  } else if (strcmp(argv[1], "stats") == 0) {
    *mode = MODE_STATS;
  } else {
    fprintf(stderr, "Error: Invalid mode '%s'\n", argv[1]);
    print_usage(argv[0]);
    return -1;
  }

  // Set defaults
  *dataset_path = DEFAULT_DATASET_PATH;
  *model_path = DEFAULT_MODEL_PATH;

  // Reset getopt state for parsing options
  optind = 2;

  // Parse options
  int opt;
  while ((opt = getopt(argc, argv, "d:m:")) != -1) {
    switch (opt) {
      case 'd':
        *dataset_path = optarg;
        break;
      case 'm':
        *model_path = optarg;
        break;
      default:
        print_usage(argv[0]);
        return -1;
    }
  }

  // Check for extra arguments
  if (optind < argc) {
    fprintf(stderr, "Error: Unexpected argument '%s'\n", argv[optind]);
    print_usage(argv[0]);
    return -1;
  }

  return 0;
}
