#include "sokoban.hpp"
#include <fstream>

int main() {
    std::vector<std::string> grid;
    read_map(grid);

    std::string result = solve(grid);

    // position p;
    // p.grid = grid;

    // std::ofstream log("log.txt");
    // if (result == "No solution!") {
    //     log << "No solution!" << std::endl;
    //     std::cout<<"No solution!" << std::endl;
    //     return 0;
    // }

    // log << "Solved!!!:" << std::endl;

    // for (int i = 0; i < (int)result.size(); ++i) {
    //     switch(result[i]) {
    //         case 'U':
    //             get_up(p, p);
    //             for (int j =0; j < (int)grid.size(); ++j) {
    //                 log << p.grid[j] << std::endl;
    //             }
    //             log << "--------------------\n";
    //             break;
    //         case 'D':
    //             get_down(p, p);
    //             for (int j =0; j < (int)grid.size(); ++j) {
    //                 log << p.grid[j] << std::endl;
    //             }
    //             log << "--------------------\n";
    //             break;
    //         case 'L':
    //             get_left(p, p);
    //             for (int j =0; j < (int)grid.size(); ++j) {
    //                 log << p.grid[j] << std::endl;
    //             }
    //             log << "--------------------\n";
    //             break;
    //         case 'R':
    //             get_right(p, p);
    //             for (int j =0; j < (int)grid.size(); ++j) {
    //                 log << p.grid[j] << std::endl;
    //             }
    //             log << "--------------------\n";
    //             break;
    //         default:
    //             break;
    //     }
    // }

    std::cout << result << std::endl;

    // log.close();
    return 0;
}
