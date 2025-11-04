#ifndef ALGO_H
#define ALGO_H

#include "../../logic/board.h"
#include "ml.h"

static int load_model(NaiveBayesModel* model, const char* filename);
static void ai_move(int board[SIZE][SIZE], const NaiveBayesModel model);
static void flatten_board(int board[SIZE][SIZE], int* board_vector);

int temp_board[SIZE][SIZE] = {
    {1, 0, 1},
    {1, 2, 0},
    {2, 0, 0},
};
#endif // ALGO_H