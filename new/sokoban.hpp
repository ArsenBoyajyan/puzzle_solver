#include <vector>
#include <string>
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <bitset>
#include <math.h>

using namespace std;
size_t grid_size;
bool check_invalid(const std::vector<std::string>& grid);

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

class state {
    public:
        state() : coordinates(bitset<72>()), direction(bitset<2>()), pushed(false) {}

        state(const state &other) : coordinates(other.coordinates), direction(other.direction), pushed(other.pushed) {}

        state &operator=(const state &other) {
            coordinates = other.coordinates;
            direction = other.direction;
            pushed = other.pushed;
            return *this;
        }

        bool operator==(const state &other) const {
            return coordinates == other.coordinates && direction == other.direction && pushed == other.pushed;
        }

        bool operator!=(const state &other) const {
            return !(*this == other);
        }

        void init(std::vector<std::string> &grid) {
            coordinates.reset();
            pushed = false;
            set_direction('R');
            uint16_t rows = (uint16_t) grid.size();
            uint16_t cols = (uint16_t) grid[0].size();
            grid_size = cols;
            size_t box_number = 0;
            for (uint16_t i = 0; i < rows; ++i) {
                for (uint16_t j = 0; j < cols; ++j) {
                    if (grid[i][j] == 'B' ) {
                        set_box(box_number++, j, i);
                        grid[i][j] = '.';
                    }
                    if (grid[i][j] == 'S') {
                        set_player(j, i);
                        grid[i][j] = '.';
                    }
                    if (grid[i][j] == 'R') {
                        set_box(box_number++, j, i);
                        grid[i][j] = 'T';
                    }
                }
            }
        }

        state move (const std::vector<std::string> &grid, char d) {
            state result = *this;
            coordinate current_player = get_player();
            result.set_direction(d);
            result.pushed = false;
            switch (d) {
                case 'D':
                    result.set_player(current_player.x, current_player.y + 1);
                    break;
                case 'U':
                    result.set_player(current_player.x, current_player.y - 1);
                    break;
                case 'L':
                    result.set_player(current_player.x - 1, current_player.y);
                    break;
                case 'R':
                    result.set_player(current_player.x + 1, current_player.y);
                    break;
                default:
                    result.set_player(current_player.x, current_player.y);
                    break;
            }

            current_player = result.get_player();

            if (grid[current_player.y][current_player.x] == '#') {
                return *this; // Wall, do nothing
            }
            
            for (size_t i = 0; i < 8; ++i) {
                coordinate box = get_box(i);
                if (box.x == 0 || box.y == 0) break;
                if (box.x == current_player.x && box.y == current_player.y) {
                    pushed = true;
                    switch (d) {
                        case 'D':
                            result.set_box(i, current_player.x, current_player.y + 1);
                            break;
                        case 'U':
                            result.set_box(i, current_player.x, current_player.y - 1);
                            break;
                        case 'L':
                            result.set_box(i, current_player.x - 1, current_player.y);
                            break;
                        case 'R':
                            result.set_box(i, current_player.x + 1, current_player.y);
                            break;
                        default:
                            result.set_box(i, current_player.x, current_player.y);
                            break;
                    }

                    coordinate new_box = result.get_box(i);
                    if (grid[new_box.y][new_box.x] == '#') {
                        return *this; // Wall behind the box
                    }

                    // Check for another box on the way
                    for (size_t j = 0; j < 8; ++j) {
                        if (i == j) continue;
                        coordinate other_box = result.get_box(j);
                        if (other_box.x == 0 || other_box.y == 0) break;
                        if (other_box.x == new_box.x && other_box.y == new_box.y) {
                            return *this; // Another box is in the way
                        }
                    }
                }
            }
            return result;
        }

        bool check_win(const std::vector<std::string> &grid) {
            for (size_t i = 0; i < 8; ++i) {
                coordinate box = get_box(i);
                if (box.x == 0 || box.y == 0) break;
                if (grid[box.y][box.x] != 'T') {
                    return false;
                }
            }
            return true;
        }

        bool check_deadlock(const std::vector<std::string> &grid) {
            return false;
        }

        bool visit(std::unordered_map<std::bitset<72>, std::bitset<3>> &visited) {
            if (visited.find(coordinates) != visited.end()) {
                return false; // Already visited
            } else {
                bitset<3> value;
                value.set(0, direction[0]);
                value.set(1, direction[1]);
                value.set(2, pushed);
                visited.insert({coordinates, value});
                return true;
            }
        }

