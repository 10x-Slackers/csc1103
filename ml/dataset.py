from typing import TypedDict

from logic.board import print_board

# Dataset file must be placed in the root of the project in the "dataset" folder
DATASET_PATH = "dataset/tic-tac-toe.data"


class DataEntry(TypedDict):
    """
    Represents a single entry in the Tic Tac Toe dataset.

    Attributes:
        board: 3x3 Tic-Tac-Toe board (list of lists of str)
        outcome: Result of the game ("Positive" or "Negative")
    """

    board: list[list[str]]
    outcome: str


def main() -> None:
    """
    Main function to demonstrate dataset parsing. Prints the first 5 entries.
    """
    for entry in parse(DATASET_PATH)[:5]:
        print_board(entry["board"])
        print("Outcome:", entry["outcome"])
        print()


def read(file_path: str) -> list[list[str]]:
    """
    Read the provided dataset file and return its contents as a list of rows.

    Args:
        file_path: Path to the dataset file
    Returns:
        List of rows, where each row is a list of strings
    """
    with open(file_path, encoding="utf-8") as file:
        return [row.strip().split(",") for row in file]


def convert_ttt_matrix(ttt: list[str], formatted: bool = False) -> list[list[str]]:
    """
    Convert a flat list into a 3x3 matrix, optionally formatting the cells to fit
    our board representation.

    Args:
        ttt: Flat list of 9 elements representing the board
        formatted: Whether to format the cells (e.g., uppercase "X" and "O")
    Returns:
        3x3 matrix (list of lists)
    """
    if formatted:
        ttt = [cell.upper() if cell in ("x", "o") else "" for cell in ttt]
    return [ttt[i : i + 3] for i in range(0, 9, 3)]


def parse(file_path: str) -> list[DataEntry]:
    """
    Read and parse the dataset file and convert it into our Entry format.

    Args:
        file_path: Path to the dataset file
    Returns:
        List of Entry objects containing the board and outcome
    """
    dataset: list[DataEntry] = []

    for row in read(file_path):
        ttt = row[:-1]
        outcome = row[-1]
        board = convert_ttt_matrix(ttt, formatted=True)
        entry: DataEntry = {"board": board, "outcome": outcome}
        dataset.append(entry)

    return dataset


if __name__ == "__main__":
    main()
