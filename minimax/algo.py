from dataclasses import dataclass
from typing import Optional

MIN_VALUE = -65536
MAX_VALUE = 65536


@dataclass
class Tree:
    """
    Simple binary tree implementation.

    Attributes:
        left (Optional[Tree]): The left child node (for parent nodes).
        right (Optional[Tree]): The right child node (for parent nodes).
        data (Optional[int]): The value of the node (for leaf nodes).
    """

    left: Optional["Tree"] = None
    right: Optional["Tree"] = None
    data: Optional[int] = None


def minimax(node: Tree, is_max: bool = True) -> int:
    """
    Minimax algorithm without alpha-beta pruning.

    Args:
        node (Tree): The current node in the game tree.
        is_max (bool): True if the current level is the maximising player, False if minimizing
    Returns:
        int: The minimax value of the node.
    """
    # Return the value if it's a leaf node
    if node.data is not None:
        return node.data

    # Ensure the node has both children
    if node.left is None or node.right is None:
        raise ValueError("node must have both left and right children")

    # Run minimax recursively while alternating between maximizing and minimizing
    left_val = minimax(node.left, not is_max)
    right_val = minimax(node.right, not is_max)

    if is_max:
        return max(left_val, right_val)
    else:
        return min(left_val, right_val)


def minimax_ab(
    node: Tree,
    alpha: int = MIN_VALUE,
    beta: int = MAX_VALUE,
    is_max: bool = True,
) -> int:
    """
    Minimax algorithm with alpha-beta pruning.

    Args:
        node (Tree): The current node in the game tree.
        alpha (float): The best found option for the maximizer.
        beta (float): The best found option for the minimizer.
        is_max (bool): True if the current level is the maximising player, False if minimizing

    Returns:
        int: The minimax value of the node.
    """
    # Return the value if it's a leaf node
    if node.data is not None:
        return node.data

    # Ensure the node has both children
    if node.left is None or node.right is None:
        raise ValueError("node must have both left and right children")

    if is_max:
        # Start with the lowest possible value
        value = MIN_VALUE
        for child in (node.left, node.right):
            # Recursively call minimax_ab for child nodes
            child_val = minimax_ab(child, alpha, beta, not is_max)
            # Update to the highest value found so far
            value = max(value, child_val)
            alpha = max(alpha, value)
            # Prune if current value is already better than beta
            if alpha >= beta:
                break
        return int(value)

    else:
        # Start with the highest possible value
        value = MAX_VALUE
        for child in (node.left, node.right):
            # Recursively call minimax_ab for child nodes
            child_val = minimax_ab(child, alpha, beta, not is_max)
            # Update to the lowest value found so far
            value = min(value, child_val)
            beta = min(beta, value)
            # Prune if current value is already worse than alpha
            if alpha >= beta:
                break
        return int(value)


tree = Tree()

tree.left = Tree()

tree.left.left = Tree()

tree.left.left.left = Tree()
tree.left.left.left.data = 4

tree.left.left.right = Tree()
tree.left.left.right.data = 8

tree.left.right = Tree()

tree.left.right.left = Tree()
tree.left.right.left.data = 9

tree.left.right.right = Tree()
tree.left.right.right.data = 3

tree.right = Tree()

tree.right.left = Tree()

tree.right.left.left = Tree()
tree.right.left.left.data = 2

tree.right.left.right = Tree()
tree.right.left.right.data = -2

tree.right.right = Tree()

tree.right.right.left = Tree()
tree.right.right.left.data = 9

tree.right.right.right = Tree()
tree.right.right.right.data = -1

# Max         o
#         /       \
# Min    o         o
#       /  \      /  \
# Max  o    o    o    o
#     / \  / \  / \  / \
#    4  8 9  3 2  -2 9 -1

if __name__ == "__main__":
    print("minimax:", minimax(tree))
    print("minimax with alpha-beta pruning:", minimax_ab(tree))
