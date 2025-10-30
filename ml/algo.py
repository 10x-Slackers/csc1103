"""
Naive Bayes classifier for Tic-Tac-Toe game outcome prediction.

Author: Kitsuneez, Commit2main
Date: 2025-10-26
Version: 2
"""

import json
import math
import random
from typing import TypedDict

from logic.board import find_empty_cells, print_board
from ml.dataset import DataEntry, parse

MODEL_PATH = "ml/naive_bayes_model.json"

BLANK_BOARD: list[list[str]] = [
    ["", "", ""],
    ["", "", ""],
    ["", "", ""],
]

TEST_BOARD: list[list[str]] = [
    ["X", "", "X"],
    ["X", "O", ""],
    ["O", "", ""],
]

MAPPING = {(0, 0): "TN", (1, 1): "TP", (1, 0): "FN", (0, 1): "FP"}

AI_PLAYER = "O"
HUMAN_PLAYER = "X"

# Mapping from human-readable symbols to integers used by the model.
VECTORS = {"": 0, "X": 1, "O": 2, "positive": 1, "negative": 0}

# Laplace smoothing parameter. A small positive value (1.0) prevents
# zero probabilities when a feature value hasn't been observed in the
# training data for a given class.
ALPHA = 1.0

# Model shape constants
STATE = 3  # Each cell can be empty, X, or O
FEATURES = 9  # Cells on a 3x3 board
CLASSES = 2  # Negative (0) and positive (1)
EPISODES = 10  # Number of games to simulate in the demo

# Types to make function signatures easier to read
BoardVector = list[int]
OutcomeVector = int
Prior = list[float]
Likelihood = list[list[list[float]]]


class Model(TypedDict):
    """TypedDict for a saved model.

    Fields:
        prior: list of prior probabilities for each class (length CLASSES)
        likelihood: 3D list with shape [class][feature][value]
    """

    prior: Prior
    likelihood: Likelihood


def main() -> None:
    """
    Train a model from the dataset and run a quick demo.
    """
    # Load and vectorize the dataset
    dataset = parse("dataset/tic-tac-toe.data")
    random.shuffle(dataset)
    # Split dataset into 80:20 for training:testing after shuffling
    training_len, testing_len = int(len(dataset) * 0.8), int(len(dataset) * 0.2)
    training_data = dataset[:training_len]
    testing_data = dataset[training_len:]
    x: list[BoardVector] = []
    y: list[OutcomeVector] = []
    for entry in training_data:
        board_vector, outcome_vector = vectorize(entry)
        x.append(board_vector)
        y.append(outcome_vector)
    # Train and persist the model
    prior, likelihood = train(x, y)
    save_model(prior, likelihood, MODEL_PATH)
    model = load_model(MODEL_PATH)
    # simulate test
    result = test_model(testing_data, model)
    summary = confusion_calculation(result, testing_len)
    print_matrix(summary)
    # Demo of model making move on a test board
    print("\nTest board:")
    print_board(TEST_BOARD)
    move = ai_move(TEST_BOARD, model)
    print(f"AI selected move: {move}")
    TEST_BOARD[move[0]][move[1]] = AI_PLAYER
    print_board(TEST_BOARD)


def print_matrix(summary: dict[str, int | float]) -> None:
    """
    Print confusion matrix and related metrics

    Args:
        summary: dictionary of confusion matrix counts and metrics
    Returns:
        None
    """
    print("\n=== Confusion Matrix Counts ===")
    print(f"TP: {summary['TP']:<5} FP: {summary['FP']:<5}")
    print(f"FN: {summary['FN']:<5} TN: {summary['TN']:<5}")
    print(f"Total: {summary['Total']}\n")

    print("=== Metrics Summary ===")
    print(f"{'Metric':<30} {'Value':>10}")
    print("-" * 41)
    for k, v in summary.items():
        if k in ["TP", "FP", "FN", "TN", "Total"]:  # skip raw counts in this section
            continue
        print(f"{k:<30} {v:>10.4f}")


