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

from logic.board import check_winner, find_empty_cells
from ml.dataset import DataEntry, parse

MODEL_PATH = "ml/naive_bayes_model.json"

TEST_BOARD: list[list[str]] = [
    ["", "", ""],
    ["", "", ""],
    ["", "", ""],
]
AI_PLAYER = "O"
HUMAN_PLAYER = "X"

# Mapping from human-readable symbols to integers used by the model.
VECTORS = {"": 0, "X": 1, "O": 2, "positive": 1, "negative": 0}

# Laplace smoothing parameter. A small positive value (1.0) prevents
# zero probabilities when a feature value hasn't been observed in the
# training data for a given class.
ALPHA = 1.0

# Model shape constants
VALUES = 3  # Each cell can be empty, X, or O
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
    x: list[BoardVector] = []
    y: list[OutcomeVector] = []

    for entry in dataset:
        board_vector, outcome_vector = vectorize(entry)
        x.append(board_vector)
        y.append(outcome_vector)

    # Train and persist the model
    prior, likelihood = train(x, y)
    save_model(prior, likelihood, MODEL_PATH)
    episodes = int(len(dataset) / 80 * 20)
    simulate_game(episodes)


def simulate_game(episodes: int) -> None:
    """
    Simulate a single game between the AI and a random player.

    Args:
        episodes: Number of episodes to simulate
    """
    # Simulate several games between the AI and player to have a hybrid training
    # 20-80 split, 20% for testing, 80% from dataset provided
    result: dict[str, int] = {"X": 0, "O": 0, "Draw": 0}
    for _ in range(episodes):
        model = load_model(MODEL_PATH)
        current_board = [row.copy() for row in TEST_BOARD]
        player = HUMAN_PLAYER
        while check_winner(current_board) is None:
            if player == HUMAN_PLAYER:
                empty_cells = find_empty_cells(current_board)
                move = random.choice(empty_cells)
            else:
                move = ai_move(current_board, model)
            current_board[move[0]][move[1]] = player
            player = AI_PLAYER if player == HUMAN_PLAYER else HUMAN_PLAYER
        result[check_winner(current_board) or "Draw"] += 1
    print(f"Game result: {result}\n")


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
    feature_counts = [[[0] * VALUES for _ in range(FEATURES)] for _ in range(CLASSES)]
    prior: Prior = [0.0] * CLASSES
    likelihood: Likelihood = [
        [[0.0] * VALUES for _ in range(FEATURES)] for _ in range(CLASSES)
    ]
    total_samples = len(x)

    # Count occurrences of every class and feature value
    for board_vector, outcome in zip(x, y, strict=True):
        class_counts[outcome] += 1
        for i, cell_value in enumerate(board_vector):
            feature_counts[outcome][i][cell_value] += 1

    # Compute prior probabilities (base probabilities of each class)
    for cl in range(CLASSES):
        prior[cl] = (class_counts[cl] + ALPHA) / (total_samples + ALPHA * CLASSES)

    # Compute likelihood probabilities (probability of feature given class)
    for cl in range(CLASSES):
        for feat in range(FEATURES):
            total_count = sum(feature_counts[cl][feat]) + ALPHA * VALUES
            for val in range(VALUES):
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
    with open(filepath, "w") as f:
        json.dump(model, f)


def load_model(filepath: str) -> Model:
    """
    Load a trained Naive Bayes model from a file.

    Args:
        filepath: Path to the model file
    Returns:
        The loaded Naive Bayes model
    """
    with open(filepath) as f:
        model: Model = json.load(f)
    return model


def probability(board: list[list[str]], model: Model) -> float:
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
    return math.exp(log_post[1])


def predict(board: list[list[str]], model: Model) -> int:
    """
    Predict the outcome class for a board.
    """
    p_positive = probability(board, model)
    return 1 if p_positive >= 0.5 else 0


def normalize_to_x(board: list[list[str]], ai_player: str) -> list[list[str]]:
    """
    Normalize the board to always have 'X' as the AI player.
    If 'O' has more moves, swap 'X' and 'O'.

    Args:
        board: 3x3 Tic-Tac-Toe board
        ai_player: str - The current AI player symbol ("X" or "O")
    Returns:
        Normalized board with 'X' as the AI player
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

    best_move = (-1, -1)
    best_prob = -1.0

    # Normalize board to have 'X' as the AI player for consistent predictions
    normalized = normalize_to_x(board, AI_PLAYER)

    for i, j in empty_cells:
        # Simulate the move
        test_board = [row.copy() for row in normalized]
        test_board[i][j] = "X"  # AI is 'X' after normalization

        # Predict outcome probability
        p_positive = probability(test_board, model)

        # Choose the move with the highest predicted probability of winning
        if p_positive > best_prob:
            best_prob = p_positive
            best_move = (i, j)

    return best_move


if __name__ == "__main__":
    main()
