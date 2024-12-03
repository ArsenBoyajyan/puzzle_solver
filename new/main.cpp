#include "sokoban.hpp"
#include <fstream>

int main() {
    std::vector<std::string> grid;
    read_map(grid);
    std::vector<std::string> grid_copy = grid;

    std::string result = solve(grid);

    state s;
    s.init(grid_copy);

    std::ofstream log("log.txt");
    if (log.is_open()) {
        if (result == "No solution!") {
            log << "No solution!" << std::endl;
            log.close();
            return 0;
        }
        log << "Solved!!!\n";
        for (char c : result) {
            std::vector<std::string> temp_grid = s.get_grid(grid);
            for (auto& line : temp_grid) {
                log << line << std::endl;
            }
            log << "-----------------------\n";
            s = s.move(temp_grid, c);
        }
        std::vector<std::string> temp_grid = s.get_grid(grid);
        for (auto& line : temp_grid) {
            log << line << std::endl;
        }
        log.close();
    } else {
        std::cout << "Unable to open log file" << std::endl;
    }

    std::cout << result << std::endl;

    return 0;
}


// #include "sokoban.hpp"

// using namespace std;

// int main() {
//     std::vector<std::string> grid;
//     read_map(grid);

//     // Create a state object from the grid
//     state s = state();
//     s.init(grid);
//     vector<string> grid_copy = s.get_grid(grid);
//     for (auto& line : grid_copy) {
//         cout<<line<<endl;
//     }

//     s = s.move(grid_copy, 'D');

//     grid_copy = s.get_grid(grid);
//     for (auto& line : grid_copy) {
//         cout<<line<<endl;
//     }


//     return 0;
// }
