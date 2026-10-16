#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "Objects.h"
#include "Structures.h"
#include "Lab.h"

class Maze;


/**
 * @brief Represents a localized 16x16 grid segment of the maze.
 * * A chunk holds its own logical grid, procedurally generated 3D props, and light sources.
 * It bakes its geometry into a single optimized Mesh (VAO/VBO/EBO) to minimize draw calls.
 */
class Chunk : public Object {
public:
    std::vector<LightData> localLamps;
    std::vector<Prop> props;

    Chunk(int gridX, int gridZ, Maze* parentMaze, Material* wallMat, Material* floorMat, Material* ceilingMat);   
    ~Chunk() override;

    void GeneratePass1_Structure();     
    void GeneratePass4_Props();     

    /**
     * @brief Bakes the logical grid into a solid 3D mesh.
     * * Iterates through the chunk's grid and generates quads ONLY for faces
     * that are exposed to empty spaces. This face-culling technique drastically
     * reduces the number of rendered triangles.
     */
    void BuildMesh();

    void Draw(const glm::mat4& view, const glm::mat4& proj) override;

    CellType GetLocalCell(int localX, int localZ) const;

    void AddQuad(std::vector<Vertex>& verts, std::vector<unsigned int>& idx,
        glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 normal);

    bool IsSolid(int localX, int localZ) const;

    int GetChunkX() const { return chunkX; }
    int GetChunkZ() const { return chunkZ; }

private:
    Maze* maze; 
    int chunkX, chunkZ;
    unsigned int floorCeilingIndicesCount = 0;

    Material* wallMaterial;
    Material* floorMaterial;
    Material* ceilingMaterial;

    CellType grid[CHUNK_SIZE][CHUNK_SIZE];

    void AddWallFace(std::vector<Vertex>& verts, std::vector<unsigned int>& idx,
        glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, glm::vec3 normal);
    void AddLintel(std::vector<Vertex>& verts, std::vector<unsigned int>& idx, int localX, int localZ);
};

class ObjectModel;

