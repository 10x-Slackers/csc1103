#include "../../logic/board.h"
#include "ml.h"

static int load_model(NaiveBayesModel* model, const char* filename);
static int ai_move(int board[SIZE][SIZE], const NaiveBayesModel model);
static void flatten_board(int board[SIZE][SIZE], int* board_vector);

int temp_board[SIZE][SIZE] = {
    {1, 0, 1},
    {1, 2, 0},
    {2, 0, 0},
};

int main() {
  NaiveBayesModel model =
      (NaiveBayesModel){.prior = {0.0}, .likelihood = {{{0.0}}}};
  int res = load_model(&model, MODEL_FILE);
  if (res != 0) {
    printf("Error loading model");
    return 1;
  }
  print_board(temp_board);
  ai_move(temp_board, model);
  print_board(temp_board);
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
// TBI
int ai_move(int board[SIZE][SIZE], const NaiveBayesModel model) {
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
    flatten_board(board, board_vector);
    // Evaluate board
    score_struct p_score = probability(board_vector, &model);
    float p_positive = p_score.positive;
    if (p_positive > best_score) {
      best_score = p_positive;
      best_move = cell;
    }
    // Undo move
    board[cell.row][cell.col] = 0;
  }
  board[best_move.row][best_move.col] = 2;  // Make the best move
  return 0;
}

/**
 * @brief: Flattens a 2D Tic-Tac-Toe board into a 1D array.
 *
 * @param board: 2D array representing the Tic-Tac-Toe board
 * @param board_vector: pointer to a 1D array to hold the flattened board
 * @return: void
 */
void flatten_board(int board[SIZE][SIZE], int* board_vector) {
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      board_vector[i * SIZE + j] = board[i][j];
    }
  }
}
