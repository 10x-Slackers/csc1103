# Project Overview

This project is a university computer science assignment to [scope]. The goal is to [objective]. The code should be well-documented and follow best practices for each language.

## Project Structure

- `folder/`
  - folder_description

## Libraries and Frameworks

- No external libraries or frameworks are used, or should be used.

## Coding Standards

- Use meaningful variable and function names.
- Include comments to explain complex logic.
- Write modular code with functions.
- Include error handling where appropriate.

### Python

- Follow PEP 8 guidelines for code style.
- Include docstrings for all functions and classes.
- Utilize type hints for function signatures.

### C

- Follow the C99 standard for code style.

## Developer Environment

- Dev Container is used for a standardised development environment
  - The configuration file can be found at `.devcontainer/devcontainer.json`
- Pre-Commit is used to identify any code issues before PR submission
  - The configuration file can be found at `.pre-commit-config.yaml`
- Ruff is used for linting and formatting Python code
  - The configuration file can be found at `ruff.toml`
- CppCheck is used for linting C/C++ code
  - It is used with `--enable=all --suppress=missingIncludeSystem`
- clang-format is used for formatting C/C++ code
  - It is used with `-style=google`

## Pull Requests

- The PR title should follow Conventional Commits
  - e.g. `feat(auth): implement login function`
- Pre-commit hooks should be ran before committing any changes
- When reviewing a PR, ensure all best practices and standards are met