def test_model(testing_data: list[DataEntry], model: Model) -> list[str]:
    """
    Test the trained model on the testing dataset and print f-score

    Args:
        testing_data: List of dataset entries for testing
        model: Trained Naive Bayes model
    Returns:
        None
    """
    outcomes: list[OutcomeVector] = []
    predict_outcomes: list[OutcomeVector] = []
    for test_data in testing_data:
        _, outcome = vectorize(test_data)
        outcomes.append(outcome)
        # predict_res is always [negative, positive]
        predict_res = probability(test_data["board"], model)
        predict_outcomes.append(predict_res.index(max(predict_res)))
    result: list[str] = [
        MAPPING[(o, p)] for o, p in zip(outcomes, predict_outcomes, strict=True)
    ]
    return result


def confusion_calculation(result: list[str], count: int) -> dict[str, int | float]:
    """
    calculate and print confusion matrix and related metrics

    Args:
        result: list of results from model predictions
    Returns:
        None
    """
    summary: dict[str, int | float] = {"TN": 0, "TP": 0, "FN": 0, "FP": 0}
    for r in result:
        summary[r] += 1

    tn, tp, fn, fp = summary["TN"], summary["TP"], summary["FN"], summary["FP"]
    p = tp + fn
    n = tn + fp
    # true positive rate and true negative rate
    tpr = tp / p
    tnr = tn / n
    summary.update(
        {
            "Prevalence": p / count,
            "Accuracy": (tp + tn) / count,
            "Balanced Accuracy": (tpr + tnr) / 2,
            "Positive predictive value": tp / (tp + fp),
            "Negative Predictive Value": tn / (tn + fn),
            "True Positive Rate": tpr,
            "True Negative Rate": tnr,
            "F1-score": 2 * tp / (2 * tp + fp + fn),
            "False Positive Rate": 1 - tnr,
            "False Negative Rate": 1 - tpr,
            "Total": count,
        }
    )
    return summary


def vectorize(entry: DataEntry) -> tuple[BoardVector, OutcomeVector]:
    """
    Convert a dataset entry into a numerical vector representation.

    Args:
        entry: A dictionary with keys "board" and "outcome"
    Returns:
        A list of integers representing the vectorized form of the entry
    """
    board_vector: BoardVector = []
    for row in entry["board"]:
        for cell in row:
            board_vector.append(VECTORS[cell])
    outcome_vector = VECTORS[entry["outcome"].lower()]
    return board_vector, outcome_vector


def train(x: list[BoardVector], y: list[OutcomeVector]) -> tuple[Prior, Likelihood]:
    """
    Train the Naive Bayes classifier by updating counts based on the provided data.

    Args:
        x: List of integers representing the board state
        y: Integer representing the outcome (0 or 1)
    """
    # Initialize counts
    class_counts = [0] * CLASSES
    # count of each occurence for each possible STATE (0,1,2)
    feature_counts = [[[0] * STATE for _ in range(FEATURES)] for _ in range(CLASSES)]
    prior: Prior = [0.0] * CLASSES
    # 2 different outcomes with 9 boxes with each box having 3 possible STATE (0, 1, 2)
    likelihood: Likelihood = [
        [[0.0] * STATE for _ in range(FEATURES)] for _ in range(CLASSES)
    ]
    total_samples = len(x)

    # Count occurrences of every class and feature value
    for board_vector, outcome in zip(x, y, strict=True):
        # keep track for number of outcome
        class_counts[outcome] += 1
        for i, cell_value in enumerate(board_vector):
            # increment outcome for box i with ""(0), X(1) or O(2)
            feature_counts[outcome][i][cell_value] += 1

    # Compute prior probabilities (base probabilities of each class)
    for cl in range(CLASSES):
        # Formula used: (number of outcome + ALPHA) /
        # (total samples + ALPHA * 2[possible outcomes])
        prior[cl] = (class_counts[cl] + ALPHA) / (total_samples + ALPHA * CLASSES)

    # Compute likelihood probabilities (probability of feature given class)
    for cl in range(CLASSES):
        # loops through all boxes
        for feat in range(FEATURES):
            # total count for all possible state for each box given outcome
            # ALPHA * state is so that when a feature value hasn't been observed in the
            # training data for a given class, so it doesn't lead to zero probability
            total_count = sum(feature_counts[cl][feat]) + ALPHA * STATE
            # loops through all possible state for each box
            for val in range(STATE):
                # possibility for each box to be in each state given outcome
                likelihood[cl][feat][val] = (
                    feature_counts[cl][feat][val] + ALPHA
                ) / total_count

    return prior, likelihood


