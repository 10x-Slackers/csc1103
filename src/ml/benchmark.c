#include "benchmark.h"

#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../shared/board.h"
#include "../shared/minimax.h"
#include "../shared/naive_bayes.h"

/* Global Naive Bayes model */
static NaiveBayesModel model;
/* Algorithms to benchmark */
static BenchmarkAlgorithm algorithms[] = {
    {"Random", RANDOM},
    {"Minimax Perfect", MINIMAX},
    {"Minimax Imperfect", MINIMAX_HANDICAP},
    {"Naive Bayes", NAIVE_BAYES},
};

/**
 * @brief Initialise a BenchmarkResult structure.
 * @param algorithm Algorithm to benchmark.
 * @param algorithm_name Name of the algorithm for display.
 * @param result Pointer to BenchmarkResult structure to initialise.
 */
static void init_result(Algorithm algorithm, const char* algorithm_name,
                        BenchmarkResult* result) {
  result->algorithm = algorithm;
  result->algorithm_name = algorithm_name;
  result->win_rate = 0.0;
  result->draw_rate = 0.0;
  for (int i = 0; i < MAX_MOVES; i++) {
    result->moves_left_result[i].total_time = 0.0;
    result->moves_left_result[i].avg_time = 0.0;
    result->moves_left_result[i].min_time = DBL_MAX;
    result->moves_left_result[i].max_time = 0.0;
  }
}

/**
 * @brief Benchmark the win rate of a specific algorithm against a random
 * opponent.
 * @param result Pointer to BenchmarkResult structure to store results.
 */
static void benchmark_win(BenchmarkResult* result) {
  if (!result) return;
  // Initialise start values
  Algorithm algorithm = result->algorithm;
  const char* algorithm_name = result->algorithm_name;
  int wins = 0;
  int draws = 0;

  for (int i = 0; i < RUNS; i++) {
    // Alternate opponent every run and starting player every two runs
    Player random_player = (i % 2 == 0) ? PLAYER_X : PLAYER_O;
    Player starting_player = ((i / 2) % 2 == 0) ? PLAYER_X : PLAYER_O;
    Board board;
    init_board(&board, starting_player);
    Winner winner = ONGOING;
    // Run until game over
    while (winner == ONGOING) {
      Cell move;
      if (board.current_player == random_player) {
        // Random opponent move
        move = random_move(&board);
      } else {
        // AI move based on algorithm
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
            move = nb_find_move(&board, &model);
            break;
        }
      }
      if (!make_move(&board, &move)) {
        fprintf(stderr, "Warning: Invalid move returned by %s\n",
                algorithm_name);
        break;
      }
      winner = check_winner(&board, NULL);
    }

    // Check result and update statistics
    if ((winner == WIN_O && random_player == PLAYER_X) ||
        (winner == WIN_X && random_player == PLAYER_O)) {
      wins++;
    } else if (winner == DRAW) {
      draws++;
    }
  }

  // Calculate win/draw rate percentage
  result->win_rate = (100.0 * wins) / RUNS;
  result->draw_rate = (100.0 * draws) / RUNS;
}

/**
 * @brief Benchmark the response time of a specific algorithm.
 * @param result Pointer to BenchmarkResult structure to store results.
 */
static void benchmark_response(BenchmarkResult* result) {
  if (!result) return;
  // Initialise start values
  Algorithm algorithm = result->algorithm;
  const char* algorithm_name = result->algorithm_name;
  int total_moves[MAX_MOVES] = {0};
  int moves_left;

  // Perform RUNS games
  for (int i = 0; i < RUNS; i++) {
    // Alternate starting player
    Player starting_player = (i % 2 == 0) ? PLAYER_X : PLAYER_O;
    Board board;
    init_board(&board, starting_player);
    // Randomise the first move to vary starting positions
    Cell move = random_move(&board);
    make_move(&board, &move);
    // Run until game over
    while (check_winner(&board, NULL) == ONGOING) {
      moves_left = MAX_MOVES - board.move_count;
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
          move = nb_find_move(&board, &model);
          break;
      }
      // Measure end time (ms)
      clock_t end_time = clock();
      // Validate move
      if (!make_move(&board, &move)) {
        fprintf(stderr, "Warning: Invalid move returned by %s\n",
                algorithm_name);
        break;
      }
      // Calculate elapsed time in milliseconds
      double elapsed_time =
          (double)(end_time - start_time) * 1000.0 / CLOCKS_PER_SEC;
      // Update result statistics
      total_moves[moves_left]++;
      MovesLeftResult* mlr = &result->moves_left_result[moves_left];
      mlr->total_time += elapsed_time;
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

/**
 * @brief Print the benchmark results in table format.
 * @param results Array of BenchmarkResult structures.
 * @param num_results Number of results in the array.
 */
static void print_results(const BenchmarkResult results[], size_t num_results) {
  // Win and Draw Rate
  printf("%-20s %-15s %-15s\n", "Algorithm", "Win Rate (%)", "Draw Rate (%)");
  for (size_t i = 0; i < num_results; i++) {
    const BenchmarkResult* r = &results[i];
    printf("%-20s %-15.2f %-15.2f\n", r->algorithm_name, r->win_rate,
           r->draw_rate);
  }

  // Response Time
  printf("\n%-20s %-15s %-15s %-15s %-15s\n", "Algorithm", "Moves Left",
         "Avg Time (ms)", "Min Time (ms)", "Max Time (ms)");
  for (size_t i = 0; i < num_results; i++) {
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
  if (!model_path) {
    fprintf(stderr, "Error: Model path is not specified\n");
    return EXIT_FAILURE;
  }
  printf("Loading Naive Bayes model from %s...\n", model_path);
  if (load_nb_model(&model, model_path) != 0) {
    fprintf(stderr, "Error: Failed to load model\n");
    return EXIT_FAILURE;
  }
  printf("Model loaded successfully.\n");

  // Initialize results array
  size_t num_algorithms = sizeof(algorithms) / sizeof(algorithms[0]);
  BenchmarkResult results[num_algorithms];
  for (size_t i = 0; i < num_algorithms; i++) {
    init_result(algorithms[i].algorithm, algorithms[i].name, &results[i]);
  }

  printf("Starting benchmarks (%d runs per algorithm)...\n", RUNS);
  printf("Benchmarking Win Rates...\n");
  for (size_t i = 0; i < num_algorithms; i++) benchmark_win(&results[i]);
  printf("Benchmarking Response Times...\n");
  for (size_t i = 0; i < num_algorithms; i++) benchmark_response(&results[i]);

  printf("\nBenchmark Results:\n");
  print_results(results, num_algorithms);
  return EXIT_SUCCESS;
}
