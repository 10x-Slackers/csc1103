#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "../shared/board.h"

#define MAX_MOVES (SIZE * SIZE + 1)
#define RUNS 1000

typedef enum { RANDOM, MINIMAX, MINIMAX_HANDICAP, NAIVE_BAYES } Algorithm;

typedef struct {
  int moves_left;
  double total_time;
  double avg_time;
  double min_time;
  double max_time;
} MovesLeftResult;

typedef struct {
  Algorithm algorithm;
  const char* algorithm_name;
  MovesLeftResult moves_left_result[MAX_MOVES];
} BenchmarkResult;

int run_benchmarks(const char* model_path);

#endif  // BENCHMARK_H
