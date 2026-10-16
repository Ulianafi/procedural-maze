#pragma once

#include <glm/glm.hpp>
#include "Camera.h" 
#include "Lab.h"

enum Player_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class Player {
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;


    float MovementSpeed;
    float MouseSensitivity;
    bool noClip;

    Player(glm::vec3 startPosition);

    void ProcessKeyboard(Player_Movement direction, float deltaTime, Maze* maze = nullptr);
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void ToggleNoClip();

    void AttachCamera(Camera& camera) const;

private:
    void updateVectors();
};