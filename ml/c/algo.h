#ifndef ALGO_H  // if ALGO_H is not defined
#define ALGO_H  // define ALGO_H

#define OUTCOMES 2
#define CELLS 9
#define STATE 3
#define MODEL_FILE "ml/naive_bayes.bin"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../logic/board.h"
typedef struct {
  float negative;
  float positive;
} score_struct;

typedef struct {
  float prior[OUTCOMES];
  float likelihood[OUTCOMES][CELLS][STATE];
} NaiveBayesModel;

static int load_model(NaiveBayesModel* model, const char* filename);
static void ai_move(int board[SIZE][SIZE], const NaiveBayesModel model);
static void flatten_board(int board[SIZE][SIZE], int* board_vector);
score_struct probability(const int board_vector[9],
                         const NaiveBayesModel* model);

int temp_board[SIZE][SIZE] = {
    {1, 0, 1},
    {1, 2, 0},
    {2, 0, 0},
};
#endif  // End of condition
