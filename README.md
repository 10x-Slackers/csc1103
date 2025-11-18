# CSC1103 Mini Project

AI-enhanced tic-tac-toe minigame

---

## Project Scope

- [link_to_source](link_to_source)
  - scope_description

## Usage

### runtime_executable

```sh
./runtime_executable <args> [optional]
```

- runtime_executable_description

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
  - [gtk4](https://packages.msys2.org/packages/mingw-w64-ucrt-x86_64-gtk4?repo=ucrt64)

#### For building on MacOS (Optional)

- [Homebrew](https://brew.sh/)
  - [pkgconf](https://formulae.brew.sh/formula/pkgconf)
  - [Meson](https://formulae.brew.sh/formula/meson)
  - [gtk+](https://formulae.brew.sh/formula/gtk+)

> [!WARNING]
> Do not use GitHub Desktop! All interactions (files, git, runtime, etc.) should be done through the Dev Container within VS Code.

### Installation

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
2. Ensure that gcc, Meson, and GTK4 library are installed

   ```sh
   pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-meson mingw-w64-ucrt-x86_64-gtk4
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

7. To run it without using the shell, copy the required DLLs into the same directory as the binary

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

   - You may either run it via the Terminal or Finder

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
