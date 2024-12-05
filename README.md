# Sokoban Solver

This project implements a Sokoban solver using C++ and the SFML library (optional graphics mode).  The solver finds a solution path to move all boxes to their target positions.

## Features

* **Solver:**  Uses a Breadth-First Search (BFS) algorithm to find the shortest path to solve the puzzle.  Supports maps with up to 8 boxes.
* **Input:** Reads Sokoban map configurations from a text file (specified using `< input.txt`).  Hereis a sample input:

```
11 10
###########
#..#..#...#
#..B....#.#
#..##.#...#
##.#...B###
##.#.....##
#SBBTT#..##
#..#.TT.###
####..#####
###########
```


* **Output:**  Prints the solution path (sequence of moves: U, D, L, R) to the console.  Additionally, writes a log file (`log.txt`) detailing the steps and visualization to the `log.txt` file.
* **Graphics (Optional):**  A separate executable (`sokoban_graph`) provides a visual representation of the solution process using SFML. Requires image files for visualization.
* **Deadlock Detection:** Includes basic deadlock detection to identify unsolvable configurations.
* **Error Handling:**  Includes basic error handling for invalid map inputs and file operations.


## Dependencies

* **C++ Compiler:**  A C++17 compatible compiler (like g++ or clang++).
* **SFML (Optional):** The Simple and Fast Multimedia Library for the graphics mode.  Install using your system's package manager (e.g., `apt-get install libsfml-dev` on Debian/Ubuntu).
* **X11 (Optional):** For graphics mode on some systems.

## Compilation and Execution

1.  **Navigate to the project directory:** `cd sokoban_solver`

2.  **Compile (without graphics):** `make` This will create `sokoban_nog`.

3.  **Compile (with graphics):** `make` This will create `sokoban_graph`. (Requires SFML and possibly X11 to be installed).

4.  **Run (without graphics):** `make run_nog`  This will run the solver with input from `testcases/in`.

5.  **Run (with graphics):** `make run_graph` This will run the solver with graphics using input from `testcases/in`. Make sure `pictures` directory with images is present.

6.  **Clean:** `make clean`  This will remove the compiled executables.


## File Structure

*   **`main.cpp`:** The main function for the non-graphics version.
*   **`show.cpp`:** The main function for the graphics version.
*   **`sokoban.hpp`:** Header file containing the core Sokoban solver logic.
*   **`Makefile`:**  Makefile for compilation and execution.
*   **`pictures`:**  (Optional) Directory containing image files for the graphics mode (`wall.jpg`, `player.jpg`, `box.jpg`, `target.jpg`, `path.jpg`, `box-on-target.jpg`).
*   **`testcases`:** Place your `in` file here.