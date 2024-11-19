#include "sokoban.hpp"

int main() {
    std::vector<std::string> grid;
    read_map(grid); 

    std::string result = solve(grid);

    position p;
    p.grid = grid;

    // for (int i = 0; i < (int)result.size(); ++i) {
    //     switch(result[i]) {
    //         case 'U':
    //             get_up(p, p);
    //             for (int j =0; j < (int)grid.size(); ++j) {
    //                 std::cout << p.grid[j] << std::endl;
    //             }
    //             std::cout << "UP\n";
    //             break;
    //         case 'D':
    //             get_down(p, p);
    //             for (int j =0; j < (int)grid.size(); ++j) {
    //                 std::cout << p.grid[j] << std::endl;
    //             }
    //             std::cout << "DOWN\n";
    //             break;
    //         case 'L':
    //             get_left(p, p);
    //             for (int j =0; j < (int)grid.size(); ++j) {
    //                 std::cout << p.grid[j] << std::endl;
    //             }
    //             std::cout << "LEFT\n";
    //             break;
    //         case 'R':
    //             get_right(p, p);
    //             for (int j =0; j < (int)grid.size(); ++j) {
    //                 std::cout << p.grid[j] << std::endl;
    //             }
    //             std::cout << "RIGHT\n";
    //             break;
    //         default:
    //             break;
    //     }
    // }

    std::cout << result << std::endl;
    return 0;
}