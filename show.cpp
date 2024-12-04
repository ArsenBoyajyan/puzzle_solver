#include "sokoban.hpp"
#include <SFML/Graphics.hpp>
#include <X11/Xlib.h>
#include <fstream>
#include <vector>
#include <string>
#include <iostream>

const int TILE_SIZE = 64;

void drawMatrix(sf::RenderWindow &window, const std::vector<std::string> &matrix, 
                sf::Texture &brickTexture, sf::Texture &playerTexture, 
                sf::Texture &boxTexture, sf::Texture &targetTexture, 
                sf::Texture &pathTexture, sf::Texture &boxontargetTexture) {
    sf::Sprite sprite;

    for (int i = 0; i < matrix.size(); ++i) {
        for (int j = 0; j < matrix[i].size(); ++j) {
            switch (matrix[i][j]) {
                case '#': sprite.setTexture(brickTexture); break;
                case 'S': sprite.setTexture(playerTexture); break;
                case 'B': sprite.setTexture(boxTexture); break;
                case 'T': sprite.setTexture(targetTexture); break;
                case '.': sprite.setTexture(pathTexture); break;
                case 'R': sprite.setTexture(boxontargetTexture); break;
            }

            // Scale the sprite to match TILE_SIZE
            sprite.setScale(
                static_cast<float>(TILE_SIZE) / sprite.getTexture()->getSize().x,
                static_cast<float>(TILE_SIZE) / sprite.getTexture()->getSize().y
            );

            sprite.setPosition(j * TILE_SIZE, i * TILE_SIZE);
            window.draw(sprite);
        }
    }
}

int main() {
    // Grid initialization
    std::vector<std::string> grid;
    read_map(grid);
    std::vector<std::string> grid_copy = grid;

    std::string result = solve(grid);

    state s;
    s.init(grid_copy);

    sf::RenderWindow window(sf::VideoMode(grid[0].size() * TILE_SIZE, grid.size() * TILE_SIZE), "Sokoban Solver");

    sf::Texture brickTexture, playerTexture, boxTexture, targetTexture, pathTexture, boxontargetTexture;
    if (!brickTexture.loadFromFile("pictures/wall.jpg") ||
        !playerTexture.loadFromFile("pictures/player.jpg") ||
        !boxTexture.loadFromFile("pictures/box.jpg") ||
        !targetTexture.loadFromFile("pictures/target.jpg") ||
        !pathTexture.loadFromFile("pictures/path.jpg") ||
        !boxontargetTexture.loadFromFile("pictures/box-on-target.jpg")) {
        std::cerr << "Error loading textures!" << std::endl;
        return -1;
    }

    std::ofstream log("log.txt");
    if (log.is_open()) {
        if (result == "No solution!") {
            log << "No solution!" << std::endl;
            std::cout << "No solution!" << std::endl;
            log.close();
            return 0;
        }

        log << "Solved!!!\n";

        int i = 1;
        for (char c : result) {
            std::vector<std::string> temp_grid = s.get_grid(grid_copy);

            for (auto &line : temp_grid) {
                log << line << std::endl;
            }
            log << "-----------------------\n";

            while (window.isOpen()) {
                sf::Event event;
                while (window.pollEvent(event)) {
                    if (event.type == sf::Event::Closed) {
                        window.close();
                        return 0;
                    }
                }

                window.clear();
                drawMatrix(window, temp_grid, brickTexture, playerTexture, boxTexture, targetTexture, pathTexture, boxontargetTexture);
                window.display();

                sf::sleep(sf::seconds(0.2));
                i++;
                break;
            }

            s = s.move(temp_grid, c);
        }

        std::vector<std::string> temp_grid = s.get_grid(grid_copy);
        drawMatrix(window, temp_grid, brickTexture, playerTexture, boxTexture, targetTexture, pathTexture, boxontargetTexture);
        window.display();
        sf::sleep(sf::seconds(5));
        for (auto &line : temp_grid) {
            log << line << std::endl;
        }
        log.close();
    } else {
        std::cerr << "Unable to open log file" << std::endl;
    }

    std::cout << result << std::endl;

    return 0;
}
