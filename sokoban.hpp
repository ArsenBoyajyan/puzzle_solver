#include <vector>
#include <string>
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <bitset>
#include <math.h>

uint16_t size;
bool check_invalid(const std::vector<std::string>& grid);

class coordinate {

    public:

    coordinate() : index(0) {}

    coordinate(uint16_t x, uint16_t y) : index(std::bitset<8>(x + y * size)) {}

    coordinate(const coordinate& other) : index(other.index) {}

    uint16_t get_x() const {
        return get_value() % size;
    }

    uint16_t get_y() const {
        return get_value() / size;
    }

    bool operator==(const coordinate& other) const {
        return index == other.index;
    }

    coordinate& operator=(const coordinate& other) {
        index = other.index;
        return *this;
    }

    bool operator!=(const coordinate& other) const {
        return !(*this == other);
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
        std::bitset<8> index;
        uint16_t get_value() const {
            return static_cast<uint16_t>(index.to_ulong());
        }
};

coordinate start;

class state {

    public:
        state() : boxes(std::vector<coordinate>()), player(coordinate()), direction(std::bitset<2>()), pushed(false) {}

        state(std::vector<std::string> &grid) {
            uint16_t rows = (uint16_t) grid.size();
            uint16_t cols = (uint16_t) grid[0].size();
            pushed = false;
            size = cols;
            for (uint16_t i = 0; i < rows; ++i) {
                for (uint16_t j = 0; j < cols; ++j) {
                    if (grid[i][j] == 'B' ) {
                        coordinate box = coordinate(j, i);
                        grid[i][j] = '.';
                        boxes.push_back(box);
                    }
                    if (grid[i][j] == 'S') {
                        player = coordinate(j, i);
                        start = player;
                        grid[i][j] = '.';
                    }
                    if (grid[i][j] == 'R') {
                        grid[i][j] = 'T';
                        boxes.push_back(coordinate(j, i));
                    }
                }
            }
            boxes.shrink_to_fit();
        }
        state(const state& other) : boxes(other.boxes), player(other.player), direction(other.direction), pushed(other.pushed) {}

        state& operator=(const state& other) {
            boxes = other.boxes;
            player = other.player;
            direction = other.direction;
            pushed = other.pushed;
            return *this;
        }

        bool operator==(const state& other) const {
            return boxes == other.boxes && player.get_x() == other.player.get_x() && player.get_y() == other.player.get_y();
        }

        // Hash for a vector of coordinates
        struct VectorCoordinateHash {
            std::size_t operator()(const std::vector<coordinate>& v) const {
                std::size_t hash = 0;
                for (const auto& coord : v) {
                    CoordinateHash coordinateHasher;
                    hash ^= coordinateHasher(coord) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
                }
                return hash;
            }
        };

