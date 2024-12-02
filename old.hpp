#include <vector>
#include <string>
#include <iostream>
#include <unordered_set>
#include <queue>

// Each possible permutation of figures is represented as a node of a graph
struct position {
    std::vector<std::string> grid;
    position* up = nullptr;
    position* down = nullptr;
    position* left = nullptr;
    position* right = nullptr;
    std::string path = "";

    position& operator=(const position& other) {
        grid = other.grid;
        up = other.up;
        down = other.down;
        left = other.left;
        right = other.right;
        path = other.path;
        return *this;
    }
};

void get_up(position& p, position& result);
void get_down(position& p, position& result);
void get_left(position& p, position& result);
void get_right(position& p, position& result);
bool check_invalide(const std::vector<std::string> &grid);
bool check_solved(std::vector<std::string> &grid);
bool check_deadlock(const std::vector<std::string> &grid);


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

    position root;
    root.grid = grid;

    std::queue<position> q;
    q.push(root);

    struct GridHash {
        size_t operator()(const std::vector<std::string>& grid) const {
            size_t seed = 0;
            for (const auto& row : grid) {
                for (char c : row) {
                    seed ^= std::hash<char>{}(c) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
                }
            }
            return seed;
        }
    };
    std::unordered_set<std::vector<std::string>, GridHash> visited;
    
    // visited.insert(root.grid);


    while (!q.empty()) {

        position p;
        p = q.front();
        q.pop();

        if (check_solved(p.grid)) {
            return p.path;
        }
        
        if (check_deadlock(p.grid) || visited.find(p.grid) != visited.end()) {
            continue;
        }

        visited.insert(p.grid);

        position upper, lower, left, right;

        get_up(p, upper);
        get_down(p, lower);
        get_left(p, left);
        get_right(p, right);
        if (p.up != nullptr) {
            q.push(*p.up);
        }
        if (p.down != nullptr) {
            q.push(*p.down);
        }
        if (p.left != nullptr) {
            q.push(*p.left);
        }
        if (p.right != nullptr) {
            q.push(*p.right);
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
    "L",
    "LLRDDLLUULUUURRDLDUURRRDLLULLDDRUDDDURRRU",
    "DRUUUULUURDRRURDDDRDLDDRLULRUULD",
    "RRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRR",
    "L",
    "LRURURULUULLDRURRDLDDRUDDDLLLLURUURUL"
    "L",
    "ULDDDRDLLLLUURDLDRUUDDRRULDLU",
    "RDDDDUULLLDDLLURDRRRDRLRUULULLDLDRRLUURRDD",
    "LLLDDLRRRRRUUUULDDDRDLLLLULURUULURDRR", 
    "L", 
    "L", 
    "L", 
    "L", 
    "L" 
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
    return check_deadlock(grid);
}

bool check_deadlock(const std::vector<std::string> &grid) {
    unsigned long rows = (unsigned long) grid.size();
    unsigned long cols = (unsigned long) grid[0].size();

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

void get_up(position& p, position& result) {
    result.grid = p.grid;
    result.path = p.path + 'U';
    for (size_t i = 1; i < p.grid.size(); ++i) {
        for (size_t j = 0; j < p.grid[i].size(); ++j) {
            if (p.grid[i][j] == 'S' || p.grid[i][j] == 's') {
                switch (p.grid[i-1][j]){
                    case '#': // wall
                        p.up = nullptr;
                        return;
                    case 'B': // box
                    case 'R':
                        if (p.grid[i-2][j] == '#' || p.grid[i-2][j] == 'B') { // wall or box behind the box
                            p.up = nullptr; 
                            return;
                        } else if (p.grid[i-2][j] == 'T'){ // target behind the box
                            result.grid[i-2][j] = 'R';
                            result.grid[i-1][j] = (p.grid[i-1][j] == 'R') ? 's' : 'S';
                            result.grid[i][j] = (p.grid[i][j] == 's') ? 'T' : '.';
                            p.up = &result;
                            return;
                        } else { // box pushed
                            result.grid[i-2][j] = 'B';
                            result.grid[i-1][j] = (p.grid[i-1][j] == 'T' || p.grid[i-1][j] == 'R') ? 's' : 'S'; //Changed
                            result.grid[i][j] = (p.grid[i][j] == 's') ? 'T' : '.';
                            p.up = &result;
                            return;
                        }    
                    case 'T':
                        result.grid[i-1][j] = 's';
                        result.grid[i][j] = '.';
                        p.up = &result;
                        return;
                    case '.':
                        result.grid[i-1][j] = 'S';
                        result.grid[i][j] = (p.grid[i][j] == 's') ? 'T' : '.'; //Changed
                        p.up = &result;
                        return;
                    default:
                        return;
                }
            }
        }
    }
}

void get_down(position& p, position& result){
    result.grid = p.grid;
    result.path = p.path + "D";
    for (size_t i = 1; i < p.grid.size(); ++i) {
        for (size_t j = 0; j < p.grid[i].size(); ++j) {
            if (p.grid[i][j] == 'S' || p.grid[i][j] == 's') {
                switch (p.grid[i+1][j]){
                    case '#': // wall
                        p.down = nullptr;
                        return;
                    case 'B': // box
                    case 'R':
                        if (p.grid[i+2][j] == '#' || p.grid[i+2][j] == 'B') { // wall or box behind the box
                            p.down = nullptr; 
                            return;
                        } else if (p.grid[i+2][j] == 'T'){ // target behind the box
                            result.grid[i+2][j] = 'R';
                            result.grid[i+1][j] = (p.grid[i+1][j] == 'R') ? 's' : 'S';
                            result.grid[i][j] = (p.grid[i][j] == 's') ? 'T' : '.';
                            p.down = &result;
                            return;
                        } else { // box pushed
                            result.grid[i+2][j] = 'B';
                            result.grid[i+1][j] = (p.grid[i+1][j] == 'T' || p.grid[i+1][j] == 'R') ? 's' : 'S';
                            result.grid[i][j] = (p.grid[i][j] == 's') ? 'T' : '.';
                            p.down = &result;
                            return;
                        }    
                    case 'T':
                        result.grid[i+1][j] = 's';
                        result.grid[i][j] = (p.grid[i][j] == 's') ? 'T' : '.'; //Changed
                        p.down = &result;
                        return;
                    case '.':
                        result.grid[i+1][j] = 'S';
                        result.grid[i][j] = (p.grid[i][j] == 's') ? 'T' : '.'; //Changed
                        p.down = &result;
                        return;
                    default:
                        break;
                }
            }
        }
    }
}

void get_left(position& p, position& result){
    result.grid = p.grid;
    result.path = p.path + 'L';
    for (size_t i = 1; i < p.grid.size(); ++i) {
        for (size_t j = 1; j < p.grid[i].size(); ++j) {
            if (p.grid[i][j] == 'S' || p.grid[i][j] == 's') {
                switch (p.grid[i][j-1]){
                    case '#': // wall
                        p.left = nullptr;
                        break;
                    case 'B': // box
                    case 'R':
                        if (p.grid[i][j-2] == '#' || p.grid[i][j-2] == 'B') { // wall or box behind the box
                            p.left = nullptr; 
                            break;
                        } else if (p.grid[i][j-2] == 'T'){ // target behind the box
                            result.grid[i][j-2] = 'R';
                            result.grid[i][j-1] = (p.grid[i][j-1] == 'R') ? 's' : 'S';  
                            result.grid[i][j] = (p.grid[i][j] == 's') ? 'T' : '.';
                            p.left = &result;
                            break;
                        } else { // box pushed
                            result.grid[i][j-2] = 'B';
                            result.grid[i][j-1] = (p.grid[i][j-1] == 'T' || p.grid[i][j-1] == 'R') ? 's' : 'S';
                            result.grid[i][j] = (p.grid[i][j] == 's') ? 'T' : '.';
                            p.left = &result;
                            break;
                        }    
                    case 'T':
                        result.grid[i][j-1] = 's';
                        result.grid[i][j] = '.';
                        p.left = &result;
                        break;
                    case '.':
                        result.grid[i][j-1] = 'S';
                        result.grid[i][j] = (p.grid[i][j] == 's') ? 'T' : '.';
                        p.left = &result;
                        break;
                    default:
                        break;
                }
            }
        }
    }
}

void get_right(position& p, position& result){
    result.grid = p.grid;
    result.path = p.path + 'R';
    for (size_t i = 1; i < p.grid.size(); ++i) {
        for (size_t j = 0; j < p.grid[i].size(); ++j) {
            if (p.grid[i][j] == 'S' || p.grid[i][j] == 's') {
                switch (p.grid[i][j+1]){
                    case '#': // wall
                        p.right = nullptr;
                        return;
                    case 'B': // box
                    case 'R':
                        if (p.grid[i][j+2] == '#' || p.grid[i][j+2] == 'B') { // wall or box behind the box
                            p.right = nullptr; 
                            return;
                        } else if (p.grid[i][j+2] == 'T'){ // target behind the box
                            result.grid[i][j+2] = 'R';
                            result.grid[i][j+1] = (p.grid[i][j+1] == 'R') ? 's' : 'S';
                            result.grid[i][j] = (p.grid[i][j] == 's') ? 'T' : '.';
                            p.right = &result;
                            return;
                        } else { // box pushed
                            result.grid[i][j+2] = 'B';
                            result.grid[i][j+1] = (p.grid[i][j+1] == 'T' || p.grid[i][j+1] == 'R') ? 's' : 'S';
                            result.grid[i][j] = (p.grid[i][j] == 's') ? 'T' : '.';
                            p.right = &result;
                            return;
                        }    
                    case 'T':
                        result.grid[i][j+1] = 's';
                        result.grid[i][j] = (p.grid[i][j] == 's') ? 'T' : '.'; //Changed
                        p.right = &result;
                        return;
                    case '.':
                        result.grid[i][j+1] = 'S';
                        result.grid[i][j] = (p.grid[i][j] == 's') ? 'T' : '.'; //Changed
                        p.right = &result;
                        return;
                    default:
                        break;
                }
            }
        }
    }
}

bool check_solved(std::vector<std::string> &grid) {
    for (size_t i = 1; i < grid.size(); ++i) {
        for (size_t j = 0; j < grid[i].size(); ++j) {
            if (grid[i][j] == 'T' || grid[i][j] == 's') {
                return false;
            }
        }
    }
    return true;
}
