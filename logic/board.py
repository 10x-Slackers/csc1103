def print_board(board: list[list[str]]) -> None:
    """
    Print the Tic-Tac-Toe board in a readable format.

    Args:
        board: NxN Tic-Tac-Toe board (list of lists of str)
    """
    for row in board:
        print("|".join(cell if cell != "" else "_" for cell in row))


def find_empty_cells(board: list[list[str]]) -> list[tuple[int, int]]:
    """
    Find all empty cells in the Tic-Tac-Toe board.

    Args:
        board: NxN Tic-Tac-Toe board (list of lists of str)
    Returns:
        list of tuples representing the coordinates of empty cells
    """
    empty_cells: list[tuple[int, int]] = []

    # Look through the board and find empty cells
    for i in range(len(board)):
        for j in range(len(board[i])):
            if board[i][j] == "" or board[i][j] == " ":
                empty_cells.append((i, j))

    return empty_cells


def check_winner(board: list[list[str]]) -> str | None:
    """
    Check the Tic-Tac-Toe board for a winner or a draw.

    Args:
        board: 3x3 Tic-Tac-Toe board (list of lists of str)
    Returns:
        "O" if AI_PLAYER wins, "X" if HUMAN_PLAYER wins, "Draw" if it's a draw,
        None if the game is ongoing
    """
    # Check rows and columns for a win
    for i in range(len(board)):
        # Check rows
        if board[i][0] == board[i][1] == board[i][2] != "":
            return board[i][0]
        # Check columns
        if board[0][i] == board[1][i] == board[2][i] != "":
            return board[0][i]

    # Check diagonals for a win
    if board[0][0] == board[1][1] == board[2][2] != "":
        return board[0][0]
    if board[0][2] == board[1][1] == board[2][0] != "":
        return board[0][2]

    # Check for draw
    if find_empty_cells(board) == []:
        return "Draw"

    return None
