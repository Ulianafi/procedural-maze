#include "Config.h"
#include <fstream>
#include <sstream>
#include <iostream>

Config gameConfig;

void Config::load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open " << filename << ", using default settings.\n";
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        if (iss >> key) {
            if (key == "WINDOW_WIDTH") iss >> windowWidth;
            else if (key == "WINDOW_HEIGHT") iss >> windowHeight;
            else if (key == "PLAYER_SPEED") iss >> playerSpeed;
            else if (key == "MOUSE_SENSITIVITY") iss >> mouseSensitivity;
            else if (key == "RENDER_DISTANCE") iss >> renderDistance;
            else if (key == "FOG_DENSITY") iss >> fogDensity;
            else if (key == "MAZE_SEED") iss >> mazeSeed;
        }
    }
    std::cout << "Config loaded successfully!\n";
}