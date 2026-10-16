#include "Player.h"

Player::Player(glm::vec3 startPosition)
    : noClip(true), Yaw(-90.0f), Pitch(0.0f), WorldUp(glm::vec3(0.0f, 1.0f, 0.0f))
{
    MovementSpeed = gameConfig.playerSpeed;       
    MouseSensitivity = gameConfig.mouseSensitivity; 
    Position = startPosition;
    updateVectors();
}

void Player::ProcessKeyboard(Player_Movement direction, float deltaTime, Maze* maze) {
    float velocity = MovementSpeed * deltaTime;

    glm::vec3 moveDir(0.0f);

    if (noClip) {
  
        if (direction == FORWARD)  moveDir += Front;
        if (direction == BACKWARD) moveDir -= Front;
        if (direction == LEFT)     moveDir -= Right;
        if (direction == RIGHT)    moveDir += Right;
    }
    else {
       
        glm::vec3 flatFront = glm::normalize(glm::vec3(Front.x, 0.0f, Front.z));
        if (direction == FORWARD)  moveDir += flatFront;
        if (direction == BACKWARD) moveDir -= flatFront;
        if (direction == LEFT)     moveDir -= Right;
        if (direction == RIGHT)    moveDir += Right;
    }

    if (glm::length(moveDir) > 0.0f) {
        moveDir = glm::normalize(moveDir) * velocity;
    }


    if (noClip) {
        Position += moveDir;
    }
    else {
        float playerRadius = 0.25f;

        glm::vec3 nextPosX = Position + glm::vec3(moveDir.x, 0.0f, 0.0f);
        if (maze != nullptr && !maze->CheckCollision(nextPosX, playerRadius)) {
            Position.x = nextPosX.x;
        }

        glm::vec3 nextPosZ = Position + glm::vec3(0.0f, 0.0f, moveDir.z);
        if (maze != nullptr && !maze->CheckCollision(nextPosZ, playerRadius)) {
            Position.z = nextPosZ.z;
        }

        Position.y = 1.5f; 
    }
}

void Player::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    if (constrainPitch) {
        if (Pitch > 89.0f) Pitch = 89.0f;
        if (Pitch < -89.0f) Pitch = -89.0f;
    }

    updateVectors();
}

void Player::ToggleNoClip() {
    noClip = !noClip;
}

void Player::AttachCamera(Camera& camera) const {
  
    camera.Position = Position;
    camera.Front = Front;
    camera.Up = Up;
}

void Player::updateVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);

    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}