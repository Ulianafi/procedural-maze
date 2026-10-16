#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 GetViewMatrix() const;
};