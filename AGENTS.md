# Project Overview

This project is a university computer science assignment to develop a tic-tac-toe minigame enhanced with rudimentary machine learning (minimax, reinforcement learning, naive bayes) with C and GTK. The goal is to have both two-player and one-player mode, with a user-friendly GUI. The code should be well-documented and follow best practices for each language. Python will be used for easier prototyping.

## Libraries and Frameworks

- GTK 4.0
- GStreamer

## Coding Standards

- Use meaningful variable and function names.
- Include comments to explain complex logic.
- Write modular code with functions.
- Include error handling where appropriate.
- Follow the C99 standard and CppCheck's `--enable-all` warnings for code style.

## Developer Environment

- Dev Container is used for a standardised development environment
  - The configuration file can be found at `.devcontainer/devcontainer.json`
- Pre-Commit is used to identify any code issues before PR submission
  - The configuration file can be found at `.pre-commit-config.yaml`
- CppCheck is used for linting C/C++ code
  - It is used with `--enable=all --suppress=unmatchedSuppression --suppress=missingIncludeSystem --check-level=exhaustive`
- clang-format is used for formatting C/C++ code
  - It is used with `--style=google`

## Pull Requests

- The PR title should follow Conventional Commits
  - e.g. `feat(auth): implement login function`
- Pre-commit hooks should be ran before committing any changes
- When reviewing a PR, ensure all best practices and standards are met
