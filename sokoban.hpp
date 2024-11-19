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
bool check_invalide(std::vector<std::string> &grid);
bool check_solved(std::vector<std::string> &grid);


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
    
    visited.insert(root.grid);


    while (!q.empty()) {
        position p;
        p = q.front();
        q.pop();

        if (check_solved(p.grid)) {
            return p.path;
        }

        position upper, lower, left, right;

        get_up(p, upper);
        get_down(p, lower);
        get_left(p, left);
        get_right(p, right);
        if (p.up != nullptr) {
            if (visited.find(p.up->grid) == visited.end()) {
                q.push(*p.up);
                visited.insert(p.up->grid);
            }
        }
        if (p.down != nullptr) {
            if (visited.find(p.down->grid) == visited.end()) {
                q.push(*p.down);
                visited.insert(p.down->grid);
            }
        }
        if (p.left != nullptr) {
            if (visited.find(p.left->grid) == visited.end()) {
                q.push(*p.left);
                visited.insert(p.left->grid);
            }
        }
        if (p.right != nullptr) {
            if (visited.find(p.right->grid) == visited.end()) {
                q.push(*p.right);
                visited.insert(p.right->grid);
            }
        }
    }
    
    return "";
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

bool check_invalide(std::vector<std::string> &grid) {
    // Check if there's a box surrounded by walls or other boxes in a way that makes it immovable
    for (size_t i = 1; i < grid.size(); ++i) {
        for (size_t j = 1; j < grid[i].size(); ++j) {
            if (grid[i][j] == 'B') {
                bool is_surrounded = true;
                if (grid[i-1][j] != '#' && grid[i-1][j] != 'B') is_surrounded = false;
                if (grid[i+1][j] != '#' && grid[i+1][j] != 'B') is_surrounded = false;
                if (grid[i][j-1] != '#' && grid[i][j-1] != 'B') is_surrounded = false;
                if (grid[i][j+1] != '#' && grid[i][j+1] != 'B') is_surrounded = false;
                if (is_surrounded) return true;
            }
        }
    }

    // Check if the number of boxes and targets match
    int boxes = 0, targets = 0;
    for (const auto& row : grid) {
        for (char c : row) {
            if (c == 'B') ++boxes;
            if (c == 'T') ++targets;
        }
    }
    if (boxes != targets) return true;

    // Check if there are more then one start position (character s)
    int start = 0;
    for (const auto& row : grid) {
        for (char c : row) {
            if (c == 's') ++start;
        }
    }
    if (start > 1) return true;

    // Check if Any box is placed in a corner or against a wall where it has no path to the target position
    // TODO this is a bit tricky, need to think about it

    return false;
}

void get_up(position& p, position& result) {
    result.grid = p.grid;
    result.path = p.path + 'U';
    for (size_t i = 1; i < p.grid.size(); ++i) {
        for (size_t j = 0; j < p.grid[i].size(); ++j) {
            if (p.grid[i][j] == 's' || p.grid[i][j] == 'S') {
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
                            result.grid[i-1][j] = (p.grid[i-1][j] == 'R') ? 'S' : 's';
                            result.grid[i][j] = (p.grid[i][j] == 'S') ? 'T' : '.';
                            p.up = &result;
                            return;
                        } else { // box pushed
                            result.grid[i-2][j] = 'B';
                            result.grid[i-1][j] = (p.grid[i-1][j] == 'T' || p.grid[i-1][j] == 'R') ? 'S' : 's'; //Changed
                            result.grid[i][j] = (p.grid[i][j] == 'S') ? 'T' : '.';
                            p.up = &result;
                            return;
                        }    
                    case 'T':
                        result.grid[i-1][j] = 'S';
                        result.grid[i][j] = '.';
                        p.up = &result;
                        return;
                    case '.':
                        result.grid[i-1][j] = 's';
                        result.grid[i][j] = (p.grid[i][j] == 'S') ? 'T' : '.'; //Changed
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
            if (p.grid[i][j] == 's' || p.grid[i][j] == 'S') {
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
                            result.grid[i+1][j] = (p.grid[i+1][j] == 'R') ? 'S' : 's';
                            result.grid[i][j] = (p.grid[i][j] == 'S') ? 'T' : '.';
                            p.down = &result;
                            return;
                        } else { // box pushed
                            result.grid[i+2][j] = 'B';
                            result.grid[i+1][j] = (p.grid[i+1][j] == 'T' || p.grid[i+1][j] == 'R') ? 'S' : 's';
                            result.grid[i][j] = (p.grid[i][j] == 'S') ? 'T' : '.';
                            p.down = &result;
                            return;
                        }    
                    case 'T':
                        result.grid[i+1][j] = 'S';
                        result.grid[i][j] = (p.grid[i][j] == 'S') ? 'T' : '.'; //Changed
                        p.down = &result;
                        return;
                    case '.':
                        result.grid[i+1][j] = 's';
                        result.grid[i][j] = (p.grid[i][j] == 'S') ? 'T' : '.'; //Changed
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
            if (p.grid[i][j] == 's' || p.grid[i][j] == 'S') {
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
                            result.grid[i][j-1] = (p.grid[i][j-1] == 'R') ? 'S' : 's';  
                            result.grid[i][j] = (p.grid[i][j] == 'S') ? 'T' : '.';
                            p.left = &result;
                            break;
                        } else { // box pushed
                            result.grid[i][j-2] = 'B';
                            result.grid[i][j-1] = (p.grid[i][j-1] == 'T' || p.grid[i][j-1] == 'R') ? 'S' : 's';
                            result.grid[i][j] = (p.grid[i][j] == 'S') ? 'T' : '.';
                            p.left = &result;
                            break;
                        }    
                    case 'T':
                        result.grid[i][j-1] = 'S';
                        result.grid[i][j] = '.';
                        p.left = &result;
                        break;
                    case '.':
                        result.grid[i][j-1] = 's';
                        result.grid[i][j] = (p.grid[i][j] == 'S') ? 'T' : '.';
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
            if (p.grid[i][j] == 's' || p.grid[i][j] == 'S') {
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
                            result.grid[i][j+1] = (p.grid[i][j+1] == 'R') ? 'S' : 's';
                            result.grid[i][j] = (p.grid[i][j] == 'S') ? 'T' : '.';
                            p.right = &result;
                            return;
                        } else { // box pushed
                            result.grid[i][j+2] = 'B';
                            result.grid[i][j+1] = (p.grid[i][j+1] == 'T' || p.grid[i][j+1] == 'R') ? 'S' : 's';
                            result.grid[i][j] = (p.grid[i][j] == 'S') ? 'T' : '.';
                            p.right = &result;
                            return;
                        }    
                    case 'T':
                        result.grid[i][j+1] = 'S';
                        result.grid[i][j] = (p.grid[i][j] == 'S') ? 'T' : '.'; //Changed
                        p.right = &result;
                        return;
                    case '.':
                        result.grid[i][j+1] = 's';
                        result.grid[i][j] = (p.grid[i][j] == 'S') ? 'T' : '.'; //Changed
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
            if (grid[i][j] == 'T' || grid[i][j] == 'B') {
                return false;
            }
        }
    }
    return true;
}
    