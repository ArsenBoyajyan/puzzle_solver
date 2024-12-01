#include <vector>
#include <string>
#include <iostream>
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <bitset>

uint16_t size;
void print_grid(std::vector<std::string> grid);


class coordinate {

    public:

    coordinate() : index(0) {}

    coordinate(uint16_t x, uint16_t y) : index( (uint16_t)  (y * size + x)) {}

    coordinate(const coordinate& other) : index(other.index) {}

    uint16_t get_x() const {
        return index % size;
    }

    uint16_t get_y() const {
        return index / size;
    }

    bool operator==(const coordinate& other) const {
        return index == other.index;
    }

    coordinate& operator=(const coordinate& other) {
        index = other.index;
        return *this;
    }

    coordinate upper() const {
        return coordinate(get_x(), get_y() - 1);
    }

    coordinate lower() const {
        return coordinate(get_x(), get_y() + 1);
    }

    coordinate left() const {
        return coordinate(get_x() - 1, get_y());
    }

    coordinate right() const {
        return coordinate(get_x() + 1, get_y());
    }

    private:
        uint16_t index;
};

bool check_invalide(const std::vector<std::string>& grid);

class state {

    public:
        state() : first(true) {}

        state(std::vector<std::string> &grid) {
            uint16_t rows = (uint16_t) grid.size();
            uint16_t cols = (uint16_t) grid[0].size();
            size = cols;
            // set_direction('U');
            first = true;
            path = std::vector<std::bitset<2>>();
            for (uint16_t i = 0; i < rows; ++i) {
                for (uint16_t j = 0; j < cols; ++j) {
                    if (grid[i][j] == 'B' ) {
                        coordinate box = coordinate(j, i);
                        grid[i][j] = '.';
                        boxes.push_back(box);
                    }
                    if (grid[i][j] == 'S') {
                        player = coordinate(j, i);
                        grid[i][j] = '.';
                    }
                    if (grid[i][j] == 'R') {
                        grid[i][j] = 'T';
                        boxes.push_back(coordinate(j, i));
                    }
                }
            }
        }

        state(const state& other) 
            : boxes(other.boxes), player(other.player), /*parent(other.parent),*/ path(other.path), first(other.first) {
        }

        state& operator=(const state& other) {
            // parent = other.parent;
            boxes = other.boxes;
            player = other.player;
            first = other.first;
            path = other.path;
            return *this;
        }

        bool operator==(const state& other) const {
            return boxes == other.boxes && player.get_x() == other.player.get_x() && player.get_y() == other.player.get_y();
        }

        std::string get_path(const std::vector<std::string> &grid) {
            // std::string result;
            // state* current = this;
            // while (current->parent != nullptr) {
            //     coordinate parent_player = current->parent->player;
            //     if (current->player.get_y() < parent_player.get_y()) {
            //         result.push_back('U');
            //     } else if (current->player.get_y() > parent_player.get_y()) {
            //         result.push_back('D');
            //     } else if (current->player.get_x() < parent_player.get_x()) {
            //         result.push_back('L');
            //     } else if (current->player.get_x() > parent_player.get_x()) {
            //         result.push_back('R');
            //     } else {
            //         std::cout<< "fuck";
            //         break;
            //     }
            //     current = current->parent;
            // }
            // std::reverse(result.begin(), result.end());
            // return result;

            std::string result;
            for (auto& direction : this->path) {
                switch (direction.to_ulong()) {
                    case 0:
                        result.push_back('U');
                        break;
                    case 1:
                        result.push_back('D');
                        break;
                    case 2:
                        result.push_back('L');
                        break;
                    case 3:
                        result.push_back('R');
                        break;
                    default:
                        break;
                }
            }
            return result;

            // std::string result;
            // state current = *this;
            // while (!current.first) {
            //     std::cout<<current.get_direction()<<std::endl;
            //     result.push_back(current.direction.to_ulong());
            //     current = current.back_track(grid);
            // }
            // return result;
        }

        state move (const std::vector<std::string> &grid, char direction) {
            state result = *this;
            add_path(direction);
            // set_direction(direction);
            switch (direction) {
                case 'U':
                    result.player = result.player.upper();
                    break;
                case 'D':
                    result.player = result.player.lower();
                    break;
                case 'L':
                    result.player = result.player.left();
                    break;
                case 'R':
                    result.player = result.player.right();
                    break;
                default:
                    break;
            }

            if (grid[result.player.get_y()][result.player.get_x()] == '#') {
                return *this; // Wall, do nothing
            }

            for (auto& box : result.boxes) {
                if (box.get_x() == result.player.get_x() && box.get_y() == result.player.get_y()) {
                    switch (direction) {
                        case 'U':
                            box = box.upper();
                            break;
                        case 'D':
                            box = box.lower();
                            break;
                        case 'L':
                            box = box.left();
                            break;
                        case 'R':
                            box = box.right();
                            break;
                        default:
                            break;
                    }
                    
                    if (grid[box.get_y()][box.get_x()] == '#' || 
                        std::any_of(boxes.begin(), boxes.end(), [&](coordinate b) { return b.get_x() == box.get_x() && b.get_y() == box.get_y(); })) {
                        return *this; // Obstacle behind the box, do nothing
                    }
                }
            }

            // result.parent = this;

            return result;
        }

