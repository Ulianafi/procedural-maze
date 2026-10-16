#pragma once
#include <string>

struct Config {
    int windowWidth = 1280;
    int windowHeight = 720;
    float playerSpeed = 2.8f;
    float mouseSensitivity = 0.4f;
    int renderDistance = 3;
    float fogDensity = 0.03f;
    unsigned int mazeSeed = 948352;

    void load(const std::string& filename);
};

extern Config gameConfig;