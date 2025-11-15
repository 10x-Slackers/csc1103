#ifndef ML_CLI_H
#define ML_CLI_H

#define DEFAULT_DATASET_PATH "dataset/tic-tac-toe.data"
#define DEFAULT_MODEL_PATH "src/ml/naive_bayes.bin"

typedef enum { MODE_NONE, MODE_TRAIN, MODE_STATS } ProgramMode;

/**
 * @brief Parse command line arguments for the ML program.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @param mode Pointer to store the program mode.
 * @param dataset_path Pointer to store the dataset path.
 * @param model_path Pointer to store the model path.
 * @return 0 on success, -1 on error.
 */
int parse_arguments(int argc, char* argv[], ProgramMode* mode,
                    const char** dataset_path, const char** model_path);

#endif  // ML_CLI_H
