#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "../shared/board.h"

#define MAX_MOVES (SIZE * SIZE + 1)
#define RUNS 5000

typedef enum { RANDOM, MINIMAX, MINIMAX_IMPERFECT, NAIVE_BAYES } Algorithm;

typedef struct {
  const char* name;
  Algorithm algorithm;
} BenchmarkAlgorithm;

typedef struct {
  double total_time;
  double avg_time;
  double min_time;
  double max_time;
} MovesLeftResult;

typedef struct {
  Algorithm algorithm;
  const char* algorithm_name;
  float win_rate;
  float draw_rate;
  MovesLeftResult moves_left_result[MAX_MOVES];
} BenchmarkResult;

/**
 * @brief Run benchmarks for different algorithms and print results in a table.
 * @param model_path Path to the Naive Bayes model file.
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on error.
 */
int run_benchmarks(const char* model_path);

#endif  // BENCHMARK_H