        string get_path(unordered_map<std::bitset<72>, std::bitset<3>> &visited, const vector<string> &grid) {
            string result = "";
            state current = *this;
            bool with_box;
            char d = get_direction();
            result += d;
            
            while (true) {
                vector<string> new_grid;
                new_grid = current.get_grid(grid);
                for (auto &row : new_grid) {
                    cout << row << endl;
                }
                if (visited.find(current.coordinates) == visited.end()) {
                    break;
                }
                std::bitset<3> info = visited[current.coordinates];
                with_box = info[2];
                bitset<2> new_direction;
                new_direction.set(0, info[0]);
                new_direction.set(1, info[1]);
                current.direction = new_direction;
                current.pushed = with_box;
                result += d;
                current = current.move_back();
            }
            reverse(result.begin(), result.end());
            return result;
        }
    
        vector<string> get_grid(const vector<string> &grid) {
            vector<string> result = grid;
            coordinate player = get_player();
            result[player.y][player.x] = 'S';
            for (size_t i = 0; i < 8; ++i) {
                coordinate box = get_box(i);
                if (box.x == 0 || box.y == 0) break;
                result[box.y][box.x] = 'B';
                if (grid[box.y][box.x] == 'T') {
                    result[box.y][box.x] = 'R';
                }
            }
            return result;
        }
    
    private:
        bitset<72> coordinates;
        bitset<2> direction; // 0: up, 1: down, 2: left, 3: right
        bool pushed = false;

        struct coordinate {
            size_t x;
            size_t y;
        };

        coordinate get_player() const {
            coordinate result;
            size_t index = coordinates.to_ulong() & 0xFF;
            result.x = index % grid_size;
            result.y = index / grid_size;
            return result;
        }

        coordinate get_box(size_t i) {
            coordinate result;
            size_t index = (coordinates >> ((i + 1) * 8)).to_ulong() & 0xFF;
            result.x = index % grid_size;
            result.y = index / grid_size;
            return result;
        }

        void set_player(size_t x, size_t y) {
            size_t index = y * grid_size + x;
            coordinates &= ~(std::bitset<72>(0xFF)); // Mask to clear the last 8 bits
            coordinates |= std::bitset<72>(index);
        }

        void set_box(size_t i, size_t x, size_t y) {
            if (i >= 8) {
                throw out_of_range("Index out of range");
            }
            size_t index = y * grid_size + x;
            coordinates &= ~(std::bitset<72>(0xFF) << ((i + 1) * 8)); // Mask to clear 8 bits
            coordinates |= (std::bitset<72>(index) << ((i + 1) * 8));
        }

        void set_direction(char d) {
            switch (d) {
                case 'U':
                    direction = 0;
                    break;
                case 'D':
                    direction = 1;
                    break;
                case 'L':
                    direction = 2;
                    break;
                case 'R':
                    direction = 3;
                    break;
                default:
                    direction.reset();
                    break;
            }
        }


        char get_direction() {
            switch (direction.to_ulong()) {
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

        state move_back() {
            state result = *this;
            coordinate current_player = get_player();
            coordinate box_location;
            char d = get_direction();
            cout<<"direction: "<<d<<"\n";
            switch (d) {
                case 'U':
                    result.set_player(current_player.x, current_player.y + 1);
                    box_location.x = current_player.x;
                    box_location.y = current_player.y - 1;
                    break;
                case 'D':
                    result.set_player(current_player.x, current_player.y - 1);
                    box_location.x = current_player.x;
                    box_location.y = current_player.y + 1;
                    break;
                case 'L':
                    result.set_player(current_player.x + 1, current_player.y);
                    box_location.x = current_player.x - 1;
                    box_location.y = current_player.y;
                    break;
                case 'R':
                    result.set_player(current_player.x - 1, current_player.y);
                    box_location.x = current_player.x + 1;
                    box_location.y = current_player.y;
                    break;
                default:
                    break;
            }

            if (pushed) {
                for (size_t i = 0; i < 8; ++i) {
                    coordinate box = get_box(i);
                    if (box.x == 0 || box.y == 0) break;
                    if (box.x == box_location.x && box.y == box_location.y) {
                        result.set_box(i, current_player.x, current_player.y);
                    }
                }
            }
            return result;
        }

        
};

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
    state s;
    s.init(grid);
    if (s.check_win(grid)) {
        return "";
    }
    std::unordered_map<std::bitset<72>, std::bitset<3>> visited;
    std::queue<state> q;
    q.push(s);
    while (!q.empty()) {
        state current_state = q.front();
        q.pop();

        if (current_state.visit(visited)) {
            for (char direction : "DRUL") {
                if (direction == '\0') {
                    break;
                }
                state new_state = current_state.move(grid, direction);
                if (new_state.check_win(grid)) {
                    new_state.visit(visited);
                    return new_state.get_path(visited, grid);
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