# CSC1103 Programming Methodology Mini Project 2025

Kid-friendly 3x3 Tic-Tac-Toe game with artificial intelligence in C, for a compute limited tablet.

---

## Project Scope

- [Tic-Tac-Toe GUI Game](src/app/)
  - [GUI Application](src/app/main.c)
    - Developed with GTK 4 for cross-platform compatibility
    - Supports 2-player mode and 1-player mode against 3 AI difficulties
    - 4 main screens:
      - [Main Menu](src/app/main_menu.c)
      - [Difficulty Selection](src/app/option_menu.c)
      - [Player Selection](src/app/option_menu.c)
      - [Game Grid](src/app/board_gui.c)
  - [Game State Management](src/app/game.c)
  - [Audio System](src/app/audio.c)
    - Developed with GStreamer
    - Background music and sound effects
- [Machine Learning CLI program](src/ml/)
  - [Training](src/ml/training.c)
    - Train a Naive Bayes classifier on a dataset of terminal Tic-Tac-Toe boards and outcomes
  - [Evaluation Metrics](src/ml/statistics.c)
    - Calculate confusion matrix, accuracy, precision, recall, and F1-score for the trained model
  - [Benchmarking](src/ml/benchmark.c)
    - Benchmark various AI algorithms
    - Calculate win rates against random opponent
    - Calculate average response time to return a move
- [Core Tic-Tac-Toe Logic](src/shared/board.c)
  - Modular board representation
  - Game state checking (win/draw/ongoing)
  - Move management with 1-move undo
  - Random move generation for AI
- [Minimax Algorithm](src/shared/minimax.c)
  - Implementation of the minimax algorithm for Tic-Tac-Toe with alpha-beta pruning and imperfection
  - Highly efficient with tuned sample move and depth limitation
- [Naive Bayes Classifier](src/shared/naive_bayes.c)
  - Implementation of a Naive Bayes classifier for Tic-Tac-Toe AI
  - Trained on a dataset of terminal Tic-Tac-Toe boards and outcomes to predict optimal moves

## Usage

### Tic-Tac-Toe Game

```sh
./tictactoe
```

- Add `.exe` suffix on Windows
- Requires a display output (cannot run in Dev Container)

### ML CLI Program

```sh
./ml-cli <train/stats/benchmark> [-d <dataset_file>] [-m <model_file>]
```

- Add `.exe` suffix on Windows
- Use `train` to train a Naive Bayes model on the dataset file
- Use `stats` to evaluate the model file on the testing split of the dataset file
- Use `benchmark` to benchmark win rate and response time of various AI algorithms

## Getting Started

### Prerequisites

- [Git](https://github.com/git-guides/install-git) (fully set-up)
- [Docker/Podman](https://docs.docker.com/engine/install/)
- [VS Code](https://code.visualstudio.com/download)
  - [Dev Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers) extension

#### For building on Windows (Optional)

- [MSYS2](https://msys2.org/)
  - [gcc](https://packages.msys2.org/packages/mingw-w64-ucrt-x86_64-gcc?repo=ucrt64)
  - [meson](https://packages.msys2.org/packages/mingw-w64-ucrt-x86_64-meson?repo=ucrt64)
  - [gstreamer](https://packages.msys2.org/packages/mingw-w64-ucrt-x86_64-gstreamer?repo=ucrt64)
  - [gst-plugins-base](https://packages.msys2.org/packages/mingw-w64-ucrt-x86_64-gst-plugins-base?repo=ucrt64)
  - [gtk4](https://packages.msys2.org/packages/mingw-w64-ucrt-x86_64-gtk4?repo=ucrt64)
  - [xxd](https://packages.msys2.org/base/vim) (part of vim package)

#### For building on MacOS (Optional)

- [Homebrew](https://brew.sh/)
  - [pkgconf](https://formulae.brew.sh/formula/pkgconf)
  - [Meson](https://formulae.brew.sh/formula/meson)
  - [gstreamer](https://formulae.brew.sh/formula/gstreamer)
  - [gtk+](https://formulae.brew.sh/formula/gtk+)

> [!WARNING]
> Do not use GitHub Desktop! All interactions (files, git, runtime, etc.) should be done through the Dev Container within VS Code.

### Setting Up

1. Clone the repo

   ```sh
   git clone git@github.com:10x-Slackers/csc1103.git
   ```

2. Open the repository in VS Code

   ```sh
   code csc1103/
   ```

3. Click on the "Re-open in Dev Container" prompt
4. Start working!

### Building

#### Linux

1. Within the Dev Container, setup the Meson build directory

   ```sh
   meson setup builddir
   ```

2. Run the build via Meson

   ```c
   meson compile -C builddir
   ```

3. The binary can be found in `builddir/src`

> [!NOTE]
> The GUI app will not run in the Dev Container due to the lack of a display output. Run it on the host machine instead.

#### Windows

1. Launch the MSYS2 UCRT64 shell
   - Ensure that MSYS2 is already installed
   - Try searching from the start menu if you cannot find it
2. Ensure that required libraries are installed

   ```sh
   pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-meson mingw-w64-ucrt-x86_64-gstreamer mingw-w64-ucrt-x86_64-gst-plugins-base mingw-w64-ucrt-x86_64-gtk4 vim
   ```

3. Clone or copy the repository files into a location accessible by the shell
4. Setup the Meson build directory

   ```sh
   meson setup builddir
   ```

5. Run the build via Meson

   ```c
   meson compile -C builddir
   ```

6. The binary can be found in `builddir/src`

   - You may run it via the MSYS2 shell

   ```sh
   ./builddir/src/tictactoe.exe
   ```

7. To run it without using the shell (e.g. using File Explorer), copy the required DLLs into the same directory as the binary

   ```sh
   cd builddir/src && \
   ldd tictactoe.exe | grep '\/ucrt64.*\.dll' -o | xargs -I{} cp "{}" .
   ```

#### MacOS

> [!NOTE]
> Due to the lack of a MacOS machine among the project members, we were not able to test building on MacOS.

1. Launch Terminal
2. Ensure that Homebrew, pkgconf, Meson, and GTK4 library are installed

   ```sh
   brew install pkgconf meson gtk+
   ```

3. Setup the Meson build directory

   ```sh
   meson setup builddir
   ```

4. Run the build via Meson

   ```c
   meson compile -C builddir
   ```

5. The binary can be found in `builddir/src`

   ```sh
   ./builddir/src/tictactoe
   ```

## Developer Tooling

- Dev Containers
  - Standardised developer environment
- CppCheck
  - C/C++ code linting
- clang-format
  - C/C++ code formatting
- Pre-Commit
  - Run linting and formatting for all files during git commit
