#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up) {
    Position = position;
    Front = front;
    Up = up;
}

glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(Position, Position + Front, Up);
}