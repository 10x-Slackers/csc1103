#ifndef ML_H
#define ML_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OUTCOMES 2
#define CELLS 9
#define STATE 3
#define ALPHA 1.0
#define FEATURES 9
#define MODEL_FILE "ml/naive_bayes.bin"

typedef struct {
  float prior[OUTCOMES];
  float likelihood[OUTCOMES][CELLS][STATE];
} NaiveBayesModel;

typedef struct {
  float negative;
  float positive;
} score_struct;

typedef struct {
  char*** board;
  char* outcome;
} DataEntry;

score_struct probability(const int board_vector[9],
                         const NaiveBayesModel* model);

#endif  // ML_H
