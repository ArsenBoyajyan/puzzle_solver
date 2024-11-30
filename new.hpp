#include <vector>
#include <string>
#include <iostream>
#include <unordered_set>
#include <queue>
#include <bitset>
#include <algorithm>


struct coordinate {
    uint8_t x;
    uint8_t y;
};

bool check_invalide(const std::vector<std::string>& grid);

class state {
    public:
        state(std::vector<std::string> &grid) {
            uint8_t rows = (uint8_t) grid.size();
            uint8_t cols = (uint8_t) grid[0].size();
            for (uint8_t i = 0; i < rows; ++i) {
                for (uint8_t j = 0; j < cols; ++j) {
                    if (grid[i][j] == 'B' ) {
                        coordinate box = {j, i};
                        grid[i][j] = '.';
                        boxes.push_back(box);
                    }
                    if (grid[i][j] == 'S') {
                        player = {j, i};
                        grid[i][j] = '.';
                    }
                    if (grid[i][j] == 'R') {
                        grid[i][j] = 'T';
                        boxes.push_back({j, i});
                    }
                }
            }
        }

        state& operator=(const state& other) {
            path = other.path;
            boxes = other.boxes;
            player = other.player;
            return *this;
        }

        // bool operator==(const state& other) const {
        //     return boxes == other.boxes && player.x == other.player.x && player.y == other.player.y;
        // }

        void add_path(char direction) {
            switch (direction) {
                case 'U':
                    path.push_back(0);
                    break;
                case 'D':
                    path.push_back(1);
                    break;
                case 'L':
                    path.push_back(2);
                    break;
                case 'R':
                    path.push_back(3);
                    break;
                default:
                    break;
            }
        }

        std::string get_path() {
            std::string result;
            for (uint8_t i = 0; i < path.size(); ++i) {
                switch (path[i].to_ulong()) {
                    case 0:
                        result += 'U';
                        break;
                    case 1:
                        result += 'D';
                        break;
                    case 2:
                        result += 'L';
                        break;
                    case 3:
                        result += 'R';
                        break;
                    default:
                        break;
                }
            }
            return result;
        }
    
        state move (const std::vector<std::string> &grid, char direction) {
            state result = *this;
            switch (direction) {
                case 'U':
                    result.player.y = result.player.y - 1;
                    break;
                case 'D':
                    result.player.y = result.player.y + 1;
                    break;
                case 'L':
                    result.player.x = result.player.x - 1;
                    break;
                case 'R':
                    result.player.x = result.player.x + 1;
                    break;
                default:
                    break;
            }

            if (grid[result.player.y][result.player.x] == '#') {
                return *this; // Wall, do nothing
            }

            for (auto& box : result.boxes) {
                if (box.x == result.player.x && box.y == result.player.y) {
                    switch (direction) {
                        case 'U':
                            box.y = box.y - 1;
                            break;
                        case 'D':
                            box.y = box.y + 1;
                            break;
                        case 'L':
                            box.x = box.x - 1;
                            break;
                        case 'R':
                            box.x = box.x + 1;
                            break;
                        default:
                            break;
                    }
                    
                    if (grid[box.y][box.x] == '#' || 
                        std::any_of(result.boxes.begin(), result.boxes.end(), [&](coordinate b) { return b.x == box.x && b.y == box.y; })) {
                        return *this; // Obstacle behind the box, do nothing
                    }
                }
            }
            
            result.add_path(direction);
            return result;
        }

        bool check_deadlock(const std::vector<std::string> &grid) {
            for (auto& box : boxes) {
                bool top_wall = (grid[box.y-1][box.x] == '#');
                bool bottom_wall = (grid[box.y+1][box.x] == '#');
                bool left_wall = (grid[box.y][box.x-1] == '#');
                bool right_wall = (grid[box.y][box.x+1] == '#');

                if ((top_wall || bottom_wall) && (left_wall || right_wall)) {
                    return true;
                }

                if ((top_wall && left_wall && grid[box.y - 1][box.x-1] == '#') || 
                    (top_wall && right_wall && grid[box.y - 1][box.x + 1] == '#') ||
                    (bottom_wall && left_wall && grid[box.y + 1][box.x - 1] == '#') ||
                    (bottom_wall && right_wall && grid[box.y + 1][box.x + 1] == '#')) {
                    return true;
                }
            }
            return false;
        }

        bool check_solved(const std::vector<std::string> &grid) {
            for (auto& box : boxes) {
                if (grid[box.y][box.x] != 'T') {
                    return false;
                }
            }
            return true;
        }

        // struct hash {
        //     // size_t operator()(const state& s) const {
        //     //     size_t result = 0;
        //     //     for (const auto& box : s.boxes) {
        //     //         result ^= std::hash<uint8_t>{}(box.x) + std::hash<uint8_t>{}(box.y);
        //     //     }
        //     //     result ^= std::hash<uint8_t>{}(s.player.x) + std::hash<uint8_t>{}(s.player.y);
        //     //     return result;
        //     // }

