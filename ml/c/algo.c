/**
 * @author: kitsuneez
 * @date: 2025-11-04
 * @description: Implements AI move logic for Tic-Tac-Toe using a Naive Bayes
 * model.
 */

#include "algo.h"

/**
 * @brief: Main function to test AI move on a Tic-Tac-Toe board.
 *
 * @return: int: exit status
 */
int main() {
  NaiveBayesModel model =
      (NaiveBayesModel){.prior = {0.0}, .likelihood = {{{0.0}}}};
  int res = load_model(&model, MODEL_FILE);
  if (res != 0) {
    printf("Error loading model");
    return 1;
  }
  print_board(temp_board, NULL);
  ai_move(temp_board, model);
  print_board(temp_board, NULL);
  return 0;
}

/**
 * @brief: Loads a trained Naive Bayes model from a binary file.
 *
 * @param model: pointer to the NaiveBayesModel struct to be populated
 * @param filename: name of the binary file containing the trained model
 *
 * @return: int: 0 on success, 1 on failure
 */
int load_model(NaiveBayesModel* model, const char* filename) {
  FILE* fp = fopen(filename, "rb");
  if (fp == NULL) {
    printf("Could not open model file\n");
    return 1;
  }
  fread(model, sizeof(NaiveBayesModel), 1, fp);
  fclose(fp);
  return 0;
}

/**
 * @brief: Determines and makes the best move for the AI on the Tic-Tac-Toe
 * board.
 *
 * @param board: 2D array representing the current Tic-Tac-Toe board
 * @param model: trained Naive Bayes model for evaluating board states
 *
 * @return: None
 */
void ai_move(int board[SIZE][SIZE], const NaiveBayesModel model) {
  // Implement AI move logic here
  Cell empty_cells[SIZE * SIZE];
  int empty_count = find_empty_cells(board, empty_cells, SIZE * SIZE);
  Cell best_move = {.col = -1, .row = -1};
  float best_score = -1.0;
  for (int i = 0; i < empty_count; i++) {
    int board_vector[9];
    Cell cell = empty_cells[i];
    // Simulate move
    board[cell.row][cell.col] = 2;  // Assume AI is player 2
    // Evaluate board
    score_struct p_score = probability(board, &model);
    float p_positive = p_score.positive;
    if (p_positive > best_score) {
      best_score = p_positive;
      best_move = cell;
    }
    // Undo move
    board[cell.row][cell.col] = 0;
  }
  board[best_move.row][best_move.col] = 2;  // Make the best move
}

/**
 * @brief: Calculates the probability scores for each outcome given a board
 * vector and a trained Naive Bayes model.
 *
 * @param board         2D array representing the Tic-Tac-Toe board
 * @param model         Pointer to the trained NaiveBayesModel
 * @return score_struct Containing the probability scores for negative and
 *                      positive outcomes
 */
score_struct probability(const int board[SIZE][SIZE],
                         const NaiveBayesModel* model) {
  float log_scores[OUTCOMES] = {0.0};
  for (int outcome = 0; outcome < OUTCOMES; outcome++) {
    float s = log(model->prior[outcome]);
    for (int row = 0; row < SIZE; row++) {
      for (int col = 0; col < SIZE; col++) {
        int cell_value = board[row][col];
        s += log(model->likelihood[outcome][row][col][cell_value]);
      }
    }
    log_scores[outcome] = s;
  }

  float max_score =
      log_scores[0] > log_scores[1] ? log_scores[0] : log_scores[1];
  float total_log_score = 0;
  for (int outcome = 0; outcome < OUTCOMES; outcome++) {
    total_log_score += exp(log_scores[outcome] - max_score);
  }
  float lse = max_score + log(total_log_score);
  for (int outcome = 0; outcome < OUTCOMES; outcome++) {
    log_scores[outcome] = exp(log_scores[outcome] - lse);
  }
  score_struct score = {.negative = log_scores[0], .positive = log_scores[1]};
  return score;
}
