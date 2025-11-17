#include "benchmark.h"

#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../shared/board.h"
#include "../shared/minimax.h"
#include "../shared/naive_bayes.h"

static void benchmark(Algorithm algorithm, const char* algorithm_name,
                      const NaiveBayesModel* model, BenchmarkResult* result) {
  // Initialize result values
  result->algorithm = algorithm;
  result->algorithm_name = algorithm_name;
  result->total_time = 0.0;
  result->min_time = DBL_MAX;
  result->max_time = 0.0;

  printf("Benchmarking %s\n", algorithm_name);
  int total_moves = 0;

  for (int i = 0; i < RUNS; i++) {
    Board board;
    init_board(&board, PLAYER_X);
    // Randomise the first move
    Cell move = random_move(&board);
    make_move(&board, &move);
    // Run until game over
    while (check_winner(&board, NULL) == ONGOING) {
      // Measure start time (ms)
      clock_t start_time = clock();
      // Get AI move based on algorithm
      switch (algorithm) {
        case RANDOM:
          move = random_move(&board);
          break;
        case MINIMAX:
          move = minimax_find_move(&board, false);
          break;
        case MINIMAX_HANDICAP:
          move = minimax_find_move(&board, true);
          break;
        case NAIVE_BAYES:
          move = nb_find_move(&board, model);
          break;
      }
      // Measure end time (ms)
      clock_t end_time = clock();
      // Calculate elapsed time in milliseconds
      double elapsed_time =
          (double)(end_time - start_time) * 1000.0 / CLOCKS_PER_SEC;

      if (move.col == -1 || move.row == -1) {
        fprintf(stderr, "Warning: Invalid move returned by %s\n",
                algorithm_name);
        continue;
      }
      make_move(&board, &move);

      // Update result statistics
      total_moves++;
      result->total_time += elapsed_time;
      if (elapsed_time < result->min_time && elapsed_time > 0)
        result->min_time = elapsed_time;
      if (elapsed_time > result->max_time) result->max_time = elapsed_time;
    }
  }
  // Calculate average time
  result->avg_time = result->total_time / total_moves;
}

static void print_results(const BenchmarkResult results[], int num_results) {
  // Print header
  printf("%-20s %-15s %-15s %-15s %-15s\n", "Algorithm", "Avg Time (ms)",
         "Min Time (ms)", "Max Time (ms)", "Total Time (ms)");
  // Print results
  for (int i = 0; i < num_results; i++) {
    const BenchmarkResult* r = &results[i];
    printf("%-20s %-15.6f %-15.6f %-15.6f %-15.6f\n", r->algorithm_name,
           r->avg_time, r->min_time, r->max_time, r->total_time);
  }
}

int run_benchmarks(const char* model_path) {
  printf("Loading Naive Bayes model from %s...\n", model_path);
  NaiveBayesModel model;
  if (load_nb_model(&model, model_path) != 0) {
    fprintf(stderr, "Error: Failed to load model\n");
    return EXIT_FAILURE;
  }
  printf("Model loaded successfully.\n");

  printf("Starting benchmarks...\n");
  BenchmarkResult results[4];
  benchmark(RANDOM, "Random", NULL, &results[0]);
  benchmark(MINIMAX, "Minimax Perfect", NULL, &results[1]);
  benchmark(MINIMAX_HANDICAP, "Minimax Handicapped", NULL, &results[2]);
  benchmark(NAIVE_BAYES, "Naive Bayes", &model, &results[3]);

  printf("\nBenchmark Results (%d runs):\n", RUNS);
  print_results(results, 4);
  return EXIT_SUCCESS;
}