        //     size_t operator()(const std::vector<coordinate>& boxes, const coordinate& player) const {
        //         size_t hash = 0;
        //         for (const auto& box : boxes) {
        //             hash ^= std::hash<uint8_t>()(box.x) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        //             hash ^= std::hash<uint8_t>()(box.y) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        //         }
        //         hash ^= std::hash<uint8_t>()(player.x) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        //         hash ^= std::hash<uint8_t>()(player.y) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        //         return hash;
        //     }
        // };

        // bool visit(std::unordered_set<state, hash> &visited) {
        //     state s = *this;
        //     if (visited.find(s) != visited.end()) {
        //         return false;
        //     }
        //     visited.insert(s);
        //     return true;
        // }

        std::vector<std::string> return_grid(const std::vector<std::string> &grid) {
            std::vector<std::string> result;
            result = grid;
            for (auto& box : boxes) {
                result[box.y][box.x] = 'B';
            }
            result[player.y][player.x] = 'S';
            return result;
        }
    private:
        std::vector<std::bitset<2>> path; // 0 - up, 1 - down, 2 - left, 3 - right
        std::vector<coordinate> boxes;
        coordinate player;
        
};

/**
 * @brief  Read your map from stdin
 * @note   Input format: See project description
 * @param  &grid: You should first resize the grid! Modify it inplace.
 * @retval None
 */
void read_map(std::vector<std::string> &grid) {
    unsigned long n, m;
    std::cin >> m >> n;
    grid.resize(n, std::string(m, ' '));
    for (unsigned long i = 0; i < n; ++i) {
        std::cin >> grid[i];
    }
}

/**
 * @brief  Solve the sokoban described by the grid
 * @note   Output string format: See project description
 * @param  &grid: The grid you read by `read_map'
 * @retval 
 */
std::string solve(std::vector<std::string> &grid){
    if (check_invalide(grid)) {
        return "No solution!";
    }
    state s(grid);
    // std::unordered_set<state, state::hash> visited;
    std::queue<state> q;
    q.push(s);
    while (!q.empty()) {
        state s = q.front();
        q.pop();
        if (s.check_solved(grid)) {
            return s.get_path();
        }
        if (s.check_deadlock(grid)) {
            continue;
        }
        // if (s.visit(visited)) {
        //     for (char direction : "UDLR") {
        //         state new_state = s.move(grid, direction);
        //         q.push(new_state);
        //     }
        // }
        if (true) {
            for (char direction : "UDLR") {
                state new_state = s.move(grid, direction);
                q.push(new_state);
            }
        }
    }
    return "No solution!";
}

// For big cases, paste your answers here (i.e. replace "ans for big 1" with your answer)
// Do not remove the first element, it should be left blank.
// NOTE: Ensure the order is correct!
// Your answer should look like "UUDDLLRR..."
const std::vector<std::string> answers = {
    "__leave_this_blank__", 
    "ans for big 1",
    "ans for big 2",
    "ans for big 3",
    "ans for big 4",
    "ans for big 5",
    "ans for big 6",
    "ans for big 7",
    "ans for big 8",
    "ans for big 9",
    "ans for big 10"
};

// Don't modify this.
std::string print_answer(int index) {
    if (index < 1 || index >= (int)answers.size()) {
        return "invalid";
    }
    return answers[(size_t)index];
}

bool check_invalide(const std::vector<std::string>& grid) {
    unsigned long rows = (unsigned long) grid.size();
    unsigned long cols = (unsigned long) grid[0].size();
    int start_count = 0;
    int box_count = 0;
    int target_count = 0;

    for (unsigned long i = 0; i < rows; ++i) {
        for (unsigned long j = 0; j < cols; ++j) {
            char cell = grid[i][j];
            if (cell == 'S') start_count++;
            else if (cell == 'B') box_count++;
            else if (cell == 'T') target_count++;
        }
    }

    //Check for multiple starts or no starts
    if (start_count != 1) return true;

    //Check if more boxes than targets
    if (box_count > target_count) return true;

    //Check for immovable boxes (simplified check - could be improved)
    for (unsigned long i = 1; i < rows - 1; ++i) {
        for (unsigned long j = 1; j < cols - 1; ++j) {
            if (grid[i][j] == 'B') {
                // Check for box in a corner
                bool top_wall = (grid[i-1][j] == '#');
                bool bottom_wall = (grid[i+1][j] == '#');
                bool left_wall = (grid[i][j-1] == '#');
                bool right_wall = (grid[i][j+1] == '#');

                if ((top_wall || bottom_wall) && (left_wall || right_wall)) {
                    return true;
                }

                // Check for box against two walls in a T-junction
                if ((top_wall && left_wall && grid[i-1][j-1] == '#') || 
                    (top_wall && right_wall && grid[i-1][j+1] == '#') ||
                    (bottom_wall && left_wall && grid[i+1][j-1] == '#') ||
                    (bottom_wall && right_wall && grid[i+1][j+1] == '#')) {
                    return true;
                }
            }
        }
    }

    return false;
}