def save_model(prior: Prior, likelihood: Likelihood, filepath: str) -> None:
    """
    Save the trained Naive Bayes model to a file.

    Args:
        prior: Prior probabilities for each class
        likelihood: Likelihood probabilities for each feature given class
        filepath: Path to save the model
    """
    model: Model = {"prior": prior, "likelihood": likelihood}
    with open(filepath, "w", encoding="utf-8") as f:
        json.dump(model, f)


def load_model(filepath: str) -> Model:
    """
    Load a trained Naive Bayes model from a file.

    Args:
        filepath: Path to the model file
    Returns:
        The loaded Naive Bayes model
    """
    with open(filepath, encoding="utf-8") as f:
        model: Model = json.load(f)
    return model


def probability(board: list[list[str]], model: Model) -> list[float]:
    """
    Compute the posterior probability that the given board leads to a positive (1)
    outcome, according to the Naive Bayes model. Uses the log-sum-exp trick to convert
    log-scores into normalized posterior probabilities in a numerically stable way
    (preventing underflow).

    Args:
        board: 3x3 Tic-Tac-Toe board (list of lists of str)
        model: Trained Naive Bayes model
    Returns:
        A float in [0, 1] representing P(class=positive | board)
    """
    # Vectorize board
    board_vector: BoardVector = []
    for row in board:
        for cell in row:
            board_vector.append(VECTORS[cell])

    # Compute log scores for each class
    # log P(class) + sum log P(feature|class)
    log_scores = [0.0] * CLASSES
    for cl in range(CLASSES):
        # Start with log prior
        s = math.log(model["prior"][cl])
        for i, cell_value in enumerate(board_vector):
            # Add log likelihoods
            s += math.log(model["likelihood"][cl][i][cell_value])
        log_scores[cl] = s

    # Normalize to get log posterior probabilities (log-sum-exp)
    m = max(log_scores)
    exp_scores = [math.exp(lp - m) for lp in log_scores]
    lse = m + math.log(sum(exp_scores))
    log_post = [s - lse for s in log_scores]
    post = [math.exp(lp) for lp in log_post]
    return post


def normalize_to_o(board: list[list[str]], ai_player: str) -> list[list[str]]:
    """
    Normalize the board to always have 'O' as the AI player.
    so that AI can make a move as 'X' always go first in the model training.

    Args:
        board: 3x3 Tic-Tac-Toe board
        ai_player: str - The current AI player symbol ("O")
    Returns:
        Normalized board with 'O' as the AI player
    """
    if ai_player == "X":
        return board

    normalized_board: list[list[str]] = []
    for row in board:
        new_row: list[str] = []
        for cell in row:
            if cell == "X":
                new_row.append("O")
            elif cell == "O":
                new_row.append("X")
            else:
                new_row.append(cell)
        normalized_board.append(new_row)
    return normalized_board


def ai_move(board: list[list[str]], model: Model) -> tuple[int, int]:
    """
    Selects a move for the AI player using the Naive Bayes model to predict outcomes.

    Args:
        board: 3x3 Tic-Tac-Toe board (list of lists of str)
        model: Trained Naive Bayes model
    Returns:
        tuple[int, int]: coordinates of the best move
    """
    empty_cells = find_empty_cells(board)
    if not empty_cells:
        raise ValueError("No empty cells available for a move.")
    # Initialize a negative best probability to ensure any valid move will be better
    best_move = (-1, -1)
    best_prob = -1.0

    # Normalize board to have 'O' as the AI player for consistent predictions
    normalized = normalize_to_o(board, AI_PLAYER)

    for i, j in empty_cells:
        # Simulate the move
        test_board = [row.copy() for row in normalized]
        test_board[i][j] = "O"  # AI is 'O' after normalization

        # Predict outcome probability
        p_positive = probability(test_board, model)[1]

        # Choose the move with the highest predicted probability of winning
        if p_positive > best_prob:
            best_prob = p_positive
            best_move = (i, j)

    return best_move


if __name__ == "__main__":
    main()
