"""
Tic-Tac-Toe game for two players.

Author: kaiwenteoo
Date: 2025-10-15
Version: 1
"""


def main():
    """
    Main entry point for the Tic-Tac-Toe game.
    """
    play_game()


def play_game():
    """
    Run a command-line Tic-Tac-Toe game for two players.
    """
    board = [["" for _ in range(3)] for _ in range(3)]
    current_player = "X"

    while True:
        print_board(board)
        row, col = get_valid_move(current_player, board)
        board[row][col] = current_player

        winner = check_winner(board)
        if winner is not None:
            print_board(board)
            if winner == 0:
                print("It's a draw!")
            elif winner == 1:
                print("Player X wins!")
            else:  # winner == 2
                print("Player O wins!")
            break

        current_player = "O" if current_player == "X" else "X"


def print_board(board: list[list[str]]) -> None:
    """
    Print the board showing both current cell values and cell numbers.

    Empty cells show their number (1–9), filled cells show 'X' or 'O'.

    Args:
        board: 3x3 Tic-Tac-Toe board (list of lists of str)
    """
    for i in range(3):
        row_display = []
        for j in range(3):
            cell_value = board[i][j]
            cell_number = i * 3 + j + 1
            row_display.append(cell_value if cell_value != "" else str(cell_number))
        print(" | ".join(row_display))
        if i < 2:
            print("-" * 9)


def get_valid_move(current_player: str, board: list[list[str]]) -> tuple[int, int]:
    """
    Prompt the current player to enter a valid move.

    Args:
        current_player: "X" or "O"
        board: Current state of the game board

    Returns:
        Tuple of (row, col) representing the valid move
    """
    while True:
        try:
            move = int(input(f"Player {current_player}, choose your box (1-9): "))
            if move < 1 or move > 9:
                print("Invalid input. Choose a number between 1 and 9.")
                continue
        except ValueError:
            print("Please enter a valid number.")
            continue

        row = (move - 1) // 3
        col = (move - 1) % 3

        if board[row][col] != "":
            print("That box is already taken. Choose another.")
            continue

        return row, col


def check_winner(board: list[list[str]]) -> int | None:
    """
    Check the Tic-Tac-Toe board for a winner or a draw.

    Args:
        board: 3x3 Tic-Tac-Toe board (list of lists of str)

    Returns:
        1 if player X wins,
        2 if player O wins,
        0 if it's a draw,
        None if the game is ongoing
    """
    # Rows and Columns
    for i in range(3):
        if board[i][0] == board[i][1] == board[i][2] != "":
            return 1 if board[i][0] == "X" else 2
        if board[0][i] == board[1][i] == board[2][i] != "":
            return 1 if board[0][i] == "X" else 2

    # Diagonals
    if board[0][0] == board[1][1] == board[2][2] != "":
        return 1 if board[0][0] == "X" else 2
    if board[0][2] == board[1][1] == board[2][0] != "":
        return 1 if board[0][2] == "X" else 2

    # Draw: no empty cells left
    if all(board[i][j] != "" for i in range(3) for j in range(3)):
        return 0

    return None


if __name__ == "__main__":
    main()