        // state back_track(const std::vector<std::string> &grid) {
        //     state result = *this;
        //     // char dir = get_direction();
        //     // Restore previous state by reversing the direction
        //     switch (dir) {
        //         case 'U':
        //             result.player = result.player.lower();
        //             break;
        //         case 'D':
        //             result.player = result.player.upper();
        //             break;
        //         case 'L':
        //             result.player = result.player.right();
        //             break;
        //         case 'R':
        //             result.player = result.player.left();
        //             break;
        //         default:
        //             break;
        //     }

        //     for (auto& box : result.boxes) {
        //         if (box.get_x() == result.player.get_x() && box.get_y() == result.player.get_y()) {
        //             switch (dir) {
        //             case 'U':
        //                 box = box.lower();
        //                 break;
        //             case 'D':
        //                 box = box.upper();
        //                 break;
        //             case 'L':
        //                 box = box.right();
        //                 break;
        //             case 'R':
        //                 box = box.left();
        //                 break;
        //             default:
        //                 break;
        //             }
        //             if (grid[box.get_y()][box.get_x()] == '#' || 
        //                 std::any_of(boxes.begin(), boxes.end(), [&](coordinate b) { return b.get_x() == box.get_x() && b.get_y() == box.get_y(); })) {
        //                 return *this; // Obstacle behind the box, do nothing
        //             }
        //         }
        //     }

        //     return result;
        // }

        bool check_deadlock(const std::vector<std::string> &grid) {
            for (auto& box : boxes) {
                if (grid[box.get_y()][box.get_x()] == 'T') {
                    return false;
                }
                bool top_wall = (grid[box.get_y()-1][box.get_x()] == '#');
                bool bottom_wall = (grid[box.get_y()+1][box.get_x()] == '#');
                bool left_wall = (grid[box.get_y()][box.get_x()-1] == '#');
                bool right_wall = (grid[box.get_y()][box.get_x()+1] == '#');

                if ((top_wall || bottom_wall) && (left_wall || right_wall)) {
                    return true;
                }

                if ((top_wall && left_wall && grid[box.get_y() - 1][box.get_x()-1] == '#') || 
                    (top_wall && right_wall && grid[box.get_y() - 1][box.get_x() + 1] == '#') ||
                    (bottom_wall && left_wall && grid[box.get_y() + 1][box.get_x() - 1] == '#') ||
                    (bottom_wall && right_wall && grid[box.get_y() + 1][box.get_x() + 1] == '#')) {
                    return true;
                }
            }
            return false;
        }

        bool check_solved(const std::vector<std::string> &grid) {
            for (auto& box : boxes) {
                if (grid[box.get_y()][box.get_x()] != 'T') {
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
                    x = (unsigned char)box.get_x();
                    y = (unsigned char)box.get_y();
                    hash ^= std::hash<uint8_t>()(x) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
                    hash ^= std::hash<uint8_t>()(y) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
                }
                x = (unsigned char)s.player.get_x();
                y = (unsigned char)s.player.get_y();
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
                result[box.get_y()][box.get_x()] = 'B';
            }
            result[player.get_y()][player.get_x()] = 'S';
            return result;
        }
    
        
    private:
        // state* parent;
        std::vector<coordinate> boxes;
        coordinate player;
        bool first;
        
        std::vector<std::bitset<2>> path; // 00: up, 01: down, 10: left, 11: right
        // std::bitset<2> direction; // 00: up, 01: down, 10: left, 11: right
        
        // void set_direction(char direction) {
        //     first = false;
        //     switch (direction) {
        //         case 'U':
        //             this->direction = std::bitset<2>(0);
        //             break;
        //         case 'D':
        //             this->direction = std::bitset<2>(1);
        //             break;
        //         case 'L':
        //             this->direction = std::bitset<2>(2);
        //             break;
        //         case 'R':
        //             this->direction = std::bitset<2>(3);
        //             break;
        //         default:
        //             break;
        //     }
        // }

        // char get_direction() {
        //     switch (direction.to_ulong()) {
        //         case 0:
        //             return 'U';
        //         case 1:
        //             return 'D';
        //         case 2:
        //             return 'L';
        //         case 3:
        //             return 'R';
        //         default:
        //             return ' ';
        //     }
        // }
        void add_path(char direction) {
            switch (direction) {
                case 'U':
                    path.push_back(std::bitset<2>(0));
                    break;
                case 'D':
                    path.push_back(std::bitset<2>(1));
                    break;
                case 'L':
                    path.push_back(std::bitset<2>(2));
                    break;
                case 'R':
                    path.push_back(std::bitset<2>(3));
                    break;
                default:
                    break;
            }
        }
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
            for (char direction : "DRUL") {
                state new_state = current_state.move(grid, direction);
                print_grid(new_state.get_grid(grid));
                if (new_state.check_solved(grid)) {
                    return new_state.get_path(grid);
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

void print_grid(std::vector<std::string> grid) {
    for (unsigned long i = 0; i < grid.size(); ++i) {
        std::cout << grid[i] << std::endl;
    }
}