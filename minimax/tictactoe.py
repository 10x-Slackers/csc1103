import random
from minimax.algo import MIN_VALUE, MAX_VALUE
from logic.board import check_winner, find_empty_cells, print_board

AI_PLAYER = "O"
HUMAN_PLAYER = "X"

TEST_BOARD: list[list[str]] = [
    ["X", "", "X"],
    ["X", "O", ""],
    ["O", "", ""],
]
TOTAL_CELLS = sum(len(row) for row in TEST_BOARD)


def minimax_ttt(
    board: list[list[str]],
    depth: int,
    is_max: bool = True,
    alpha: int = MIN_VALUE,
    beta: int = MAX_VALUE,
) -> int:
    """
    Minimax evaluation for Tic-Tac-Toe with alpha-beta pruning.

    Higher scores are better for the AI player. The algorithm will prefer wins
    with fewer moves and losses with more moves.

    Args:
        board: NxN board (list of lists of str)
        depth: current search depth
        is_max: True when it's the AI player's turn
        alpha: best found option for the AI player
        beta:  best found option for the human player

    Returns:
        int: minimax score for the current board state
    """
    winner = check_winner(board)
    # Depth is used to prefer faster wins and slower losses
    if winner == AI_PLAYER:
        return TOTAL_CELLS - depth
    if winner == HUMAN_PLAYER:
        return -TOTAL_CELLS + depth
    if winner == "Draw":
        return 0

    # Find all possible moves (empty cells) on the board
    moves = find_empty_cells(board)

    if is_max:
        highest_score = MIN_VALUE
        # Try all possible moves and choose the one with the highest score
        for i, j in moves:
            # Make a move and call minimax recursively, then undo the move
            board[i][j] = AI_PLAYER
            score = minimax_ttt(board, depth + 1, False, alpha, beta)
            board[i][j] = ""
            # Update to the highest score found so far
            if score > highest_score:
                highest_score = score
            alpha = max(alpha, highest_score)
            # Prune if current value is already better than beta
            if beta <= alpha:
                break
        return highest_score
    else:
        lowest_score = MAX_VALUE
        # Try all possible moves and choose the one with the lowest score
        for i, j in moves:
            # Make a move and call minimax recursively, then undo the move
            board[i][j] = HUMAN_PLAYER
            score = minimax_ttt(board, depth + 1, True, alpha, beta)
            board[i][j] = ""
            # Update to the lowest score found so far
            if score < lowest_score:
                lowest_score = score
            beta = min(beta, lowest_score)
            # Prune if current value is already better than alpha
            if beta <= alpha:
                break
        return lowest_score


def random_move(board: list[list[str]]) -> tuple[int, int]:
    """
    Select a random empty cell on the Tic-Tac-Toe board.

    Args:
        board: NxN Tic-Tac-Toe board (list of lists of str)
    Returns:
        tuple[int, int]: coordinates of the selected empty cell
    """
    empty_cells = find_empty_cells(board)
    if not empty_cells:
        raise ValueError("No empty cells available for a move.")

    return random.choice(empty_cells)


def best_move(board: list[list[str]]) -> tuple[int, int]:
    """
    Select the best move for the AI player using the minimax algorithm.

    Args:
        board: NxN Tic-Tac-Toe board (list of lists of str)
    Returns:
        tuple[int, int]: coordinates of the best move
    """
    best_score = MIN_VALUE
    best_move = (-1, -1)

    # Find all possible moves (empty cells) on the board
    moves = find_empty_cells(board)
    if not moves:
        raise ValueError("No empty cells available for a move.")

    # Try all possible moves and choose the one with the best score
    for i, j in moves:
        # Use a copy of the board to simulate the move
        sim_board = [row[:] for row in board]
        sim_board[i][j] = AI_PLAYER
        # Get the minimax score for this move
        score = minimax_ttt(sim_board, 0, False)
        # Update to the best score found so far
        if score > best_score:
            best_score = score
            best_move = (i, j)

    return best_move


if __name__ == "__main__":
    print("Test board:")
    print_board(TEST_BOARD)

    print(f"Random move (player {AI_PLAYER}):")
    move = random_move(TEST_BOARD)
    TEST_BOARD[move[0]][move[1]] = AI_PLAYER
    print_board(TEST_BOARD)
    TEST_BOARD[move[0]][move[1]] = ""

    print(f"Best move (player {AI_PLAYER}):")
    move = best_move(TEST_BOARD)
    TEST_BOARD[move[0]][move[1]] = AI_PLAYER
    print_board(TEST_BOARD)
    TEST_BOARD[move[0]][move[1]] = ""
