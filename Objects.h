#pragma once

#include "Structures.h"

/**
 * @brief Base class for all renderable entities in the scene.
 * * Provides a common interface for updating and drawing objects,
 * as well as managing their spatial transformations (position, rotation, scale)
 * and materials. Demonstrates polymorphism in the rendering pipeline.
 */
class Object {
public:
    Object(Material* mat = nullptr) : material(mat), geometry(nullptr), initialized(false), pos(0), specific(0), render(true) {}

    virtual ~Object();

    virtual void Update();
    /**
     * @brief Renders the object using the provided camera matrices.
     * @param viewMatrix The camera's view matrix (position and orientation).
     * @param projectionMatrix The camera's projection matrix (perspective).
     */
    virtual void Draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) = 0;

    void Hide(bool hide = true);
    void SetPosition(glm::vec3 position);
    glm::vec3 GetPosition();
    void SetScale(glm::vec3 scaleFactors);
    void SetRotation(float angleDegrees, glm::vec3 axis);
    void SetModelMatrix(const glm::mat4& matrix) {
        modelMatrix = matrix;
    }

protected:
    Material* material;
    Geometry* geometry;
    bool initialized;

    glm::mat4 modelMatrix;
    glm::vec3 pos;
    int specific;
    bool render;
};

class Mesh {
public:
    Geometry geometry;
    Material material;

    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const Material& mat);
    void Draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::mat4& modelMatrix);
    void Cleanup();
};

//class for models loaded via assimp
/**
 * @brief Handles loading and rendering of complex 3D models via the Assimp library.
 * * Capable of parsing models with multiple meshes, automatically extracting
 * vertices, normals, texture coordinates, and applying materials.
 */
class ObjectModel : public Object {
public:
    ObjectModel(const std::string& path);
    ~ObjectModel();

    void Draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) override;

private:
    std::vector<Mesh> meshes;
    std::string directory;

    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};

//class for own model
class ObjectTriangle : public Object {
public:
    ObjectTriangle(Material* mat = nullptr);
    void Draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) override;
};

class ObjectSkybox : public Object {
public:
    ObjectSkybox(Material* mat);
    void Draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) override;
};

class ObjectAnomaly : public Object {
public:
    ObjectAnomaly(Material* mat, glm::vec3 startPosition);
    void Update(float time);
    void Draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) override;

private:
    glm::vec3 startPos;
    glm::vec3 scaleFactors;
};