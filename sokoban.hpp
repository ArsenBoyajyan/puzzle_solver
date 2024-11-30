#include <vector>
#include <string>
#include <iostream>
#include <unordered_set>
#include <queue>
#include <algorithm>


struct coordinate {
    uint16_t x;
    uint16_t y;

    bool operator==(const coordinate& other) const {
        return x == other.x && y == other.y;
    }
};

bool check_invalide(const std::vector<std::string>& grid);

class state {
    public:
        state(std::vector<std::string> &grid) {
            uint16_t rows = (uint16_t) grid.size();
            uint16_t cols = (uint16_t) grid[0].size();
            parent = nullptr;
            for (uint16_t i = 0; i < rows; ++i) {
                for (uint16_t j = 0; j < cols; ++j) {
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

        state(const state& other) 
            : boxes(other.boxes), player(other.player), parent(other.parent) {
        }

        state& operator=(const state& other) {
            parent = other.parent;
            boxes = other.boxes;
            player = other.player;
            return *this;
        }

        bool operator==(const state& other) const {
            return boxes == other.boxes && player.x == other.player.x && player.y == other.player.y;
        }

        std::string get_path() {
            std::string result;
            state* current = this;
            while (current->parent != nullptr) {
                coordinate parent_player = current->parent->player;
                if (current->player.y < parent_player.y) {
                    result.push_back('U');
                } else if (current->player.y > parent_player.y) {
                    result.push_back('D');
                } else if (current->player.x < parent_player.x) {
                    result.push_back('L');
                } else if (current->player.x > parent_player.x) {
                    result.push_back('R');
                }
                current = current->parent;
            }
            std::reverse(result.begin(), result.end());
            return result;
        }
        state move (const std::vector<std::string> &grid, char direction) {
            state result = *this;
            result.parent = this;
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
                        std::any_of(boxes.begin(), boxes.end(), [&](coordinate b) { return b.x == box.x && b.y == box.y; })) {
                        return *this; // Obstacle behind the box, do nothing
                    }
                }
            }

            return result;
        }

        
        bool check_deadlock(const std::vector<std::string> &grid) {
            for (auto& box : boxes) {
                if (grid[box.y][box.x] == 'T') {
                    return false;
                }
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

        struct hash {
            size_t operator()(const state& s) const {
                size_t hash = 0;
                unsigned char x, y;
                for (const auto& box : s.boxes) {
                    x = (unsigned char)box.x;
                    y = (unsigned char)box.y;
                    hash ^= std::hash<uint8_t>()(x) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
                    hash ^= std::hash<uint8_t>()(y) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
                }
                x = (unsigned char)s.player.x;
                y = (unsigned char)s.player.y;
                hash ^= std::hash<uint8_t>()(x) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
                hash ^= std::hash<uint8_t>()(y) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
                return hash;
            }
        };

        bool visit(std::unordered_set<state, hash> &visited) {
            state s = *this;
            if (visited.find(s) != visited.end()) {
                return false;
            }
            visited.insert(s);
            return true;
        }

        std::vector<std::string> get_grid(const std::vector<std::string> &grid) {
            std::vector<std::string> result;
            result = grid;
            for (auto& box : boxes) {
                result[box.y][box.x] = 'B';
            }
            result[player.y][player.x] = 'S';
            return result;
        }
    private:
        std::vector<coordinate> boxes;
        coordinate player;
        state* parent;
        
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
    state start(grid);
    if (start.check_solved(grid)) {
        return "";
    }
    std::unordered_set<state, state::hash> visited;
    std::queue<state> q;
    q.push(start);
    while (!q.empty()) {
        
        state current_state = q.front();
        q.pop();

        if (current_state.visit(visited)) {
            for (char direction : "UDLR") {
                state new_state = current_state.move(grid, direction);
                if (new_state.check_solved(grid)) {
                    return new_state.get_path();
                }
                if (!new_state.check_deadlock(grid)) {
                    q.push(new_state);
                }
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
    "LLRDDLLUULUUURRDLDUURRRDLLULLDDRUDDDURRRU",
    "DRUUUULUURDRRURDDDRDLDDRLULRUULD",
    "RRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRR",
    "ans for big 5",
    "LRURURULUULLDRURRDLDDRUDDDLLLLURUURUL",
    "ans for big 7",
    "ULDDDRDLLLLUURDLDRUUDDRRULDLU",
    "RDDDDUULLLDDLLURDRRRDRLRUULULLDLDRRLUURRDD",
    "LLLDDLRRRRRUUUULDDDRDLLLLULURUULURDRR",
    "ans for big 11",
    "ans for big 12",
    "ans for big 13",
    "ans for big 14",
    "ans for big 15"
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