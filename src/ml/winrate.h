#define EPISODES 1000
#include "../shared/board.h"
#include "../shared/minimax.h"
#include "../shared/naive_bayes.h"

typedef struct {
  int wins;
  int losses;
  int draws;
} WinRateStats;

void init_winrate_stats(WinRateStats* stats);
void run_episodes(NaiveBayesModel* model);
void display_result(WinRateStats* stats);
int test_models(const char* model_path);
void update_stats(WinRateStats* stats, Board* board);
