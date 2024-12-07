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


* **Output:**  Prints the solution path (sequence of moves: U, D, L, R) to the console writes a log file. Additionally, draws the solution in a graphical window.
* **Deadlock Detection:** Includes basic deadlock detection to identify unsolvable configurations.
* **Error Handling:**  Includes basic error handling for invalid map inputs and file operations.


## Dependencies

* **C++ Compiler:**  A C++17 compatible compiler (like g++ or clang++).
* **SFML:** The Simple and Fast Multimedia Library for the graphics.  Install using your system's package manager (e.g., `apt-get install libsfml-dev` on Debian/Ubuntu).
* **X11:** For graphics on some systems.

## Compilation and Execution

1.  **Navigate to the project directory:** `cd sokoban_solver`

2.  **Compile:** `make` This will create `sokoban`.

3.  **Run:** `make run`  This will run the solver with input from `testcases/in`.

4.  **Run (specified testcase):** `make run TEST=i` This will run the solwer with the i-th big testcase.

5.  **Clean:** `make clean`  This will remove the compiled executables.


## File Structure

*   **`main.cpp`:** The main function.
*   **`sokoban.hpp`:** Header file containing the core Sokoban solver logic.
*   **`Makefile`:**  Makefile for compilation and execution.
*   **`pictures`:**  Directory containing image files for the graphics (`wall.jpg`, `player.jpg`, `box.jpg`, `target.jpg`, `path.jpg`, `box-on-target.jpg`).
*   **`testcases`:** folder containing testcases.