        std::string get_path(std::unordered_map<std::vector<coordinate>, std::bitset<3>, VectorCoordinateHash> &visited) {
            std::string path = "";
            state current_state = *this;
            while (current_state.player != start) {
                std::vector<coordinate> v = current_state.boxes;
                v.push_back(current_state.player);
                auto it = visited.find(v);
                if (it == visited.end()) {
                    return "error";
                }
                std::bitset<3> info = it->second;
                std::bitset<2> dir;
                dir.set(0, info[0]);
                dir.set(1, info[1]);
                char c = current_state.get_direction(dir);
                path += c;
                bool with_box = info[2];
                current_state = current_state.move_back(dir, with_box);
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        state move (const std::vector<std::string> &grid, char direction) {
            state result = *this;
            result.pushed = false;
            result.set_direction(direction);
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
                    result.pushed = true;
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

            return result;
        }

        bool check_deadlock(const std::vector<std::string> &grid) {
            for (auto& box : boxes) {
                // If the box is on a target, it's not a deadlock
                if (grid[box.get_y()][box.get_x()] == 'T') {
                    continue;
                }

                // Walls around the box
                bool top_wall = (grid[box.get_y()-1][box.get_x()] == '#');
                bool bottom_wall = (grid[box.get_y()+1][box.get_x()] == '#');
                bool left_wall = (grid[box.get_y()][box.get_x()-1] == '#');
                bool right_wall = (grid[box.get_y()][box.get_x()+1] == '#');

                // Simple corner deadlock
                if ((top_wall || bottom_wall) && (left_wall || right_wall)) {
                    return true;
                }

                bool top_left_wall = (grid[box.get_y()-1][box.get_x()-1] == '#');
                bool top_right_wall = (grid[box.get_y()-1][box.get_x()+1] == '#');
                bool bottom_left_wall = (grid[box.get_y()+1][box.get_x()-1] == '#');
                bool bottom_right_wall = (grid[box.get_y()+1][box.get_x()+1] == '#');

                // L-shape or boxed-in structures
                if ((top_wall && left_wall && bottom_left_wall) ||  // L-shape top-left
                    (top_wall && right_wall && bottom_right_wall) || // L-shape top-right
                    (bottom_wall && left_wall && top_left_wall) ||  // L-shape bottom-left
                    (bottom_wall && right_wall && top_right_wall)) { // L-shape bottom-right
                    return true;
                }

                // Tightly packed boxes
                for (auto& other_box : boxes) {
                    if (box == other_box) continue; // Skip self

                    // Packed horizontally
                    if (box.get_y() == other_box.get_y() && 
                        std::abs(box.get_x() - other_box.get_x()) == 1) {
                        bool vertical_wall1 = (grid[box.get_y()-1][box.get_x()] == '#' || grid[box.get_y()+1][box.get_x()] == '#');
                        bool vertical_wall2 = (grid[other_box.get_y()-1][other_box.get_x()] == '#' || grid[other_box.get_y()+1][other_box.get_x()] == '#');
                        if (vertical_wall1 && vertical_wall2) {
                            return true;
                        }
                    }

                    // Packed vertically
                    if (box.get_x() == other_box.get_x() &&
                        std::abs(box.get_y() - other_box.get_y()) == 1) {
                        bool horizontal_wall1 = (grid[box.get_y()][box.get_x()-1] == '#' || grid[box.get_y()][box.get_x()+1] == '#');
                        bool horizontal_wall2 = (grid[other_box.get_y()][other_box.get_x()-1] == '#' || grid[other_box.get_y()][other_box.get_x()+1] == '#');
                        if (horizontal_wall1 && horizontal_wall2) {
                            return true;
                        }
                    }
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

        bool visit(std::unordered_map<std::vector<coordinate>, std::bitset<3>, VectorCoordinateHash> &visited) {
            std::vector<coordinate> v = boxes;
            v.push_back(player);
            if (visited.find(v) != visited.end()) {
                return false;
            }
            std::bitset<3> info;
            info.set(0, direction[0]);
            info.set(1, direction[1]);
            info.set(2, pushed);
            visited.insert({v, info});
            return true;
        }

        std::vector<std::string> get_grid(const std::vector<std::string> &grid) {
            std::vector<std::string> result;
            result = grid;
            for (auto& box : boxes) {
                if (grid[box.get_y()][box.get_x()] == 'T') {
                    result[box.get_y()][box.get_x()] = 'R';
                } else {
                    result[box.get_y()][box.get_x()] = 'B';
                }
            }
            result[player.get_y()][player.get_x()] = 'S';
            return result;
        }
    
        
    private:
        std::vector<coordinate> boxes;
        coordinate player;
        std::bitset<2> direction; // 00: up, 01: down, 10: left, 11: right
        bool pushed;

        struct CoordinateHash {
            std::size_t operator()(const coordinate& coord) const {
                return std::hash<uint16_t>()(coord.get_x()) ^ (std::hash<uint16_t>()(coord.get_y()) << 1);
            }
        };

        void set_direction(char d) {
            switch (d) {
                case 'U':
                    this->direction = std::bitset<2>(0);
                    break;
                case 'D':
                    this->direction = std::bitset<2>(1);
                    break;
                case 'L':
                    this->direction = std::bitset<2>(2);
                    break;
                case 'R':
                    this->direction = std::bitset<2>(3);
                    break;
                default:
                    break;
            }
        }

        char get_direction(const std::bitset<2> &dir) {
            switch (dir.to_ulong()) {
                case 0:
                    return 'U';
                case 1:
                    return 'D';
                case 2:
                    return 'L';
                case 3:
                    return 'R';
                default:
                    return 'X';
            }
        }


        state move_back(const std::bitset<2> &d, bool with_box) {
            state result = *this;
            coordinate check_box = coordinate(result.player.get_x(), result.player.get_y());
            switch (d.to_ulong()) {
                case 0:
                    result.player = result.player.lower();
                    check_box = check_box.upper();
                    break;
                case 1:
                    result.player = result.player.upper();
                    check_box = check_box.lower();
                    break;
                case 2:
                    result.player = result.player.right();
                    check_box = check_box.left();
                    break;
                case 3:
                    result.player = result.player.left();
                    check_box = check_box.right();
                    break;
                default:
                    break;
            }

            if (with_box) {
                for (auto& box : result.boxes) {
                    if (box.get_x() == check_box.get_x() && box.get_y() == check_box.get_y()) {
                        switch (d.to_ulong()) {
                            case 0:
                                box = box.lower();
                                break;
                            case 1:
                                box = box.upper();
                                break;
                            case 2:
                                box = box.right();
                                break;
                            case 3:
                                box = box.left();
                                break;
                            default:
                                break;
                        }
                    }
                }
            }

            return result;
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
    if (check_invalid(grid)) {
        return "No solution!";
    }
    state s(grid);
    if (s.check_solved(grid)) {
        return "";
    }
    std::unordered_map<std::vector<coordinate>, std::bitset<3>, state::VectorCoordinateHash> visited;
    std::queue<state> q;
    q.push(s);
    while (!q.empty()) {
        
        state current_state = q.front();
        q.pop();

        if (current_state.visit(visited)) {
            for (char direction : "DRUL") {
                state new_state = current_state.move(grid, direction);
                if (new_state.check_solved(grid)) {
                    new_state.visit(visited);
                    return new_state.get_path(visited);
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

bool check_invalid(const std::vector<std::string>& grid) {
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