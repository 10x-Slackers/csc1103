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
  for (int i = 0; i < MAX_MOVES; i++) {
    result->moves_left_result[i].total_time = 0.0;
    result->moves_left_result[i].avg_time = 0.0;
    result->moves_left_result[i].min_time = DBL_MAX;
    result->moves_left_result[i].max_time = 0.0;
  }

  int total_moves[MAX_MOVES] = {0};
  int moves_left;

  printf("Benchmarking %s\n", algorithm_name);
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
      // Validate move
      if (!make_move(&board, &move)) {
        fprintf(stderr, "Warning: Invalid move returned by %s\n",
                algorithm_name);
        continue;
      }
      // Calculate elapsed time in milliseconds
      double elapsed_time =
          (double)(end_time - start_time) * 1000.0 / CLOCKS_PER_SEC;

      // Update result statistics
      moves_left = MAX_MOVES - board.move_count;
      total_moves[moves_left]++;
      MovesLeftResult* mlr = &result->moves_left_result[moves_left];
      mlr->total_time += elapsed_time;
      // Update min/max times
      if (elapsed_time < mlr->min_time) mlr->min_time = elapsed_time;
      if (elapsed_time > mlr->max_time) mlr->max_time = elapsed_time;
    }
  }

  // Calculate averages
  for (int j = 0; j < MAX_MOVES; j++) {
    MovesLeftResult* mlr = &result->moves_left_result[j];
    if (total_moves[j] > 0) {
      mlr->avg_time = mlr->total_time / total_moves[j];
    }
  }
}

static void print_results(const BenchmarkResult results[], int num_results) {
  // Header
  printf("%-20s %-15s %-15s %-15s %-15s\n", "Algorithm", "Moves Left",
         "Avg Time (ms)", "Min Time (ms)", "Max Time (ms)");

  // Per algorithm
  for (int i = 0; i < num_results; i++) {
    const BenchmarkResult* r = &results[i];
    // Per moves left
    for (int j = MAX_MOVES - 1; j >= 0; j--) {
      const MovesLeftResult* mlr = &r->moves_left_result[j];
      if (mlr->total_time > 0) {
        printf("%-20s %-15d %-15.6f %-15.6f %-15.6f\n", r->algorithm_name, j,
               mlr->avg_time, mlr->min_time, mlr->max_time);
      }
    }
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

  printf("Starting benchmarks (%d runs per algorithm)...\n", RUNS);
  BenchmarkResult results[4];
  benchmark(RANDOM, "Random", NULL, &results[0]);
  benchmark(MINIMAX, "Minimax Perfect", NULL, &results[1]);
  benchmark(MINIMAX_HANDICAP, "Minimax Handicapped", NULL, &results[2]);
  benchmark(NAIVE_BAYES, "Naive Bayes", &model, &results[3]);

  printf("\nBenchmark Results:\n", RUNS);
  print_results(results, 4);
  return EXIT_SUCCESS;
}
