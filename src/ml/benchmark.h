#ifndef BENCHMARK_H
#define BENCHMARK_H

#define RUNS 1000

typedef enum { RANDOM, MINIMAX, MINIMAX_HANDICAP, NAIVE_BAYES } Algorithm;

typedef struct {
  Algorithm algorithm;
  const char* algorithm_name;
  double total_time;
  double avg_time;
  double min_time;
  double max_time;
} BenchmarkResult;

int run_benchmarks(const char* model_path);

#endif  // BENCHMARK_H
